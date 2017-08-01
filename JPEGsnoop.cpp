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

#include "JPEGsnoop.h"
#include "MainFrm.h"
#include "JPEGsnoopDoc.h"
#include "JPEGsnoopView.h"
#include "snoop.h"
#include "SnoopConfig.h"
#include "AboutDlg.h"
#include "SettingsDlg.h"
#include "DbManageDlg.h"
#include "TermsDlg.h"
#include "UpdateAvailDlg.h"
#include "ModelessDlg.h"
#include "NoteDlg.h"
#include "DbSigs.h"
#include "afxinet.h"            // For internet
#include "io.h"                 // For _open_osfhandle

// Global log file
CDocLog* glb_pDocLog = nullptr;

// FIXME:
// Would like to change string table JPEGsnoop.rc to add:
//   AFX_IDS_OPENFILE 0xF000 (61440) = _T("Open Image / Movie") for Open Dialog
// but linker complains error RC2151 "cannot reuse string constants"

// Display the command-line help/options summary
void CJPEGsnoopApp::CmdLineHelp()
{
    CString strMsg;
    CString strLine;
    strMsg += _T("\n");
    strLine.Format(_T("JPEGsnoop v%s\n"),VERSION_STR);
    strMsg += strLine;
    strMsg += _T("\n");
    strMsg += _T("JPEGsnoop.exe <parameters>\n");
    strMsg += _T("\n");
    strMsg += _T(" One of the following input parameters:\n");
    strMsg += _T("   -help              : Show command summary\n");
    strMsg += _T("   -i <fname_in>      : Defines input JPEG filename\n");
    strMsg += _T("   -b <dir>           : Batch process directory\n");
    strMsg += _T("   -br <dir>          : Batch process directory (recursive)\n");
    strMsg += _T(" Zero or more of the following input parameters:\n");
    strMsg += _T("   -o <fname_log>     : Defines output log filename\n");
    strMsg += _T("   -ext_all           : Extract all from file\n");
    strMsg += _T("   -ext_dht_avi       : Force insert DHT for AVI (-ext_all mode)\n");
    strMsg += _T("   -scan              : Enables Scan Segment decode\n");
    strMsg += _T("   -maker             : Enables Makernote decode\n");
    strMsg += _T("   -scandump          : Enables Scan Segment dumping\n");
    strMsg += _T("   -histo_y           : Enables luminance histogram\n");
    strMsg += _T("   -dhtexp            : Enables DHT table expansion into huffman bitstrings\n");
    strMsg += _T("   -exif_hide_unk     : Disables decoding of unknown makernotes\n");
    strMsg += _T("   -offset_start      : Decode at start of file\n");
    strMsg += _T("   -offset_srch1      : Decode at 1st SOI found in file\n");
    strMsg += _T("   -offset_srch2      : Decode at 1st SOI found after start of file\n");
    strMsg += _T("   -offset_pos <###>  : Decode from byte ### (decimal) in file\n");
    strMsg += _T("   -done              : Indicate when operations complete\n");
    strMsg += _T("\n");
    CmdLineMessage(strMsg);
}

// Command-line parser class
class CMyCommandParser : public CCommandLineInfo
{
    typedef enum
    {
        cla_idle,
        cla_input,
        cla_output,
        cla_err,
        cla_batchdir,
        cla_offset_pos
    } cla_e;

    int index;
    cla_e next_arg;
    CSnoopConfig* m_pCfg;
    CString strTmp;

public:
    explicit CMyCommandParser(CSnoopConfig* pCfg)
    {
        m_pCfg = pCfg;
        CCommandLineInfo();
        index = 0; // initializes an index to allow make a positional analysis
        next_arg = cla_idle;
    };

