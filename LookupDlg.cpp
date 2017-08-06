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

#include "LookupDlg.h"
#include "jpegsnoop.h"
#include "Resource.h"

CLookupDlg::CLookupDlg(CWnd* pParent /*=NULL*/)
    : CDialog(IDD_LOOKUP, pParent)
      , m_pClassCbLookup(nullptr)
      , m_pCbLookup(nullptr)
      , m_nPixX(0)
      , m_nPixY(0)
      , m_nTestVal(0)
      , m_nSizeX(0)
      , m_nSizeY(0)
{
}

CLookupDlg::CLookupDlg(CWnd* pParent, unsigned nSizeX, unsigned nSizeY)
    : CDialog(IDD_LOOKUP, pParent)
      , m_pClassCbLookup(nullptr)
      , m_pCbLookup(nullptr)
      , m_nPixX(0)
      , m_nPixY(0)
      , m_strOffset(_T("Not Calculated"))
      , m_nTestVal(0)
      ,m_nSizeX(nSizeX)
      ,m_nSizeY(nSizeY)
{
    m_strRngX.Format(_T("(0..%u)"), m_nSizeX - 1);
    m_strRngY.Format(_T("(0..%u)"), m_nSizeY - 1);
}

void CLookupDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_X, m_nPixX);
    DDX_Text(pDX, IDC_Y, m_nPixY);
    DDX_Text(pDX, IDC_OFFSET, m_strOffset);
    DDX_Text(pDX, IDC_RNGX, m_strRngX);
    DDX_Text(pDX, IDC_RNGY, m_strRngY);
}

// Set callback function for Buf()
void CLookupDlg::SetCbLookup(void* pClassCbLookup,
                             void (*pCbLookup)(void* pClassCbLookup, unsigned nX, unsigned nY, unsigned& nByte, unsigned& nBit)
)
{
    // Save pointer to class and function
    m_pClassCbLookup = pClassCbLookup;
    m_pCbLookup = pCbLookup;
}

void CLookupDlg::OnBnClickedBtnCalc()
{
    ASSERT(m_pCbLookup);
    unsigned nByte = 0;
    unsigned nBit = 0;
    UpdateData();

    if (m_nPixX > m_nSizeX - 1)
    {
        AfxMessageBox(_T("Pixel X coord out of range"));
    }
    else if (m_nPixY > m_nSizeY - 1)
    {
        AfxMessageBox(_T("Pixel Y coord out of range"));
    }
    else
    {
        if (m_pCbLookup)
        {
            // Use callback function for lookup
            m_pCbLookup(m_pClassCbLookup, m_nPixX, m_nPixY, nByte, nBit);
            m_strOffset.Format(_T("0x%08X : %u"), nByte, nBit);
            UpdateData(false);
        }
    }
}

BEGIN_MESSAGE_MAP(CLookupDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_CALC, OnBnClickedBtnCalc)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CLookupDlg, CDialog)
