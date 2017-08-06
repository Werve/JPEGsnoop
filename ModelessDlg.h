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

// Dialog box displayed while checking for updates
// Modeless dialog box
class CModelessDlg : public CDialog
{
public:
    explicit CModelessDlg(CWnd* pParent = nullptr);

    BOOL Create(UINT nID, CWnd* pWnd) override
    {
        return CDialog::Create(nID, pWnd);
    }

    void OnCancel() override;

    CString strMsg;

#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_MODELESSDLG
    };
#endif

protected:
    void PostNcDestroy() override
    {
        delete this;
    }

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CModelessDlg)
};
