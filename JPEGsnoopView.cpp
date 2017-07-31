// JPEGsnoop - JPEG Image Decoder & Analysis Utility
// Copyright (C) 2017 - Calvin Hass
// http://www.impulseadventure.com/photo/jpeg-snoop.html
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "stdafx.h"

#include "JPEGsnoopView.h"
#include "JPEGsnoop.h"
#include "JPEGsnoopDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CJPEGsnoopView::CJPEGsnoopView()
{
}

CJPEGsnoopView::~CJPEGsnoopView()
{
}

BOOL CJPEGsnoopView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CRichEditView::PreCreateWindow(cs);
}

// This is called initially in the first time the view
// is drawn, and again after the contents have finished loading
// with OnOpenDocument()
void CJPEGsnoopView::OnInitialUpdate()
{
    CRichEditView::OnInitialUpdate();

    // FIXME This seems like the best place to add this call
    // as we know that the view will now be available
    // Want to allow drag & drop from Explorer into window
    DragAcceptFiles(true);

    // Set the printing margins (720 twips = 1/2 inch)
    SetMargins(CRect(720, 720, 720, 720));

    // Set the word wrap to off
    //   Other modes: WrapNone, WrapToWindow, WrapToTargetDevice
    m_nWordWrap = WrapNone;
    WrapChanged();

    GetRichEditCtrl().SetReadOnly(true);

    const COLORREF BLACK = RGB(1, 1, 1);

    // Set the default character formatting
    CHARFORMAT cf;
    COLORREF myColor = BLACK; //0x00ff00ff; // 00BBGGRR (purple)
    TCHAR szFaceName[LF_FACESIZE] = _T("Courier New");
    _tcscpy_s(cf.szFaceName, szFaceName);
    CDC* pDC = GetDC();

    // Modify the default character format
    cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_BOLD;
    cf.dwEffects = 0;
    cf.crTextColor = myColor;
    cf.bCharSet = ANSI_CHARSET;
    cf.bPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
    cf.yHeight = 12 * 72 * 20 / pDC->GetDeviceCaps(LOGPIXELSY);

    GetRichEditCtrl().SetDefaultCharFormat(cf);

    GetDocument()->SetupView((CRichEditView*)this);
}

BOOL CJPEGsnoopView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CJPEGsnoopView::OnDestroy()
{
    // Deactivate the item on destruction; this is important
    // when a splitter view is being used
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem != nullptr && pActiveItem->GetActiveView() == this)
    {
        pActiveItem->Deactivate();
        ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
    }
    CRichEditView::OnDestroy();
}

#ifdef _DEBUG
void CJPEGsnoopView::AssertValid() const
{
    CRichEditView::AssertValid();
}

void CJPEGsnoopView::Dump(CDumpContext& dc) const
{
    CRichEditView::Dump(dc);
}

CJPEGsnoopDoc* CJPEGsnoopView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CJPEGsnoopDoc)));
    return (CJPEGsnoopDoc*)m_pDocument;
}
#endif //_DEBUG

void CJPEGsnoopView::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szNextFile [MAX_PATH];

    // Get the # of files being dropped.
    UINT uNumFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

    // FIXME: For now, only support a single file but later may
    // consider somehow allowing multiple files to be processed?
    if (uNumFiles > 0)
    {
        if (DragQueryFile(hDropInfo, 0, szNextFile, MAX_PATH) > 0)
        {
            GetDocument()->OnOpenDocument(szNextFile);
        }
    }

    // Free up memory.
    DragFinish(hDropInfo);
}

BEGIN_MESSAGE_MAP(CJPEGsnoopView, CRichEditView)
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CJPEGsnoopView, CRichEditView)
