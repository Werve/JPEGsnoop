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

// - Dialog box for configuring a file decoder overlay
// - Enables local overwrite of the file buffer during decode for testing purposes
class COverlayBufDlg : public CDialog
{
public:
    explicit COverlayBufDlg(CWnd* pParent = nullptr);
    COverlayBufDlg(CWnd* pParent,
                   bool bEn, unsigned nOffset, unsigned nLen, CString sNewHex, CString sNewBin);

    unsigned m_nOffset;
    unsigned m_nLen;
    bool m_bApply; // When OnOK(), indicate apply and redo dialog
    BOOL m_bEn;
    CString m_sValueNewHex;

    // Callback function for Buf()
    void SetCbBuf(void* pClassCbBuf,
        BYTE(*pCbBuf)(void* pClassCbBuf, unsigned long nNum, bool bBool));

#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_OVERLAYBUFDLG
    };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;

private:
    afx_msg void OnBnClickedOvrLoad();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedApply();
    BOOL OnInitDialog() override;

    // References to callback function for Buf()
    void* m_pClassCbBuf;
    BYTE (*m_pCbBuf)(void* pClassCbBuf, unsigned long nNum, bool bBool);

    CString m_sOffset;
    CString m_sValueCurHex;
    CString m_sValueCurBin;
    CString m_sValueNewBin;

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(COverlayBufDlg)
};
