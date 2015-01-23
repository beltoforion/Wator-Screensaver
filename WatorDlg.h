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

#pragma once

#include "afxwin.h"
#include "WatorWnd.h"
#include <memory>
#include "afxcmn.h"
#include "setdpi.h"


// CWatorSaverDlg Dialogfeld
class CWatorSaverDlg : public CDialog
{
public:
  static TCHAR m_szConfig[];

  CWatorSaverDlg(CWnd* pParent = NULL);	// Standardkonstruktor
 ~CWatorSaverDlg();
  
  // Dialogfelddaten
	enum { IDD = IDD_WATORSAVER_DIALOG };

  void StoreConfig();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
  virtual BOOL PreTranslateMessage(MSG* pMsg);

	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
  int  m_iBreedTime,
       m_iPixSize,
       m_iEnergyPerPrey,
       m_iEnergyMin,
       m_iTimeout;
  BOOL m_bShowStat,
       m_bShowFPS,
       m_bShowParam,
       m_bMultScreen;
  std::auto_ptr<CWatorGL> m_pWndPreview;

  void ReadConfig();
  void Preview();

  CEdit c_EnergyPerPrey;
  CEdit c_PixSize;
  CEdit c_EnergyMin;
  CEdit c_BreedTime;
  CButton c_ShowStat;
  CButton c_ShowFPS;
  CButton c_ShowParam;
  CButton c_MultScreen;
  CSliderCtrl c_Timeout;
	CSetDPI m_DPI;

  afx_msg void OnBnClickedOk();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnBnClickedApply();
  afx_msg void OnEnKillfocusPixsize();
  afx_msg void OnBnClickedStat();
  afx_msg void OnBnClickedStat2();
  afx_msg void OnBnClickedStat3();
  afx_msg void OnBnClickedStat4();
  afx_msg void OnEnKillfocusMaxage();
  afx_msg void OnEnKillfocusEnergyPerPrey();
  afx_msg void OnEnKillfocusEnergyForOffspring();
  afx_msg void OnEnChangeMaxage();
  afx_msg void OnEnChangeEnergyForOffspring();
  afx_msg void OnEnChangeEnergyPerPrey();
  afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
};
