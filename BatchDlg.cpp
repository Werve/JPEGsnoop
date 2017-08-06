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

#include "BatchDlg.h"
#include "FolderDlg.h"
#include "Resource.h"

CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_BATCHDLG, pParent)
      , m_bProcessSubdir(false)
      , m_bExtractAll(false)
{
}

void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_SUBDIRS, m_bProcessSubdir);
    DDX_Check(pDX, IDC_CHECK_EXT_ALL, m_bExtractAll);
    DDX_Text(pDX, IDC_EDIT_DIR_SRC, m_strDirSrc);
    DDX_Text(pDX, IDC_EDIT_DIR_DST, m_strDirDst);
}

void CBatchDlg::OnBnClickedBtnDirSrcBrowse()
{
    // Save fields first
    UpdateData(true);

    // Now request new path
    CFolderDialog myFolderDlg(nullptr);
    myFolderDlg.SetStartPath(m_strDirSrc);
    LPCITEMIDLIST myItemIdList = myFolderDlg.BrowseForFolder(_T("Select input image folder"), 0, nullptr, false);
    CString strPath = myFolderDlg.GetPathName(myItemIdList);
    if (!strPath.IsEmpty())
    {
        m_strDirSrc = strPath;
        // Also default the output log directory to same as input
        m_strDirDst = strPath;
        UpdateData(false);
    }
}

void CBatchDlg::OnBnClickedBtnDirDstBrowse()
{
    // Save fields first
    UpdateData(true);

    // Now request new path
    CFolderDialog myFolderDlg(nullptr);
    myFolderDlg.SetStartPath(m_strDirDst);
    LPCITEMIDLIST myItemIdList = myFolderDlg.BrowseForFolder(_T("Select output log folder"), 0, nullptr, false);
    CString strPath = myFolderDlg.GetPathName(myItemIdList);
    if (!strPath.IsEmpty())
    {
        m_strDirDst = strPath;
        UpdateData(false);
    }
}

BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_DIR_SRC_BROWSE, OnBnClickedBtnDirSrcBrowse)
    ON_BN_CLICKED(IDC_BTN_DIR_DST_BROWSE, OnBnClickedBtnDirDstBrowse)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CBatchDlg, CDialog)
