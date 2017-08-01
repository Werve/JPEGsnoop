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

#include "Resource.h"

// Dialog box providing options for exporting to JPEG
class CExportDlg : public CDialog
{
public:
    explicit CExportDlg(CWnd* pParent = nullptr);

    BOOL m_bOverlayEn;
    BOOL m_bDhtAviInsert;
    CString m_strOffsetStart;
    BOOL m_bForceEoi;
    BOOL m_bIgnoreEoi;
    BOOL m_bForceSoi;
    BOOL m_bExtractAllEn;

    // Dialog Data
    enum
    {
        IDD = IDD_EXPORTDLG
    };

protected:
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CExportDlg)
};
