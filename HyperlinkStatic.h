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
//      Title:      HyperlinkStatic
//      Author:     Franz Wong
//      URL:        http://www.codeguru.com/cpp/controls/staticctrl/article.php/c5801
//      Date:       Jan 14, 2003
//
// ====================================================================================================

#pragma once

class CHyperlinkStatic : public CStatic
{
public:
    CHyperlinkStatic();

    void SetHyperlink(CString strHyperlink);
    void SetCaption(CString strCaption);

protected:
    void PreSubclassWindow() override;
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

private:
    void CreateFont();
    void GetCaptionSize();
    bool InCaptionRange(CPoint& point) const;

    CString _strCaption;
    CString _strHyperlink;
    CFont _fontCaption;
    CSize _sizeCaption;
    bool _bCreateFont;
    bool _bMouseInControl;
    bool _bGetCaptionSize;
    HCURSOR _hHandCursor;
    HCURSOR _hArrowCursor;

    DECLARE_MESSAGE_MAP()
};
