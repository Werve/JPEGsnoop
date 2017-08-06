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

#include "ExportTiffDlg.h"
#include "Resource.h"

CExportTiffDlg::CExportTiffDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_EXPORTTIFFDLG, pParent)
      , m_nCtlFmt(0)
{
}

void CExportTiffDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_RAD_RGB8, m_nCtlFmt);
    DDX_Text(pDX, IDC_EDIT_FNAME, m_sFname);
}

BOOL CExportTiffDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_nCtlFmt = 0;
    UpdateData(false);

    return TRUE;
}

BEGIN_MESSAGE_MAP(CExportTiffDlg, CDialog)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CExportTiffDlg, CDialog)