    void ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast) override
    {
        bLast; // Unreferenced param

        CString msg;

        // GUI mode decision
        // - Default to GUI mode
        // - If any command-line parameter identified, change to non-GUI mode
        // - Drag & drop will remain GUI mode
        bool bCmdLineDetected = false;

        switch (next_arg)
        {
        case cla_idle:
            if (bFlag && !_tcscmp(pszParam,_T("i")))
            {
                next_arg = cla_input;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("o")))
            {
                next_arg = cla_output;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("b")))
            {
                next_arg = cla_batchdir;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("br")))
            {
                m_pCfg->bCmdLineBatchRec = true;
                next_arg = cla_batchdir;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("help")))
            {
                m_nShellCommand = FileNothing;
                m_pCfg->bCmdLineHelp = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("h")))
            {
                m_nShellCommand = FileNothing;
                m_pCfg->bCmdLineHelp = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("?")))
            {
                m_nShellCommand = FileNothing;
                m_pCfg->bCmdLineHelp = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("offset_start")))
            {
                m_pCfg->eCmdLineOffset = DEC_OFFSET_START;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("offset_srch1")))
            {
                m_pCfg->eCmdLineOffset = DEC_OFFSET_SRCH1;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("offset_srch2")))
            {
                m_pCfg->eCmdLineOffset = DEC_OFFSET_SRCH2;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("offset_pos")))
            {
                m_pCfg->eCmdLineOffset = DEC_OFFSET_POS;
                m_pCfg->nCmdLineOffsetPos = 0;
                next_arg = cla_offset_pos;
                bCmdLineDetected = true;
            }

            else if (bFlag && !_tcscmp(pszParam,_T("ext_all")))
            {
                m_pCfg->bCmdLineExtractEn = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("ext_dht_avi")))
            {
                m_pCfg->bCmdLineExtractDhtAvi = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("scan")))
            {
                m_pCfg->bDecodeScanImg = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("maker")))
            {
                m_pCfg->bDecodeMaker = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("scandump")))
            {
                m_pCfg->bOutputScanDump = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("histo_y")))
            {
                m_pCfg->bHistoEn = true;
                m_pCfg->bDumpHistoY = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("dhtexp")))
            {
                m_pCfg->bOutputDHTexpand = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("exif_hide_unk")))
            {
                m_pCfg->bExifHideUnknown = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag && !_tcscmp(pszParam,_T("done")))
            {
                m_pCfg->bCmdLineDoneMsg = true;
                next_arg = cla_idle;
                bCmdLineDetected = true;
            }
            else if (bFlag)
            {
                // Unknown flag
                strTmp.Format(_T("ERROR: Unknown command-line flag [-%s]"), pszParam);
                // Don't disable dialog in non-interactive mode as this could be an important error

                // Show dialog box with info
                AfxMessageBox(strTmp);

                // And also report command line options
                next_arg = cla_err;
                m_nShellCommand = FileNothing;
                m_pCfg->bCmdLineHelp = true;
                bCmdLineDetected = true;
            }
            else
            {
                // Not a flag, so assume it is a drag & drop file open
                // Note that drag & drop doesn't appear to suffer from '-' prefix issues (see below)
                m_pCfg->bCmdLineOpenEn = true;
                m_pCfg->strCmdLineOpenFname = pszParam;

                // This will run in default mode which is GUI mode
                m_nShellCommand = FileOpen;
                m_strFileName = pszParam;
            }

            break;

        case cla_input:
            msg = _T("Input=[");
            msg += pszParam;
            msg += _T("]");

            // Handle case where filename was preceded by dash ('-')
            // Per CCommandLineInfo::ParseParam http://msdn.microsoft.com/en-us/library/bss6bxss.aspx
            // If first char is '-' or '/' then it is treated as flag and
            // the character is removed.
            //
            // Since we are in "cla_input" state due to preceding "-i" flag,
            // we should not have bFlag set here. If so, we need to re-instate the
            // character.
            //
            // NOTE: We actually roll-back the pszParam pointer as CWinApp::ParseCommandLine()
            // increments the pointer when setting bFlag=true.
            // Reference: C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\atlmfc\src\mfc\appcore.cpp
            //
            if (bFlag)
            {
                // Fixing hyphen filename prefix by rolling back the pointer
                pszParam--;
            }

            m_pCfg->bCmdLineOpenEn = true;
            m_pCfg->strCmdLineOpenFname = pszParam;

            m_nShellCommand = FileNothing;
            m_strFileName.Empty(); // Unused

            next_arg = cla_idle;
            break;

            // Batch directory processing
        case cla_batchdir:
            msg = _T("BatchDir=[");
            msg += pszParam;
            msg += _T("]");

            if (bFlag)
            {
                // Fixing hyphen filename prefix by rolling back the pointer
                pszParam--;
            }

            // Store the batch directory name
            m_pCfg->bCmdLineBatchEn = true;
            m_pCfg->strCmdLineBatchDirName = pszParam;

            // Set the ShellCommand to FileNothing so that the InitProcess()
            // call doesn't take any action. Instead, we can perform the
            // batch processing ourself.
            m_nShellCommand = FileNothing;
            m_strFileName = _T(""); // Unused

            next_arg = cla_idle;
            break;

        case cla_output:
            msg = _T("Output=[");
            msg += pszParam;
            msg += _T("]");
            if (bFlag)
            {
                // Fixing hyphen filename prefix by rolling back the pointer
                pszParam--;
            }
            m_pCfg->bCmdLineOutputEn = true;
            m_pCfg->strCmdLineOutputFname = pszParam;
            next_arg = cla_idle;
            break;

        case cla_offset_pos:
            msg = _T("OffsetPos=[");
            msg += pszParam;
            msg += _T("]");
            m_pCfg->nCmdLineOffsetPos = _ttoi(pszParam);
            next_arg = cla_idle;
            break;

        case cla_err:
        default:
            break;
        }

        // Now update to non-GUI mode if command-line param detected
        if (bCmdLineDetected)
        {
            m_pCfg->bGuiMode = false;
            m_pCfg->bInteractive = false;
        }
    }
};

CJPEGsnoopApp::CJPEGsnoopApp()
{
    // Reset fatal error flag
    m_bFatal = false;

    // Application-level config options
    m_pAppConfig = new CSnoopConfig();

    glb_pDocLog = new CDocLog();

    m_pDbSigs = new CDbSigs();
}

CJPEGsnoopApp::~CJPEGsnoopApp()
{
    // Save and then Delete
    if (m_pAppConfig != nullptr)
    {
        m_pAppConfig->RegistryStore();

        delete m_pAppConfig;
        m_pAppConfig = nullptr;
    }

    if (glb_pDocLog != nullptr)
    {
        delete glb_pDocLog;
        glb_pDocLog = nullptr;
    }

    if (m_pDbSigs != nullptr)
    {
        delete m_pDbSigs;
        m_pDbSigs = nullptr;
    }
}


CJPEGsnoopApp theApp;

// Override localization virtual function to avoid potential for
// satellite DLL hijacking
HINSTANCE CJPEGsnoopApp::LoadAppLangResourceDLL()
{
    return nullptr;
}

