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

// ====================================================================================================
// SOURCE CODE ACKNOWLEDGEMENT
// ====================================================================================================
// The following code is derived from the following project on CodeGuru:
//
//      Title:      HyperlinkStatic
//      Author:     Franz Wong
//      URL:        http://www.codeguru.com/cpp/controls/staticctrl/article.php/c5801
//      Date:       Jan 14, 2003
//
// ====================================================================================================

#include "stdafx.h"

#include "HyperlinkStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CAL! Added the following as otherwise we need
//     to set WINVER >= 0x0500
#define IDC_HAND MAKEINTRESOURCE(32649)

CHyperlinkStatic::CHyperlinkStatic() :
    _bCreateFont(false),
    _bMouseInControl(false),
    _bGetCaptionSize(false),
    _hHandCursor(LoadCursor(nullptr, IDC_HAND)),
    _hArrowCursor(LoadCursor(nullptr, IDC_ARROW))
{
}

void CHyperlinkStatic::SetHyperlink(CString strHyperlink)
{
    _strHyperlink = strHyperlink;
}

void CHyperlinkStatic::SetCaption(CString strCaption)
{
    _strCaption = strCaption;
    _bGetCaptionSize = false;
}

void CHyperlinkStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (_bGetCaptionSize == false)
        GetCaptionSize();
    if (InCaptionRange(point))
    ShellExecute(nullptr, _T("open"), _strHyperlink, nullptr, nullptr, SW_SHOWNORMAL);
    CStatic::OnLButtonDown(nFlags, point);
}

void CHyperlinkStatic::OnPaint()
{
    if (_bCreateFont == false)
    CreateFont();
    CPaintDC dc(this);
    CFont* pOldFont = (CFont*)dc.SelectObject(&_fontCaption);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0,0,255));
    dc.TextOut(0, 0, _strCaption);
    dc.SelectObject(pOldFont);
}

void CHyperlinkStatic::OnDestroy()
{
    CStatic::OnDestroy();
    _fontCaption.DeleteObject();
}

void CHyperlinkStatic::PreSubclassWindow()
{
    ModifyStyle(0, SS_NOTIFY, TRUE);
    GetWindowText(_strCaption);
    _bGetCaptionSize = false;
    CStatic::PreSubclassWindow();
}

LRESULT CHyperlinkStatic::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    lParam; // Unreferenced param
    wParam; // Unreferenced param
    _bMouseInControl = false;
    ::SetCursor(_hArrowCursor);
    return 0;
}

void CHyperlinkStatic::OnMouseMove(UINT nFlags, CPoint point)
{
    if (_bMouseInControl == false)
    {
        //Track the mouse leave event
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = GetSafeHwnd();
        tme.dwFlags = TME_LEAVE;
        _TrackMouseEvent(&tme);
        _bMouseInControl = true;
    }
    else
    {
        if (_bGetCaptionSize == false)
            GetCaptionSize();
        ::SetCursor((InCaptionRange(point)) ? _hHandCursor : _hArrowCursor);
    }
    CStatic::OnMouseMove(nFlags, point);
}

void CHyperlinkStatic::CreateFont()
{
    CFont* pFontParent = GetParent()->GetFont();
    if (pFontParent)
    {
        LOGFONT lf;
        pFontParent->GetObject(sizeof(lf), &lf);
        lf.lfUnderline = TRUE;
        _fontCaption.CreateFontIndirect(&lf);
        _bCreateFont = true;
    }
}

void CHyperlinkStatic::GetCaptionSize()
{
    if ((_bGetCaptionSize == false) && (_bCreateFont))
    {
        CClientDC dc(this);
        CFont* pOldFont = dc.SelectObject(&_fontCaption);
        _sizeCaption = dc.GetTextExtent(_strCaption);
        dc.SelectObject(pOldFont);
        _bGetCaptionSize = true;
    }
}

bool CHyperlinkStatic::InCaptionRange(CPoint& point) const
{
    if (_bGetCaptionSize == false)
        return false;
    return ((point.x >= 0) && (point.x < _sizeCaption.cx) &&
        (point.y >= 0) && (point.y < _sizeCaption.cy));
}

BEGIN_MESSAGE_MAP(CHyperlinkStatic, CStatic)
    ON_WM_LBUTTONDOWN()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()
