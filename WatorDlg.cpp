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
#include "WatorApp.h"
#include "WatorDlg.h"
#include "auUtility.h"
#include <sstream>
#include ".\watordlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//######################################################################
//#                                                                    #
//#   _ _ _            _    _        ___   _ _ _        _              #
//#  | | | | ___  _ _ | | _| |  ___ | | ' | | | | ___ _| |_ ___  _ _   #
//#  | | | |/ . \| '_>| |/ . | / . \| |-  | | | |<_> | | | / . \| '_>  #
//#  |__/_/ \___/|_|  |_|\___| \___/|_|   |__/_/ <___| |_| \___/|_|    # 
//#                                                                    #									
//#                                       http://wator.beltoforion.de  #
//#                                       Version 1.1		               #
//#                                       (C) 2009 Ingo Berg           #
//#                                       ingo_berg@gmx.de             #
//#                                                                    #									
//######################################################################

//--------------------------------------------------------------------------
TCHAR CWatorSaverDlg::m_szConfig[] = _T("Config");

//--------------------------------------------------------------------------
// CWatorSaverDlg Dialogfeld
CWatorSaverDlg::CWatorSaverDlg(CWnd* pParent /*=NULL*/)
  :CDialog(CWatorSaverDlg::IDD, pParent)
  ,m_iEnergyPerPrey(0)
  ,m_iEnergyMin(0)
  ,m_pWndPreview()
  ,m_iBreedTime(0)
  ,m_iPixSize(0)
  ,m_bShowStat(TRUE)
  ,m_bShowFPS(FALSE)
  ,m_bShowParam(FALSE)
  ,m_bMultScreen(FALSE)
  ,m_iTimeout(0)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//--------------------------------------------------------------------------
CWatorSaverDlg::~CWatorSaverDlg()
{
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MAXAGE, c_BreedTime);
  DDX_Control(pDX, IDC_PIXSIZE, c_PixSize);
  DDX_Control(pDX, IDC_ENERGY_FOR_OFFSPRING, c_EnergyMin);
  DDX_Control(pDX, IDC_ENERGY_PER_PREY, c_EnergyPerPrey);
  DDX_Control(pDX, IDC_STAT, c_ShowStat);
  DDX_Control(pDX, IDC_STAT2, c_ShowFPS);
  DDX_Control(pDX, IDC_STAT3, c_ShowParam);
  DDX_Text(pDX, IDC_MAXAGE, m_iBreedTime);
  DDX_Text(pDX, IDC_PIXSIZE, m_iPixSize);
  DDX_Text(pDX, IDC_ENERGY_PER_PREY, m_iEnergyPerPrey);
  DDX_Text(pDX, IDC_ENERGY_FOR_OFFSPRING, m_iEnergyMin);
  DDV_MinMaxInt(pDX, m_iBreedTime, 1, 1000);
  DDV_MinMaxInt(pDX, m_iPixSize, 1, 20);
  DDV_MinMaxInt(pDX, m_iEnergyPerPrey, 1, 200);
  DDV_MinMaxInt(pDX, m_iEnergyMin, 1, 1000);
  DDX_Check(pDX, IDC_STAT, m_bShowStat);
  DDX_Check(pDX, IDC_STAT2, m_bShowFPS);
  DDX_Check(pDX, IDC_STAT3, m_bShowParam);
  DDX_Slider(pDX, IDC_SLIDER1, m_iTimeout);
  DDX_Control(pDX, IDC_SLIDER1, c_Timeout);
  DDX_Control(pDX, IDC_STAT4, c_MultScreen);
}

