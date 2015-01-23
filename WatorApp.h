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

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole
#include "WatorWnd.h"

//-------------------------------------------------------------------------------------------
//
//  Wator Screensaver
//  (C) 2005 Ingo Berg
//
//  ingo_berg {at} gmx {dot} de
//
//-------------------------------------------------------------------------------------------

// CWatorSaverApp:
// Siehe WatorSaver.cpp für die Implementierung dieser Klasse
//

class CWatorSaverApp : public CWinApp
{
public:
	CWatorSaverApp();
 ~CWatorSaverApp();

 // Überschreibungen
	virtual BOOL InitInstance();

  // Implementierung
	DECLARE_MESSAGE_MAP()

private:
  enum ESaverMode
  {
    smCONFIG,
    smPREVIEW,
    smSAVER,
    smVOID
  };

  int m_iNumScreens;
  std::vector<RECT> m_vScreens;
  std::vector<CWatorGL*> m_vpWator;
  HWND m_hWnd;
  ESaverMode m_Mode;
  bool m_bSaverActive;

  // disable CC and AOP
  CWatorSaverApp(const CWatorSaverApp&) {}
  CWatorSaverApp& operator=(const CWatorSaverApp&) { return *this; }
  
  ESaverMode SaverInit();
};

extern CWatorSaverApp theApp;