BOOL CJPEGsnoopApp::InitInstance()
{
    // InitCommonControls() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCommonControls();

    CWinApp::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }
    AfxEnableControlContainer();

    // Check to see if we had any fatal errors yet (e.g. mem alloc)
    if (m_bFatal)
    {
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored

    // This call actually concatenates CWinApp::m_pszAppName ("JPEGsnoop")
    // with the "REG_COMPANY_NAME" to create the full
    // key path "Software/ImpulseAdventure/JPEGsnoop/Recent File List"
    SetRegistryKey(REG_COMPANY_NAME);
    LoadStdProfileSettings(4); // Load standard INI file options (including MRU)

    m_pAppConfig->RegistryLoad();

    // Now that we've loaded the registry, assign the first-run status (from EULA)
    // Set the "First Run" flag for the Signature database to avoid warning messages
    m_pDbSigs->SetFirstRun(!m_pAppConfig->bEulaAccepted);

    // Assign defaults
    m_pAppConfig->UseDefaults();

    // Ensure that the user has previously signed the EULA
    if (!CheckEula())
    {
        return false;
    }

    // Has the user enabled checking for program updates?
    if (m_pAppConfig->bUpdateAuto)
    {
        CheckUpdates(false);
    }

    m_pAppConfig->RegistryStore();

    // Update the User database directory setting
    m_pDbSigs->SetDbDir(m_pAppConfig->strDbDir);

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views

    // FIXME:
    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CJPEGsnoopDoc),
        RUNTIME_CLASS(CMainFrame), // main SDI frame window
        RUNTIME_CLASS(CJPEGsnoopView));
    pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
    AddDocTemplate(pDocTemplate);

    // Establish GUI mode defaults that can be overridden by the
    // command line parsing results.
    m_pAppConfig->bGuiMode = true;
    m_pAppConfig->bInteractive = true;

    // Parse command line for standard shell commands, DDE, file open
    CMyCommandParser cmdInfo(m_pAppConfig);
    ParseCommandLine(cmdInfo);

    if (m_pAppConfig->bGuiMode)
    {
        // If the user has requested that we open up a file in GUI mode:
        // - Normal operation
        // - Drag & drop
        // then we need to ensure that the view has been created before we
        // enter the OnOpenDocument() in OpenDocumentFile(). The reason for this
        // is that OnOpenDocument will call AnalyzeFile() and insert the log into
        // the view. The view is only created at the end of OpenDocumentFile() as
        // a result of OnInitialUpdate().
        //
        // To achieve this, the easiest method is to force a New file operation
        // first. This ensures that we create the view (in OnInitialUpdate)
        // before we launch into any processing.
        //
        // For cosmetic reasons, also call UpdateWindow to ensure that the
        // frame is drawn fully before any further processing.
        OnFileNew();
        m_pMainWnd->ShowWindow(SW_SHOW);
        m_pMainWnd->UpdateWindow();
    }

    // Do my own ProcessShellCommand(). The following is based
    // on part of what exists in appui2.cpp with no real changes.
    // I have dropped off some of the unsupported m_nShellCommand modes.
    // Perhaps this will provide us with an easier means of extending
    // support to other batch commands later.
    // ----------------------------------------------------------
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // Now handle any other command-line directives that we haven't
    // already covered above
    if (m_pAppConfig->bGuiMode == false)
    {
        //bool  bCmdLineRet = false;
        DoCmdLineCore();
        return false;
    }

    // We only arrive here if there is a window to show (bResult)

    // The one and only window has been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand

    return TRUE;
}

// Process any command-line operations
void CJPEGsnoopApp::DoCmdLineCore()
{
    ASSERT(m_pAppConfig->bGuiMode == false);

    // Allocate the command-line processing core
    CJPEGsnoopCore* pSnoopCore = nullptr;
    pSnoopCore = new CJPEGsnoopCore;

    BOOL bStatus = false;
    // Perform processing requested by command-line
    if (m_pAppConfig->bCmdLineHelp)
    {
        // Display command-line summary
        CmdLineHelp();
    }
    else if (m_pAppConfig->bCmdLineOpenEn)
    {
        // Single file processing

        // ===================================
        // Handle "-i"
        // ===================================

        // Process the file
        bStatus = pSnoopCore->DoAnalyzeOffset(m_pAppConfig->strCmdLineOpenFname);

        if (!bStatus)
        {
            // Issues during file open
            // Error message was already added to the log file in AnalyzeOpen()
            // So just report to user in console
            CString strErr;
            strErr.Format(_T("ERROR: during open of file [%s]\n"), (LPCTSTR)m_pAppConfig->strCmdLineOpenFname);
            CmdLineMessage(strErr);
        }

        // Save the output log if enabled
        // Note that we will do this even if the input file open had
        // an issue, so that post-processing will reveal error in
        // the associated log report.
        if (theApp.m_pAppConfig->bCmdLineOutputEn)
        {
            pSnoopCore->DoLogSave(m_pAppConfig->strCmdLineOutputFname);
        }

        if (bStatus)
        {
            // Now proceed to "extract all" if requested
            if (m_pAppConfig->bCmdLineExtractEn)
            {
                CString strInputFname = m_pAppConfig->strCmdLineOpenFname;
                CString strExportFname = m_pAppConfig->strCmdLineOutputFname;
                bool bOverlayEn = false;
                bool bForceSoi = false;
                bool bForceEoi = false;
                bool bIgnoreEoi = false;
                bool bExtractAllEn = true;
                bool bDhtAviInsert = false;
                CString strOutPath; // unused

                // If the user didn't explicitly provide an output filename, default to one
                if (strExportFname.IsEmpty())
                {
                    strExportFname = strInputFname + _T(".export.jpg");
                }

                pSnoopCore->DoExtractEmbeddedJPEG(strInputFname, strExportFname, bOverlayEn, bForceSoi, bForceEoi, bIgnoreEoi, bExtractAllEn, bDhtAviInsert, strOutPath);
            } // bCmdLineExtractEn
        } // bStatus
    }
    else if (m_pAppConfig->bCmdLineBatchEn)
    {
        // Batch file processing

        // ===================================
        // Handle "-b" and "-br"
        // ===================================

        // Settings for batch operations
        CString strDirSrc = m_pAppConfig->strCmdLineBatchDirName;
        CString strDirDst = m_pAppConfig->strCmdLineBatchDirName; // Only support same dir for now
        bool bSubdirs = m_pAppConfig->bCmdLineBatchRec;
        bool bExtractAll = m_pAppConfig->bCmdLineExtractEn;

        // Generate the batch file list
        pSnoopCore->GenBatchFileList(strDirSrc, strDirDst, bSubdirs, bExtractAll);

        // Now that we've created the list of files, start processing them
        unsigned nBatchFileCount = pSnoopCore->GetBatchFileCount();

        for (unsigned nFileInd = 0; nFileInd < nBatchFileCount; nFileInd++)
        {
            // Process file
            pSnoopCore->DoBatchFileProcess(nFileInd, true, bExtractAll);
        }
    }

    // Deallocate the command-line core
    if (pSnoopCore)
    {
        delete pSnoopCore;
        pSnoopCore = nullptr;
    }

    // If requested, issue done indication to the prompt
    if (m_pAppConfig->bCmdLineDoneMsg)
    {
        CmdLineDoneMessage();
    }
}


