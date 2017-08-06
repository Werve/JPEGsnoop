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

// Dialog box for Batch image processing
class CBatchDlg : public CDialog
{
public:
    explicit CBatchDlg(CWnd* pParent = nullptr);

    BOOL m_bProcessSubdir;
    BOOL m_bExtractAll;
    CString m_strDirSrc;
    CString m_strDirDst;

#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_BATCHDLG
    };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    afx_msg void OnBnClickedBtnDirSrcBrowse();
    afx_msg void OnBnClickedBtnDirDstBrowse();

    DECLARE_DYNAMIC(CBatchDlg)
    DECLARE_MESSAGE_MAP()
};