//--------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWatorSaverDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_WM_CREATE()
  ON_EN_KILLFOCUS(IDC_PIXSIZE, OnEnKillfocusPixsize)
  ON_BN_CLICKED(IDC_Apply, OnBnClickedApply)
  ON_BN_CLICKED(IDC_STAT, OnBnClickedStat)
  ON_BN_CLICKED(IDC_STAT2, OnBnClickedStat2)
  ON_BN_CLICKED(IDC_STAT3, OnBnClickedStat3)
  ON_EN_KILLFOCUS(IDC_MAXAGE, OnEnKillfocusMaxage)
  ON_EN_KILLFOCUS(IDC_ENERGY_PER_PREY, OnEnKillfocusEnergyPerPrey)
  ON_EN_KILLFOCUS(IDC_ENERGY_FOR_OFFSPRING, OnEnKillfocusEnergyForOffspring)
  ON_EN_CHANGE(IDC_MAXAGE, OnEnChangeMaxage)
  ON_EN_CHANGE(IDC_ENERGY_FOR_OFFSPRING, OnEnChangeEnergyForOffspring)
  ON_EN_CHANGE(IDC_ENERGY_PER_PREY, OnEnChangeEnergyPerPrey)
  ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
  ON_BN_CLICKED(IDC_STAT4, OnBnClickedStat4)
END_MESSAGE_MAP()


//--------------------------------------------------------------------------
// CWatorSaverDlg Meldungshandler
BOOL CWatorSaverDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  const int iDPI = 120; //96.0;
  m_DPI.Attach(AfxFindResourceHandle(MAKEINTRESOURCE(IDD), RT_DIALOG),
               m_hWnd, IDD, iDPI); // 96 is the DPI
  
  // Load Settings from registry and update the dialog elements
  ReadConfig();

  // Preview Configuration
  CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_PREVIEW);
  pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
 
  m_pWndPreview.reset(new CWatorGL(640, 480));
  m_pWndPreview->Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, NULL);
  m_pWndPreview->BringWindowToTop();
  CenterWindow();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
//	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

//---------------------------------------------------------------------------------------
/** \brief Read the saver settings from registry. */
void CWatorSaverDlg::ReadConfig()
{
  m_iBreedTime     = AfxGetApp()->GetProfileInt(m_szConfig, _T("BreedTime"), 5);
  m_iPixSize       = AfxGetApp()->GetProfileInt(m_szConfig, _T("PixSize"), 4);
  m_iEnergyPerPrey = AfxGetApp()->GetProfileInt(m_szConfig, _T("EnergyPerPrey"), 40);
  m_iEnergyMin     = AfxGetApp()->GetProfileInt(m_szConfig, _T("EnergyMin"), 60);
  m_iTimeout       = AfxGetApp()->GetProfileInt(m_szConfig, _T("Timeout"), 20);
  m_bShowStat      = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowStat"), TRUE);
  m_bShowFPS       = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowFPS"), FALSE);
  m_bShowParam     = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowParam"), FALSE);
  m_bMultScreen    = AfxGetApp()->GetProfileInt(m_szConfig, _T("MultScreen"), FALSE);

  CString sBuf;
  sBuf.Format(_T("%d"), m_iBreedTime);
  c_BreedTime.SetWindowText(sBuf);

  sBuf.Format(_T("%d"), m_iPixSize);
  c_PixSize.SetWindowText(sBuf);

  sBuf.Format(_T("%d"), m_iEnergyPerPrey);
  c_EnergyPerPrey.SetWindowText(sBuf);

  sBuf.Format(_T("%d"), m_iEnergyMin);
  c_EnergyMin.SetWindowText(sBuf);

  c_ShowStat.SetCheck(m_bShowStat);
  c_ShowFPS.SetCheck(m_bShowFPS);
  c_ShowParam.SetCheck(m_bShowParam);
  c_MultScreen.SetCheck(m_bMultScreen);
  c_Timeout.SetPos(m_iTimeout);
}

//--------------------------------------------------------------------------
// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.
void CWatorSaverDlg::OnPaint() 
{
	if (IsIconic())
	{
    CPaintDC dc(this); // Gerätekontext zum Zeichnen
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


//--------------------------------------------------------------------------
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CWatorSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::StoreConfig()
{
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("BreedTime"), m_iBreedTime);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("PixSize"), m_iPixSize);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("EnergyPerPrey"), m_iEnergyPerPrey);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("EnergyMin"), m_iEnergyMin);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("Timeout"), m_iTimeout);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("ShowStat"), m_bShowStat);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("ShowFPS"), m_bShowFPS);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("ShowParam"), m_bShowParam);
  AfxGetApp()->WriteProfileInt(m_szConfig, _T("MultScreen"), m_bMultScreen);
}


