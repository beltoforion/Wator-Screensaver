/*
  WorldOfWator
  Copyright 2009 Ingo Berg

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "stdafx.h"

#include <cstdio>
#include <cassert>
#include "wuParameter.h"
#include "WatorApp.h"
#include "WatorDlg.h"
#include "WatorWnd.h"

// multiple monitor support
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------
//
//  Wator Screensaver (Version 1.01)
//  (C) 2005 Ingo Berg
//
//  ingo_berg {at} gmx {dot} de
//
//-------------------------------------------------------------------------------------------

BOOL CALLBACK MonitorEnumCallback( HMONITOR a_hMonitor,
                                   HDC a_hdcMonitor,
                                   LPRECT a_lpRect,
                                   LPARAM a_dwData )
{
  std::vector<RECT> *pVec = (std::vector<RECT>*)a_dwData;
  pVec->push_back( *a_lpRect );
  return TRUE;
}

//--------------------------------------------------------------------------
// CWatorSaverApp
BEGIN_MESSAGE_MAP(CWatorSaverApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


//--------------------------------------------------------------------------
// CWatorSaverApp-Erstellung
CWatorSaverApp::CWatorSaverApp()
  :m_hWnd(0)
  ,m_Mode(smVOID)
  ,m_vpWator()
  ,m_iNumScreens(0)
  ,m_vScreens()
{
}


//--------------------------------------------------------------------------
CWatorSaverApp::~CWatorSaverApp()
{
  for (unsigned i=0; i<m_vpWator.size(); ++i)
  {
    delete m_vpWator[i];
  }
  m_vpWator.clear();

//  _CrtDumpMemoryLeaks();
}

//--------------------------------------------------------------------------
// Das einzige wahre CWatorSaverApp-Objekt. Kaufen Sie keine billigen
// Fälschungen!
CWatorSaverApp theApp;


//--------------------------------------------------------------------------
// CWatorSaverApp Initialisierung
BOOL CWatorSaverApp::InitInstance()
{
  SetRegistryKey(_T("Wator screen saver"));
  CWinApp::InitInstance();
  BOOL bRet(FALSE);

  try
  {
    switch (SaverInit())
    {
    case smPREVIEW:
        {
          ASSERT(m_hWnd);  

          CRect rect;
          CWnd *pParentWin = CWnd::FromHandle(m_hWnd);
		      pParentWin->GetClientRect(&rect);
          
      		// Create the saver window
          ASSERT(m_vpWator.size()==0);
          CWatorGL *pWator = new CWatorGL(640, 480);
      	  pWator->Create( NULL, 
                          WS_VISIBLE | WS_CHILD, 
                          rect, 
                          pParentWin, 
                          NULL );
          m_vpWator.push_back(pWator);

          // Set the application main window
          m_pMainWnd = pWator;
          bRet = TRUE;
        }
        break;

 		// Create the saver window and show the screensaver
    case smSAVER:
        {
          m_iNumScreens = ::GetSystemMetrics(SM_CMONITORS);

          BOOL bMultScreen = AfxGetApp()->GetProfileInt( CWatorSaverDlg::m_szConfig,
                                                         _T("MultScreen"),
                                                         FALSE );
          if (!bMultScreen)
          {
            int w = ::GetSystemMetrics(SM_CXSCREEN), 
                h = ::GetSystemMetrics(SM_CYSCREEN);
            CRect rect(0, 0, w, h);

            // Create an insatnce for each screen
            CWatorGL *pWator = new CWatorGL;
            pWator->Create(rect);
            m_vpWator.push_back( pWator );

            m_pMainWnd = pWator;
          }
          else
          {
            for (int i=0; i<m_iNumScreens; ++i)
            {
              // Get the Rectangles for each screen
              EnumDisplayMonitors( NULL,
                                   NULL,
                                   MonitorEnumCallback, 
                                  (LPARAM)(&m_vScreens) );

              // Create an insatnce for each screen
              CWatorGL *pWator = new CWatorGL;
              pWator->Create(m_vScreens[i]);
              m_vpWator.push_back( pWator );

              m_pMainWnd = pWator;
            } // for all screens
          }

          bRet = TRUE;
        }
        break;

    case smCONFIG:
        {
          CWatorSaverDlg dlg;
          m_pMainWnd = &dlg;
          INT_PTR nResponse = dlg.DoModal();
	        if (nResponse == IDOK)
	        {
		        dlg.StoreConfig();
	        }
	        else if (nResponse == IDCANCEL)
	        {
	        }

          // Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	        //  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
          bRet = FALSE;
        }    
        break;

    default:
        MessageBox(NULL, _T("internal error"), _T("screen saver"), MB_ICONERROR | MB_OK);
        bRet = FALSE;
    }
  }
  catch(std::exception &e)
  {
    MessageBox(NULL, e.what(), _T("screen saver"), MB_ICONERROR | MB_OK);
  }
  catch(...)
  {
    MessageBox(NULL, _T("internal error"), _T("screen saver"), MB_ICONERROR | MB_OK);
  }

	return bRet;
}


#pragma warning(disable:4312) // reinterpret_cast: Konvertierung von int in größeren Typ "HWND"

//-------------------------------------------------------------------------------------------
CWatorSaverApp::ESaverMode CWatorSaverApp::SaverInit()
{
  // Check command line parameters
  wu::Parameter &param(wu::Parameter::GetInst());
  
  if (param.GetArgc()==1)
  {
    m_Mode = smCONFIG; 
    m_hWnd = NULL;
  }
  else
  {
    // Pop up configuration dialog
    // Parameters:
    //      "/C"      - GetForegroundWindow() shall be parent
    //      "/C ####" - decimal interpretation of #### shall be parent
    //      none      - NULL should be the parent
    if (param.IsOpt("C"))
    {
      m_Mode = smCONFIG;
      int iVal(0);
      if ( std::sscanf(param.GetOpt("C").c_str(), "%d", &iVal)==1)
      {
        m_hWnd = reinterpret_cast<HWND>(iVal);
      } 
    }

    // Run as a full screen saver
    // command line parameter:
    //    "/S"   - run full screen
    if (param.IsOpt("S"))
    {
      m_Mode = smSAVER;
    }

    // Run a preview 
    if (param.IsOpt("P") ) // || param.IsOpt("l"))
    {
      m_Mode = smPREVIEW;
      int iVal(0);
      if ( std::sscanf(param.GetOpt("p").c_str(), "%d", &iVal)==1)
        m_hWnd = reinterpret_cast<HWND>(iVal);

//      ::MessageBox(NULL, GetCommandLine(), NULL, MB_OK);
    } 
  } // if argc!=1

  return m_Mode;
}

#pragma warning(default:4312)
