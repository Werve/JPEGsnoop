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

#include "CntrItem.h"
#include "JPEGsnoopDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CJPEGsnoopCntrItem::CJPEGsnoopCntrItem(REOBJECT* preo, CJPEGsnoopDoc* pContainer)
    : CRichEditCntrItem(preo, pContainer)
{
}

CJPEGsnoopCntrItem::~CJPEGsnoopCntrItem()
{
}

#ifdef _DEBUG
void CJPEGsnoopCntrItem::AssertValid() const
{
    CRichEditCntrItem::AssertValid();
}

void CJPEGsnoopCntrItem::Dump(CDumpContext& dc) const
{
    CRichEditCntrItem::Dump(dc);
}
#endif

IMPLEMENT_SERIAL(CJPEGsnoopCntrItem, CRichEditCntrItem, 0)
