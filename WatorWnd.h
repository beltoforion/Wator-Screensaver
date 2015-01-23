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
#ifndef WATOR_GL_H
#define WATOR_GL_H

#include "StdAfx.h"
#include "auStatBuffer.h"
#include "auUtility.h"


//---------------------------------------------------------------------------------------
// Wator predator prey simulation
//
class CWatorGL : public CWnd
{
private:
  typedef std::basic_string<TCHAR> string_type;

public:
	static LPCTSTR m_lpszClassName;

  CWatorGL();
  CWatorGL(int a_iSizeX, int a_iSizeY);
 ~CWatorGL();

  virtual BOOL Create(DWORD dwExStyle, 
                      DWORD dwStyle, 
                      const RECT& rect,
	                    CWnd* pParentWnd, 
                      UINT nID, 
                      CCreateContext* pContext = NULL);
	virtual BOOL Create(const RECT &a_Rect);

  void SetSpeed(int iSpeed);
  void SetBreedTime(int iBreedTime);
  void SetEnergyPerPrey(int iEnergy);
  void SetEnergyMin(int iEnergy);
  void SetShowFPS(BOOL);
  void SetShowStat(BOOL);
  void SetShowParam(BOOL);
  void Draw();

protected:
  int m_GLPixelIndex;

  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   
private:
  typedef class Fish
  {
  public:
    int x, y, age, energy, maxage;
    class Fish *next,*pred;

    Fish(int a_x, int a_y, int a_energy, int a_age, int a_maxage);
   ~Fish();

  private:
    static int num;
  } fish_type;

  enum SeaStates
  {
    tpPRED = 0,
    tpPREY,
    tpCOUNT,
    tpSEA = 99
  };

  volatile bool m_bStopSaver;

  CDC *m_pDC;                ///< Handle to Device context
  HGLRC m_hRC;	             ///< Handle to RC
	CPoint m_ptLast;

  // Simulation parameters
  fish_type *pRoot[tpCOUNT]; ///< First element of the list
  int m_iNum[tpCOUNT];       ///< number of individuals
  int m_iBreedTime;          ///< prey survives at most this number of rounds
  int m_iDimX;               ///< Field dimensions actually used
  int m_iDimY;               ///< Field dimensions actually used

  // Chart parameters
  static const int m_iSizeLabel = 120,
                   m_iChartX = 0,
                   m_iChartY = 0, 
                   m_iChartH = 150,
                   m_iAlpha = 210;
  static TCHAR m_szConfig[];
  int m_iWidth,
      m_iHeight,
      m_iSimHeight,
      m_iSimWidth,
      m_iPixSize,
      m_iEnergyPerPrey,
      m_iEnergyMin,
      m_iFrames;
      
  int **m_aiField;

  BOOL m_bPreview,
       m_bShowFPS,
       m_bShowStat,
       m_bShowParam;
  DWORD m_iFrameTime;

  // Program variables
  double  m_fFPS,
          m_fScaleX,
          m_fScaleY;
  GLuint	m_iFontBase;    ///< Base Display List For The Font Set
  GLuint  m_iTexture[4];  ///< Storage For 3 Textures
  GLubyte *m_iFrameBuf;
  au::StatBuffer<int> m_CircBuf[tpCOUNT]; // Circular buffer for number of individuals 
  static const int m_iCircBufSize = 400;         // store that many states in the circbuf
  static const int m_iCircBufSkip = 2;
  double m_fCircBufScale;                 // scaling information to full screen width
  unsigned int m_iTickCount;

  CWatorGL(const CWatorGL &) {}; 
  CWatorGL& operator=(const CWatorGL&) { return *this; };
  void ReadConfig();
  void Reset();
  void InitGL() const;
  void InitTextures();
  void InitFont(const string_type &sName, int iSize);

  void Print(int xpos, int ypos, const string_type &sMsg) const;

  void DrawFishes() const;
  void DrawFPS() const;
  void DrawChart() const;
  void DrawParam() const;
  void DrawIntro() const;
  void Move();
  void Tick();

  void InitScene();
  void InitField();
  void InitPopulation(int num_pred, int num_prey);

  void ReleaseField();
  void ReleaseFishes();
  void ReleaseGL();
  void KillScene();

  inline fish_type* NewFish( SeaStates kind,
                             int x,
                             int y,
                             int e,
                             int maxage );
  void SetFish(int x, int y, SeaStates state);

  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnDestroy();
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#endif // !defined(APP_H_INCLUDED)