// Report that application is finished to console
// This is an optional indicator to say that command-line
// operations have completed.
void CJPEGsnoopApp::CmdLineDoneMessage()
{
    CString strMsg = L"\n";
    strMsg += "JPEGsnoop operations complete\n";
    strMsg += "\n";
    CmdLineMessage(strMsg);
}

// Report a message to the console
//
// Note that this reporting may interject with
// the user's input to the command prompt since GUI application
// will release to command prompt immediately after launching
// and will not block.

void CJPEGsnoopApp::CmdLineMessage(CString strMsg)
{
    // Report to the console
    // REF: http://stackoverflow.com/questions/5094502/how-do-i-write-to-stdout-from-an-mfc-program
    bool bConsoleAttached = false;
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        int osfh = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), 8);
        if ((HANDLE)osfh != INVALID_HANDLE_VALUE)
        {
            *stdout = *_tfdopen(osfh, _T("a"));
            bConsoleAttached = true;
        }
    }
    if (bConsoleAttached)
    {
        // -------------------------------------------
        // Now convert from unicode for printf
        // REF: http://stackoverflow.com/questions/10578522/conversion-of-cstring-to-char

        // The number of characters in the string can be
        // less than nMaxStrLen. Null terminating character added at end.
        const size_t nMaxStrLen = 5000;
        size_t nCharsConverted = 0;
        char acMsg[nMaxStrLen];

        wcstombs_s(&nCharsConverted, acMsg,
                   strMsg.GetLength() + 1, strMsg,
                   _TRUNCATE);
        // -------------------------------------------

        // Output to console
        printf("%s", acMsg);
    }
}

// If it has been more than "nUpdateAutoDays" days since our
// last check for a recent update to the software, check the
// website for a newer version.
// - If a new version is available, the user is notified but
//   no action or changes are made. The user needs to navigate
//   to the website to download the latest version manually.
//
// INPUT:
// - bForceNow          = Do we ignore day timer and force a check now?
//
void CJPEGsnoopApp::CheckUpdates(bool bForceNow)
{
    CString strUpdateLastChk = m_pAppConfig->strUpdateLastChk;

    unsigned nCheckYear, nCheckMon, nCheckDay;
    if (strUpdateLastChk.GetLength() == 8)
    {
        nCheckYear = _tstoi(strUpdateLastChk.Mid(0, 4));
        nCheckMon = _tstoi(strUpdateLastChk.Mid(4, 2));
        nCheckDay = _tstoi(strUpdateLastChk.Mid(6, 2));
    }
    else
    {
        nCheckYear = 1980;
        nCheckMon = 1;
        nCheckDay = 1;
    }

    CTime tmeUpdateLastChk(nCheckYear, nCheckMon, nCheckDay, 0, 0, 0);
    CTime tmeToday = CTime::GetCurrentTime();
    CTimeSpan tmePeriod(m_pAppConfig->nUpdateAutoDays, 0, 0, 0);
    CTimeSpan tmeDiff = tmeToday - tmeUpdateLastChk;

    if ((bForceNow) || (tmeDiff >= tmePeriod))
    {
        CModelessDlg * pdlg = new CModelessDlg;
        pdlg->Create(IDD_MODELESSDLG, nullptr);

        CheckUpdatesWww();

        pdlg->OnCancel();

        // Update the timestamp of the last update check
        CString strCurDate = tmeToday.Format(_T("%Y%m%d"));
        m_pAppConfig->strUpdateLastChk = strCurDate;
        m_pAppConfig->Dirty();
    }
}

