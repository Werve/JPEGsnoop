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

#pragma once

#include "JPEGsnoopDoc.h"

class CJPEGsnoopCntrItem;

// JPEGsnoop SDI View class for the main text log window
class CJPEGsnoopView : public CRichEditView
{
public:
    virtual ~CJPEGsnoopView();

    CJPEGsnoopDoc* GetDocument() const;
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

protected:
    // create from serialization only
    CJPEGsnoopView();
    void OnInitialUpdate() override; // called first time after construct
    BOOL OnPreparePrinting(CPrintInfo* pInfo) override;

    afx_msg void OnDestroy();
    afx_msg void OnDropFiles(HDROP hDropInfo);

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CJPEGsnoopView)
};

#ifndef _DEBUG  // debug version in JPEGsnoopView.cpp
inline CJPEGsnoopDoc* CJPEGsnoopView::GetDocument() const
{
    return reinterpret_cast<CJPEGsnoopDoc*>(m_pDocument);
}
#endif
