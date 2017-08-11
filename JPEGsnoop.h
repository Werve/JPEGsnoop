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

#include <memory>

class CDbSigs;
class CDocLog;
class CSnoopConfig;
class CJPEGsnoopDoc;

// Define global variable for application log
extern CDocLog* glb_pDocLog;

CSnoopConfig* GetJPEGsnoopConfig();

class CJPEGsnoopApp : public CWinApp
{
public:
    CJPEGsnoopApp();
    ~CJPEGsnoopApp();

    BOOL InitInstance() override;

    // Main config options
    std::unique_ptr<CSnoopConfig> m_pAppConfig;

    // Needs to be accessed by JfifDec
    std::unique_ptr<CDbSigs> m_pDbSigs;

private:
    void MyOnFileOpen();
    void MyOnFileNew();

    void DoCmdLineCore();
    void CmdLineHelp();
    void CmdLineMessage(CString strMsg) const;
    void CmdLineDoneMessage();

    void CheckUpdates(bool bForceNow) const;
    bool CheckUpdatesWww() const;
    bool CheckEula() const;
    static CJPEGsnoopDoc* GetCurDoc();
    void DocReprocess();
    void DocImageDirty();
    void HandleAutoReprocess();

    HINSTANCE LoadAppLangResourceDLL() override;

    afx_msg void OnAppAbout();
    afx_msg void OnOptionsDhtexpand();
    afx_msg void OnOptionsMakernotes();
    afx_msg void OnUpdateOptionsDhtexpand(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsMakernotes(CCmdUI* pCmdUI);
    afx_msg void OnOptionsConfiguration();
    afx_msg void OnOptionsCheckforupdates();
    afx_msg void OnToolsManagelocaldb();
    afx_msg void OnOptionsSignaturesearch();
    afx_msg void OnUpdateOptionsSignaturesearch(CCmdUI* pCmdUI);
    afx_msg void OnScansegmentDecodeimage();
    afx_msg void OnScansegmentFullidct();
    afx_msg void OnScansegmentHistogramy();
    afx_msg void OnScansegmentDump();
    afx_msg void OnUpdateScansegmentDecodeimage(CCmdUI* pCmdUI);
    afx_msg void OnUpdateScansegmentFullidct(CCmdUI* pCmdUI);
    afx_msg void OnUpdateScansegmentHistogramy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateScansegmentDump(CCmdUI* pCmdUI);
    afx_msg void OnScansegmentNoidct();
    afx_msg void OnUpdateScansegmentNoidct(CCmdUI* pCmdUI);
    afx_msg void OnScansegmentHistogram();
    afx_msg void OnUpdateScansegmentHistogram(CCmdUI* pCmdUI);
    afx_msg void OnOptionsHideuknownexiftags();
    afx_msg void OnUpdateOptionsHideuknownexiftags(CCmdUI* pCmdUI);
    afx_msg void OnFileBatchprocess();
    afx_msg void OnOptionsRelaxedparsing();
    afx_msg void OnUpdateOptionsRelaxedparsing(CCmdUI* pCmdUI);

    bool m_bFatal; // Fatal error occurred (e.g. mem alloc)

    DECLARE_MESSAGE_MAP()
};

extern CJPEGsnoopApp theApp;