// Scrape the header of the web page to determine if a newer version
// of the software is available.
// - No information is sent to the website other than the current version number
//
// RETURN:
// - Success if connection to web page was OK
//
bool CJPEGsnoopApp::CheckUpdatesWww()
{
    CString strVerLatest;
    CString strDateLatest;

    CString strSubmitHost = IA_HOST;
    CString strSubmitPage = IA_UPDATES_CHK_PAGE;

    static LPCTSTR acceptTypes[2] = {_T("*/*"), nullptr};
    HINTERNET hConnection, hData;

    CString strFormData;
    CString strHeaders =
        _T("Content-Type: application/x-www-form-urlencoded");
    CString strFormat = _T("ver=%s");

    //*** Need to sanitize data for URL submission!
    // Search for "&", "?", "="
    strFormData.Format(strFormat,VERSION_STR);
    strFormData.GetLength();

    CString strTmp;

    CHAR pcBuffer[2048];
    CString strContents;
    DWORD dwRead; //dwStatus;
    HINTERNET hINet = InternetOpen(_T("JPEGsnoop/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hINet)
    {
        AfxMessageBox(_T("InternetOpen Failed"));
        return false;
    }
    try
    {
        hConnection = InternetConnect(hINet, (LPCTSTR)strSubmitHost, 80, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 1);
        if (!hConnection)
        {
            InternetCloseHandle(hINet);
            return false;
        }
        hData = HttpOpenRequest(hConnection, _T("POST"), (LPCTSTR)strSubmitPage, nullptr, nullptr, nullptr, 0, 1);
        if (!hData)
        {
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hINet);
            return false;
        }

        if (!HttpSendRequest(hData, (LPCTSTR)strHeaders, strHeaders.GetLength(), strFormData.GetBuffer(), strFormData.GetLength()))
        {
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hINet);
            AfxMessageBox(_T("ERROR: Couldn't SendRequest"));
            return false;
        }

        // Only read the first 1KB of page
        bool bScrapeDone = false;
        unsigned nScrapeLen = 0;
        unsigned nScrapeMax = 1024;
        while (!bScrapeDone)
        {
            if (!InternetReadFile(hData, pcBuffer, 255, &dwRead))
            {
                bScrapeDone = true;
            }
            else
            {
                if (dwRead == 0)
                {
                    bScrapeDone = true;
                    break;
                }
                pcBuffer[dwRead] = 0;
                strContents += pcBuffer;
                nScrapeLen += dwRead;
                if (nScrapeLen >= nScrapeMax)
                {
                    bScrapeDone = true;
                }
            }
        }

        // Parse the HTTP result and search for the latest
        // version identification string
        strContents.GetLength();

        CString strData;
        int nIndDataStart = -1;
        int nIndDataEnd = -1;
        unsigned nIndDataLen = 0;
        bool bFoundRange = true;
        nIndDataStart = strContents.Find(_T("***(")) + 4;
        nIndDataEnd = strContents.Find(_T(")***"));

        if ((nIndDataStart == -1) || (nIndDataEnd == -1))
        {
            // Couldn't find start or end
            bFoundRange = false;
        }

        if (nIndDataStart > nIndDataEnd)
        {
            // Start and End positions look wrong
            bFoundRange = false;
        }

        if (bFoundRange)
        {
            // Found start & end markers
            nIndDataLen = nIndDataEnd - nIndDataStart;
            strData = strContents.Mid(nIndDataStart, nIndDataLen);
            bool bDone = false;
            CString strCh;
            unsigned nInd = 0;
            while (!bDone)
            {
                if (nInd >= nIndDataLen)
                {
                    bDone = true;
                }
                else
                {
                    bool bTokDone = false;
                    CString strParam;
                    while (!bTokDone)
                    {
                        strCh = strData.GetAt(nInd);
                        if (strCh == _T("="))
                        {
                            bTokDone = true;
                            nInd += 2; // skip [
                        }
                        else
                        {
                            strParam += strCh;
                            nInd++;
                        }
                    }

                    bTokDone = false;
                    CString strVal;
                    while (!bTokDone)
                    {
                        strCh = strData.GetAt(nInd);
                        if (strCh == _T("]"))
                        {
                            bTokDone = true;
                            nInd += 2; // skip ,
                        }
                        else
                        {
                            strVal += strCh;
                            nInd++;
                        }
                    }

                    if (strParam == _T("latest_ver"))
                    {
                        strVerLatest = strVal;
                    }
                    if (strParam == _T("latest_date"))
                    {
                        strDateLatest = strVal;
                    }
                }
            }
        } // found range
    }
    catch (CInternetException* e)
    {
        e->ReportError();
        e->Delete();
        //AfxMessageBox(_T("EXCEPTION!"));
    }
    InternetCloseHandle(hConnection);
    InternetCloseHandle(hINet);
    InternetCloseHandle(hData);

    // Report that a later version exists!
    if (strVerLatest != VERSION_STR)
    {
        CUpdateAvailDlg dlgUpdate;
        dlgUpdate.strVerCur = VERSION_STR;
        dlgUpdate.strVerLatest = strVerLatest;
        dlgUpdate.strDateLatest = strDateLatest;
        dlgUpdate.bUpdateAutoStill = m_pAppConfig->bUpdateAuto;
        if (dlgUpdate.DoModal() == IDOK)
        {
            m_pAppConfig->bUpdateAuto = (dlgUpdate.bUpdateAutoStill) ? true : false;
            m_pAppConfig->Dirty();
        }
    }

    return true;
}


// Check the End-User License Agreement status
// - Ensure that the EULA has been previously signed
// - If EULA hasn't been signed, then show dialog box
//
// POST:
// - m_pAppConfig->bEulaAccepted
// - m_pAppConfig->bUpdateAuto
//
bool CJPEGsnoopApp::CheckEula()
{
    if (m_pAppConfig->bEulaAccepted)
    {
        return true;
    }
    CTermsDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        // They must have accepted the terms, update the
        // acceptance in the registry
        m_pAppConfig->bEulaAccepted = (dlg.bEulaOk != 0);
        m_pAppConfig->bUpdateAuto = (dlg.bUpdateAuto != 0);
        m_pAppConfig->Dirty();
        m_pAppConfig->RegistryStore();
        return true;
    }
    // They didn't accept, so quit the application
    m_pAppConfig->bEulaAccepted = false;
    m_pAppConfig->Dirty();
    m_pAppConfig->RegistryStore();

    // How to quit?
    return false;
}


// App command to run the dialog
void CJPEGsnoopApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CJPEGsnoopApp message handlers

