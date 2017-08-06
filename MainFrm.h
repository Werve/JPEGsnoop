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

// JPEGsnoop main frame window
// - Splitter window
// - Status bar
class CMainFrame : public CFrameWnd
{
public:
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

protected:
    // create from serialization only
    CMainFrame();
    BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

private:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    static int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);

    CSplitterWnd m_mainSplitter;
    BOOL m_bInitSplitter;
    CStatusBar m_wndStatusBar;
    CToolBar m_wndToolBar;

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CMainFrame)
};
