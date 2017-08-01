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

class CJPEGsnoopDoc;
class CJPEGsnoopView;

class CJPEGsnoopCntrItem : public CRichEditCntrItem
{
public:
    explicit CJPEGsnoopCntrItem(REOBJECT* preo = nullptr, CJPEGsnoopDoc* pContainer = nullptr);
    // Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
    //  IMPLEMENT_SERIALIZE requires the class have a constructor with
    //  zero arguments.  Normally, OLE items are constructed with a
    //  non-NULL document pointer

    CJPEGsnoopDoc* GetDocument()
    {
        return reinterpret_cast<CJPEGsnoopDoc*>(CRichEditCntrItem::GetDocument());
    }

    CJPEGsnoopView* GetActiveView()
    {
        return reinterpret_cast<CJPEGsnoopView*>(CRichEditCntrItem::GetActiveView());
    }

#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif

    DECLARE_SERIAL(CJPEGsnoopCntrItem)
};
