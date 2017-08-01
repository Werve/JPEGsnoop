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

#include <afxwin.h>
#include "Resource.h"

// Dialog box for adjusting the starting file decode offset
class COffsetDlg : public CDialog
{
public:
    explicit COffsetDlg(CWnd* pParent = nullptr);

    void SetOffset(unsigned nPos);
    unsigned GetOffset();

    // Dialog Data
    enum
    {
        IDD = IDD_OFFSETDLG
    };

protected:
    void DoDataExchange(CDataExchange* pDX) override;

private:
    void OffsetNum2Str();
    bool OffsetStr2Num();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedBaseh();
    afx_msg void OnBnClickedBased();

    unsigned m_nOffsetVal;
    int m_nRadioBaseMode;
    unsigned m_nBaseMode;
    CString m_sOffsetVal;

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(COffsetDlg)
};
