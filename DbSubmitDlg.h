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

#include "afxwin.h"
#include "Resource.h"

// Dialog box that enables user to submit a new signature to the database
class CDbSubmitDlg : public CDialog
{
public:
    explicit CDbSubmitDlg(CWnd* pParent = nullptr);
    virtual ~CDbSubmitDlg();

    CString m_strSig;
    CString m_strQual;
    CString m_strUserSoftware;
    CString m_strExifModel;
    CString m_strExifSoftware;
    CString m_strExifMake;
    int m_nSource;
    CString m_strNotes;

    // Dialog Data
    enum
    {
        IDD = IDD_DBSUBMITDLG
    };

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    afx_msg void OnBnClickedOk();

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CDbSubmitDlg)
};