// Present file open dialog
// - Populate file filter list with main decode formats
//
// POST:
// - Calls OpenDocumentFile()
//
void CJPEGsnoopApp::MyOnFileOpen()
{
    //bool bStatus = 0;
    TCHAR aszFilter[] =
        _T("JPEG Image (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
        _T("Thumbnail (*.thm)|*.thm|")
        _T("AVI Movie (*.avi)|*.avi|")
        _T("QuickTime Movie (*.mov)|*.mov|")
        _T("Digital Negative (*.dng)|*.dng|")
        _T("RAW Image (*.crw;*.cr2;*.nef;*.orf;*.pef)|*.crw;*.cr2;*.nef;*.orf;*.pef|")
        _T("PDF Files (*.pdf)|*.pdf|")
        _T("Photoshop Files (*.psd)|*.psd|")
        _T("All Files (*.*)|*.*||");

    /*
        CFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL,
        DWORD dwSize = 0);
        */

    // BUG: #1008
    CFileDialog FileDlg(TRUE, _T(".jpg"), nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, aszFilter);

    CString strTitle;
    CString strFileName;
    CString strPathName; // Added
    VERIFY(strTitle.LoadString(IDS_CAL_FILEOPEN));
    FileDlg.m_ofn.lpstrTitle = strTitle;

    // Extend the maximum filename length
    // Default is 64 for filename, 260 for path
    // Some users have requested support for longer filenames
    LPTSTR spFilePath = new TCHAR[501];
    spFilePath[0] = TCHAR(0);
    FileDlg.m_pOFN->lpstrFile = spFilePath;
    FileDlg.m_pOFN->nMaxFile = 500;
    FileDlg.m_pOFN->nMaxFileTitle = 300;

    // TODO: Should trap the following for exception (CommDlgExtendedError = FNERR_BUFFERTOOSMALL)
    // For reference, see: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646960%28v=vs.85%29.aspx
    if (FileDlg.DoModal() == IDOK)
    {
        strFileName = FileDlg.GetFileName();
        strPathName = FileDlg.GetPathName();
        //
        // For testing purposes, added this GetCurrentDirectory() check
        TCHAR szDirCur[501];
        GetCurrentDirectory(500, szDirCur);

        // NOTE:
        // The OpenDocumentFile() call converts any relative path to absolute path before loading the file.
        // It appears that VS 2003 was allowing me to pass strFileName into the OpenDocumentFile() call
        // and the user selected directory in the previous CFileDialog() caused a SetCurrentDirectory()
        // which made the relative to absolute conversion work.
        //
        // In VS 2012, it appears that the CFileDialog() is no longer causing SetCurrentDirectory()
        // to pick up the selected folder, so the relative to absolute conversion is returning a path
        // to the project/executable directory. Net result is that the file doesn't get opened as it
        // cannot be found. Opens from the MRU list will still work, however.
        //
        // In VS 2012, I can theoretically get CFileDialog to work in the old way if I happened to initialize COM
        // with COINIT_MULTITHREADED, but this doesn't seem recommended.
        //
        // Instead, it looks like I can simply pass in the full path (GetPathName()) and everything
        // works fine!

        //      OpenDocumentFile(strFileName);
        OpenDocumentFile(strPathName); // Added

        // if returns NULL, the user has already been alerted
    }

    if (spFilePath) { delete[] spFilePath; }
}

// Basic placeholder for OnFileNew
void CJPEGsnoopApp::MyOnFileNew()
{
    // Default
    OnFileNew();
}

// Retreive the currently active document (if it exists)
CJPEGsnoopDoc* CJPEGsnoopApp::GetCurDoc()
{
    CFrameWnd* pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
    ASSERT(pFrame);
    CJPEGsnoopDoc* pMyDoc;
    pMyDoc = (CJPEGsnoopDoc *)pFrame->GetActiveDocument();
    return pMyDoc;
}

// Reprocess the current file
// - Invokes the Reprocess action in the Document
// - Ensures that the document is available first 
void CJPEGsnoopApp::DocReprocess()
{
    CJPEGsnoopDoc * pMyDoc = GetCurDoc();
    if (pMyDoc)
    {
        pMyDoc->Reprocess();
    }
}

// Mark the image view as needing update
// - Will take effect on next "Reprocess" call.
void CJPEGsnoopApp::DocImageDirty()
{
    CJPEGsnoopDoc * pMyDoc = GetCurDoc();
    if (pMyDoc)
    {
        pMyDoc->J_ImgSrcChanged();
    }
}

// -------------------- Handle Menu actions ------------------

// Set menu item toggle for Options DHT Expand
void CJPEGsnoopApp::OnOptionsDhtexpand()
{
    if (m_pAppConfig->bOutputDHTexpand)
    {
        m_pAppConfig->bOutputDHTexpand = false;
    }
    else
    {
        m_pAppConfig->bOutputDHTexpand = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}

// Set menu item toggle for Options Makernotes
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnOptionsMakernotes()
{
    if (m_pAppConfig->bDecodeMaker)
    {
        m_pAppConfig->bDecodeMaker = false;
    }
    else
    {
        m_pAppConfig->bDecodeMaker = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}

// Set menu item toggle for Scan Segment Dump
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnScansegmentDump()
{
    if (m_pAppConfig->bOutputScanDump)
    {
        m_pAppConfig->bOutputScanDump = false;
    }
    else
    {
        m_pAppConfig->bOutputScanDump = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}


// Set menu item toggle for Scan Segment Dump
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnScansegmentDecodeimage()
{
    if (m_pAppConfig->bDecodeScanImg)
    {
        m_pAppConfig->bDecodeScanImg = false;
    }
    else
    {
        m_pAppConfig->bDecodeScanImg = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}


// Set menu item toggle for Scan Segment Histogram (Y)
// - Mark the image as dirty
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnScansegmentHistogramy()
{
    if (m_pAppConfig->bDumpHistoY)
    {
        m_pAppConfig->bDumpHistoY = false;
    }
    else
    {
        m_pAppConfig->bDumpHistoY = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    // Since changing the histogram can change the image
    // output, assume that this will cause a change and
    // require us to dirty the scan decode.
    // FIXME: If someone has scan preview on, processes once,
    // turns scan preview off, flips this option twice, and then 
    // turns scan preview on, they will need to recalc the scan.
    // Perhaps fix is to leave all of this up to ImgDec() block,
    // and have it look at the AppConfig options being sent. If
    // different (or source file changes), then it should recalc.

    // Mark image view area as needing update
    DocImageDirty();
    HandleAutoReprocess();
}


// If Automatic Reprocessing is enabled, reprocess the file
// - If Reprocessing is not enabled and the coach message has
//   not been disabled, inform user of option to enable
void CJPEGsnoopApp::HandleAutoReprocess()
{
    if (m_pAppConfig->bReprocessAuto)
    {
        DocReprocess();
    }
    else
    {
        if (m_pAppConfig->bCoachReprocessAuto)
        {
            // Show the coaching dialog
            CNoteDlg dlg;
            dlg.strMsg = COACH_REPROCESS_AUTO;
            dlg.DoModal();
            m_pAppConfig->bCoachReprocessAuto = !dlg.bCoachOff;
            m_pAppConfig->Dirty();
        }
    }
}

// Set menu item toggle for Scan Segment Full IDCT
// - Mark the image as dirty
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnScansegmentFullidct()
{
    if (m_pAppConfig->bDecodeScanImgAc)
    {
        m_pAppConfig->bDecodeScanImgAc = false;
    }
    else
    {
        m_pAppConfig->bDecodeScanImgAc = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    // Since changing the AC can change the image
    // output, assume that this will cause a change and
    // require us to dirty the scan decode.

    // Mark image view area as needing update
    DocImageDirty();
    HandleAutoReprocess();
}

// Set menu item toggle for Scan Segment No IDCT
// - Mark the image as dirty
// - Invoke reprocess if enabled
// NOTES:
// - Even though "Noidct" is the opposite of "Fullidct", the menu
//   option simply toggles the state, so we can reuse the handler.
void CJPEGsnoopApp::OnScansegmentNoidct()
{
    OnScansegmentFullidct();
}


// -------------------- Handle Menu status ------------------

// Set menu item status for Options DHT Expand
void CJPEGsnoopApp::OnUpdateOptionsDhtexpand(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bOutputDHTexpand)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Set menu item status for Options Makernotes
void CJPEGsnoopApp::OnUpdateOptionsMakernotes(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeMaker)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}


// Set menu item status for Scan Segment Dump
void CJPEGsnoopApp::OnUpdateScansegmentDump(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bOutputScanDump)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}


// Set menu item status for Scan Segment Decode Image
void CJPEGsnoopApp::OnUpdateScansegmentDecodeimage(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeScanImg)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Set menu item status for Scan Segment Histogram (Y)
// - May be disabled if Decode Scan Image or Histogram are disabled
void CJPEGsnoopApp::OnUpdateScansegmentHistogramy(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeScanImg && m_pAppConfig->bHistoEn)
    {
        pCmdUI->Enable(true);
    }
    else
    {
        pCmdUI->Enable(false);
    }
    if (m_pAppConfig->bDumpHistoY)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Set menu item status for Scan Segment Full IDCT
// - May be disabled if Decode Scan Image is disabled
void CJPEGsnoopApp::OnUpdateScansegmentFullidct(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeScanImg)
    {
        pCmdUI->Enable(true);
    }
    else
    {
        pCmdUI->Enable(false);
    }
    if (m_pAppConfig->bDecodeScanImgAc)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Set menu item status for Scan Segment No IDCT
// - May be disabled if Decode Scan Image is disabled
// NOTE:
// - Opposite functionality of Fullidct
void CJPEGsnoopApp::OnUpdateScansegmentNoidct(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeScanImg)
    {
        pCmdUI->Enable(true);
    }
    else
    {
        pCmdUI->Enable(false);
    }
    if (m_pAppConfig->bDecodeScanImgAc)
    {
        pCmdUI->SetCheck(FALSE);
    }
    else
    {
        pCmdUI->SetCheck(TRUE);
    }
}


// Options Configuration Dialog
// - Show the dialog box
// - Pre-populate with current m_pAppConfig settings
// - If user clicks OK, then revised settings are stored back
//   into the registry
void CJPEGsnoopApp::OnOptionsConfiguration()
{
    CSettingsDlg setDlg;
    setDlg.m_bUpdateAuto = m_pAppConfig->bUpdateAuto;
    setDlg.m_nUpdateChkDays = m_pAppConfig->nUpdateAutoDays;
    setDlg.m_strDbDir = m_pAppConfig->strDbDir;
    setDlg.m_bReprocessAuto = m_pAppConfig->bReprocessAuto;
    setDlg.m_bDbSubmitNet = m_pAppConfig->bDbSubmitNet;
    setDlg.m_nRptErrMaxScanDecode = m_pAppConfig->nErrMaxDecodeScan;
    //setDlg.m_bReportClip = m_pAppConfig->bStatClipEn;

    if (setDlg.DoModal() == IDOK)
    {
        m_pAppConfig->bUpdateAuto = (setDlg.m_bUpdateAuto != 0);
        m_pAppConfig->nUpdateAutoDays = setDlg.m_nUpdateChkDays;
        m_pAppConfig->strDbDir = setDlg.m_strDbDir;
        m_pAppConfig->bReprocessAuto = (setDlg.m_bReprocessAuto != 0);
        m_pAppConfig->bDbSubmitNet = (setDlg.m_bDbSubmitNet != 0);
        m_pAppConfig->nErrMaxDecodeScan = setDlg.m_nRptErrMaxScanDecode;
        //m_pAppConfig->bStatClipEn = (setDlg.m_bReportClip != 0);
        m_pAppConfig->Dirty();

        // Since the signature database needs to know of the
        // [possibly] updated user database directory, we must
        // set it here.
        m_pDbSigs->SetDbDir(m_pAppConfig->strDbDir);

        // Don't need to force registry write, as it will be done
        // later when exit app, but this ensures that it gets done.
        m_pAppConfig->RegistryStore();
    }
}

// Menu item to check for JPEGsnoop updates
void CJPEGsnoopApp::OnOptionsCheckforupdates()
{
    CheckUpdates(true);
}


// Manage Local Signature Database
// - Present user with database dialog box
// - This dialog enables the user to view and delete entries that are
//   defined and stored within the current installation file.
void CJPEGsnoopApp::OnToolsManagelocaldb()
{
    CDbManageDlg manageDlg;
    CJPEGsnoopDoc * pMyDoc = GetCurDoc();
    if (pMyDoc)
    {
        pMyDoc->J_ImgSrcChanged();

        CString strTmp;
        unsigned nDbUserEntries = m_pDbSigs->DatabaseExtraGetNum();

        for (unsigned nInd = 0; nInd < nDbUserEntries; nInd++)
        {
            CompSig sMySig = m_pDbSigs->DatabaseExtraGet(nInd);

            // TODO: Should we confirm that all entries are marked as valid already?

            if (sMySig.eEditor == ENUM_SOURCE_SW)
            {
                manageDlg.InsertEntry(nInd, sMySig.strMSwDisp,_T(""), sMySig.strUmQual, sMySig.strCSig);
            }
            else
            {
                manageDlg.InsertEntry(nInd, sMySig.strXMake, sMySig.strXModel, sMySig.strUmQual, sMySig.strCSig);
            }
        }

        if (manageDlg.DoModal() == IDOK)
        {
            // Now determine which entries were deleted
            // Set all flags to invalid
            // Step through remaining list from dialog and fetch indices
            // For each remaining index, set the corresponding valid indicator in the main list
            unsigned nNumExtra = m_pDbSigs->DatabaseExtraGetNum();
            for (unsigned nInd = 0; nInd < nNumExtra; nInd++)
            {
                m_pDbSigs->SetEntryValid(nInd, false);
            }
            CUIntArray anRemain;
            manageDlg.GetRemainIndices(anRemain);
            for (unsigned nInd = 0; nInd < (unsigned)anRemain.GetCount(); nInd++)
            {
                unsigned nRemainInd = anRemain.GetAt(nInd);
                m_pDbSigs->SetEntryValid(nRemainInd, true);
            }

            m_pDbSigs->DatabaseExtraStore();
            m_pDbSigs->DatabaseExtraLoad();
        }
    }
}

// Set menu item toggle for Options Signature Search
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnOptionsSignaturesearch()
{
    if (m_pAppConfig->bSigSearch)
    {
        m_pAppConfig->bSigSearch = false;
    }
    else
    {
        m_pAppConfig->bSigSearch = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();
    HandleAutoReprocess();
}

// Set menu item status for Options Signature Search
void CJPEGsnoopApp::OnUpdateOptionsSignaturesearch(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bSigSearch)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}


// Set menu item toggle for Scan Segment Histogram
// - Mark the image as dirty
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnScansegmentHistogram()
{
    if (m_pAppConfig->bHistoEn)
    {
        m_pAppConfig->bHistoEn = false;
    }
    else
    {
        m_pAppConfig->bHistoEn = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    // Since changing the histogram can change the image
    // output, assume that this will cause a change and
    // require us to dirty the scan decode.
    // FIXME: If someone has scan preview on, processes once,
    // turns scan preview off, flips this option twice, and then 
    // turns scan preview on, they will need to recalc the scan.
    // Perhaps fix is to leave all of this up to ImgDec() block,
    // and have it look at the AppConfig options being sent. If
    // different (or source file changes), then it should recalc.

    // Mark image view area as needing update
    DocImageDirty();
    HandleAutoReprocess();
}

// Set menu item status for Options Signature Search
// - May be disabled if Decode Scan Image is disabled
void CJPEGsnoopApp::OnUpdateScansegmentHistogram(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bDecodeScanImg)
    {
        pCmdUI->Enable(true);
    }
    else
    {
        pCmdUI->Enable(false);
    }
    if (m_pAppConfig->bHistoEn)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Set menu item toggle for Options Hide Unknown EXIF tags
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnOptionsHideuknownexiftags()
{
    if (m_pAppConfig->bExifHideUnknown)
    {
        m_pAppConfig->bExifHideUnknown = false;
    }
    else
    {
        m_pAppConfig->bExifHideUnknown = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}

// Set menu item status for Options Hide Unknown EXIF tags
void CJPEGsnoopApp::OnUpdateOptionsHideuknownexiftags(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bExifHideUnknown)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

// Menu item for File Batch Process
// - Invoke DoBatchProcess() from Document
void CJPEGsnoopApp::OnFileBatchprocess()
{
    CJPEGsnoopDoc * pMyDoc = GetCurDoc();
    if (pMyDoc)
    {
        pMyDoc->DoBatchProcess(_T(""), false, false);
    }
}

// Set menu item toggle for Options Relaxed Parsing
// - Invoke reprocess if enabled
void CJPEGsnoopApp::OnOptionsRelaxedparsing()
{
    if (m_pAppConfig->bRelaxedParsing)
    {
        m_pAppConfig->bRelaxedParsing = false;
    }
    else
    {
        m_pAppConfig->bRelaxedParsing = true;
    }
    // Mark option as changed for next registry update
    m_pAppConfig->Dirty();

    HandleAutoReprocess();
}

// Set menu item status for Options Relaxed Parsing
void CJPEGsnoopApp::OnUpdateOptionsRelaxedparsing(CCmdUI* pCmdUI)
{
    if (m_pAppConfig->bRelaxedParsing)
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

BEGIN_MESSAGE_MAP(CJPEGsnoopApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, MyOnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    ON_COMMAND(ID_OPTIONS_DHTEXPAND, OnOptionsDhtexpand)
    ON_COMMAND(ID_OPTIONS_MAKERNOTES, OnOptionsMakernotes)
    ON_COMMAND(ID_OPTIONS_CONFIGURATION, OnOptionsConfiguration)
    ON_COMMAND(ID_OPTIONS_CHECKFORUPDATES, OnOptionsCheckforupdates)
    ON_COMMAND(ID_OPTIONS_SIGNATURESEARCH, OnOptionsSignaturesearch)
    ON_COMMAND(ID_TOOLS_MANAGELOCALDB, OnToolsManagelocaldb)
    ON_COMMAND(ID_SCANSEGMENT_DECODEIMAGE, OnScansegmentDecodeimage)
    ON_COMMAND(ID_SCANSEGMENT_FULLIDCT, OnScansegmentFullidct)
    ON_COMMAND(ID_SCANSEGMENT_HISTOGRAMY, OnScansegmentHistogramy)
    ON_COMMAND(ID_SCANSEGMENT_DUMP, OnScansegmentDump)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_DHTEXPAND, OnUpdateOptionsDhtexpand)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_MAKERNOTES, OnUpdateOptionsMakernotes)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_SIGNATURESEARCH, OnUpdateOptionsSignaturesearch)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_DECODEIMAGE, OnUpdateScansegmentDecodeimage)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_FULLIDCT, OnUpdateScansegmentFullidct)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_HISTOGRAMY, OnUpdateScansegmentHistogramy)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_DUMP, OnUpdateScansegmentDump)
    ON_COMMAND(ID_SCANSEGMENT_NOIDCT, OnScansegmentNoidct)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_NOIDCT, OnUpdateScansegmentNoidct)
    ON_COMMAND(ID_SCANSEGMENT_HISTOGRAM, OnScansegmentHistogram)
    ON_UPDATE_COMMAND_UI(ID_SCANSEGMENT_HISTOGRAM, OnUpdateScansegmentHistogram)
    ON_COMMAND(ID_OPTIONS_HIDEUKNOWNEXIFTAGS, OnOptionsHideuknownexiftags)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_HIDEUKNOWNEXIFTAGS, OnUpdateOptionsHideuknownexiftags)
    ON_COMMAND(ID_FILE_BATCHPROCESS, OnFileBatchprocess)
    ON_COMMAND(ID_OPTIONS_RELAXEDPARSING, &CJPEGsnoopApp::OnOptionsRelaxedparsing)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_RELAXEDPARSING, &CJPEGsnoopApp::OnUpdateOptionsRelaxedparsing)
END_MESSAGE_MAP()
