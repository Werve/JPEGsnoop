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

// Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0601

// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN

// some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

// Disable implicit narrow wide conversion.
#define _CSTRING_DISABLE_NARROW_WIDE_CONVERSION

// Only use the new Unicode <-> Ansi conversion macro's.
#define _ATL_EX_CONVERSION_MACROS_ONLY

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>         // MFC support for Windows Common Controls
#include <afxrich.h>        // MFC rich edit classes
#include <afxinet.h>

// Redefining C++ keywords is in general a bad practice.
// The Microsoft C++ headers file are designed to handle redefining to the debug MFC allocator.
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
