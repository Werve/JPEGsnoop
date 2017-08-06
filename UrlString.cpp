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

// ====================================================================================================
// SOURCE CODE ACKNOWLEDGEMENT
// ====================================================================================================
// The following code is derived from the following project on CodeGuru:
//
//      Title:      CUrlString: A very simple MFC class to Encode and Decode an url string
//      Author:     Stephane Erhardt
//      URL:        http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4029/URL-Encoding.htm (comments)
//      Date:       Mar 03, 2006
//
// ====================================================================================================

#include "stdafx.h"

#include "UrlString.h"

CUrlString::CUrlString()
{
    m_csUnsafe = _T("%=\"<>\\^[]`+$,@:;/!#?&'");
    for (wchar_t iChar = 1; iChar < 33; iChar++)
        m_csUnsafe += iChar;
    for (wchar_t iChar = 124; iChar < 256; iChar++)
        m_csUnsafe += iChar;
}

CString CUrlString::Encode(CString csDecoded) const
{
    CString csCharEncoded;
    CString csEncoded = csDecoded;

    for (int iPos = 0; iPos < m_csUnsafe.GetLength(); iPos++)
    {
        csCharEncoded.Format(_T("%%%02X"), m_csUnsafe[iPos]);
        CString csCharDecoded(m_csUnsafe[iPos]);
        csEncoded.Replace(csCharDecoded, csCharEncoded);
    }
    return csEncoded;
}

CString CUrlString::Decode(CString csEncoded) const
{
    CString csUnsafeEncoded = Encode(m_csUnsafe);
    CString csDecoded = csEncoded;

    for (int iPos = 0; iPos < csUnsafeEncoded.GetLength(); iPos += 3)
    {
        CString csCharEncoded = csUnsafeEncoded.Mid(iPos, 3);
        CString csCharDecoded(static_cast<TCHAR>(_tcstol(csUnsafeEncoded.Mid(iPos + 1, 2), nullptr, 16)));
        csDecoded.Replace(csCharEncoded, csCharDecoded);
    }
    return csDecoded;
}
