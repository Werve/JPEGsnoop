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

#include "DbManageDlg.h"
#include "Resource.h"

CDbManageDlg::CDbManageDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_DBMANAGEDLG, pParent)
{
    m_asToInsert.SetSize(0, 10);
    m_anListBoxInd.SetSize(0, 10);
}

void CDbManageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_ctlListBox);
}

BOOL CDbManageDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    int lpiTabs[4];
    lpiTabs[0] = 150;
    lpiTabs[1] = 250;
    lpiTabs[2] = 300;
    m_ctlListBox.SetTabStops(3, lpiTabs);

    // Transfer the pending list of
    PopulateList();

    return TRUE;
}

// Prepare a custom signature entry for addition into the dialog listbox
void CDbManageDlg::InsertEntry(unsigned /*ind*/, CString strMake, CString strModel, CString strQual, CString /*strSig*/)
{
    CString strTmp;
    strTmp.Format(_T("Make: [%s]\tModel: [%s]\tQual: [%s]"), strMake.GetString(), strModel.GetString(), strQual.GetString());
    m_asToInsert.Add(strTmp);
}

// Copy entries from insert array to the listbox and record their position
void CDbManageDlg::PopulateList()
{
    // Reset the current entry index list
    m_anListBoxInd.RemoveAll();

    // Now copy entries over
    int nEntriesOrigMax = m_asToInsert.GetCount();
    for (int nInd = 0; nInd < nEntriesOrigMax; nInd++)
    {
        CString strTmp = m_asToInsert.GetAt(nInd);
        int nListPos = m_ctlListBox.AddString(strTmp);

        // Save the original index for each listbox row
        m_anListBoxInd.Add(nInd);

        // To allow for alternate implementations that don't use
        // a parallel array for the indices, save the original
        // index into the listbox rows themselves. Note that
        // I am not currently using this method as it is much
        // easier to leverage the parallel array member than use a
        // window control that disappears after DoModal().
        m_ctlListBox.SetItemData(nListPos, nInd);
    }
}

// Remove an entry from the signature list
void CDbManageDlg::OnBnClickedRemove()
{
    int nPos = m_ctlListBox.GetCurSel();

    // Check for case where no line is selected!
    if (nPos != -1)
    {
        m_ctlListBox.DeleteString(nPos);
        m_anListBoxInd.RemoveAt(nPos);
    }
}

// Remove all entries from the signature list
void CDbManageDlg::OnBnClickedRemoveall()
{
    m_anListBoxInd.RemoveAll();
    m_ctlListBox.ResetContent();
}

// Fetch the list of original indices that remain after
// any operations (such as remove).
// Note that this may be called after DoModal() so we
// can no longer query the listbox via GetItemData(),
// for example.
void CDbManageDlg::GetRemainIndices(CUIntArray& anRemain) const
{
    unsigned nNumEntries = m_anListBoxInd.GetCount();
    for (unsigned nInd = 0; nInd < nNumEntries; nInd++)
    {
        unsigned nEntryInd = m_anListBoxInd.GetAt(nInd);
        anRemain.Add(nEntryInd);
    }
}

BEGIN_MESSAGE_MAP(CDbManageDlg, CDialog)
    ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
    ON_BN_CLICKED(IDC_REMOVEALL, OnBnClickedRemoveall)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDbManageDlg, CDialog)