//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedOk()
{
  try
  {
    OnOK();
  }
  catch(...)
  {
    MessageBox(_T("internal error"), _T("screen saver"), MB_ICONERROR | MB_OK);
  }
}


//--------------------------------------------------------------------------
int CWatorSaverDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Fügen Sie Ihren spezialisierten Erstellcode hier ein.
  // Define the pixel format
  return 0;
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::Preview()
{
  if (!m_pWndPreview.get()) 
    return;

  StoreConfig();

  CRect rect;
	CWnd *pWnd( GetDlgItem(IDC_PREVIEW) );
  pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

  m_pWndPreview.reset( new CWatorGL(640, 480) );
  m_pWndPreview->Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, NULL);
  m_pWndPreview->BringWindowToTop();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedApply()
{
  Preview();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnKillfocusPixsize()
{
	m_iPixSize = GetDlgItemInt(IDC_PIXSIZE);
	if (m_iPixSize < 1) m_iPixSize = 1;
  Preview();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedStat()
{
  if (!m_pWndPreview.get()) return;
  m_pWndPreview->SetShowStat(m_bShowStat = c_ShowStat.GetCheck());
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedStat2()
{
  if (!m_pWndPreview.get()) return;
  m_pWndPreview->SetShowFPS(m_bShowFPS = c_ShowFPS.GetCheck());
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedStat3()
{
  if (!m_pWndPreview.get()) return;
  m_pWndPreview->SetShowParam(m_bShowParam = c_ShowParam.GetCheck());
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnBnClickedStat4()
{
  m_bMultScreen = c_MultScreen.GetCheck();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnKillfocusMaxage()
{
	if (!m_pWndPreview.get()) return;

  m_iBreedTime = GetDlgItemInt(IDC_MAXAGE);
	if (m_iBreedTime < 1) m_iBreedTime = 1;
  m_pWndPreview->SetBreedTime(m_iBreedTime);
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnKillfocusEnergyPerPrey()
{
	if (!m_pWndPreview.get()) return;

  m_iEnergyPerPrey = GetDlgItemInt(IDC_ENERGY_PER_PREY);
	if (m_iEnergyPerPrey < 1) m_iEnergyPerPrey = 1;
  m_pWndPreview->SetEnergyPerPrey(m_iEnergyPerPrey);
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnKillfocusEnergyForOffspring()
{
	if (!m_pWndPreview.get()) return;

  m_iEnergyMin = GetDlgItemInt(IDC_ENERGY_FOR_OFFSPRING);
	if (m_iEnergyMin < 1) m_iEnergyMin = 1;
  m_pWndPreview->SetEnergyMin(m_iEnergyMin);
}

//--------------------------------------------------------------------------
/** \brief Supress Enter and escape messages so they dont close the dialog. */
BOOL CWatorSaverDlg::PreTranslateMessage(MSG* pMsg)
{
  if(pMsg->message==WM_KEYDOWN)
  {
    switch(pMsg->wParam)
    {
    case VK_ESCAPE:
    case VK_RETURN:
            pMsg->wParam=NULL;
            break;
    }
  }

  return CDialog::PreTranslateMessage(pMsg);
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnChangeMaxage()
{
  OnEnKillfocusMaxage();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnChangeEnergyForOffspring()
{
  OnEnKillfocusEnergyForOffspring();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnEnChangeEnergyPerPrey()
{
  OnEnKillfocusEnergyPerPrey();
}

//--------------------------------------------------------------------------
void CWatorSaverDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
  if (!m_pWndPreview.get()) return;

  *pResult = 0;
  m_iTimeout = c_Timeout.GetPos();
  m_pWndPreview->SetSpeed(m_iTimeout);
}
