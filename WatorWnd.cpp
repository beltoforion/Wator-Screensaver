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
#include "WatorWnd.h"
#include "stdafx.h"
#include "resource.h"
#include <sstream>

using namespace std;


LPCTSTR CWatorGL::m_lpszClassName = NULL;
TCHAR CWatorGL::m_szConfig[] = _T("Config");

int CWatorGL::Fish::num = 0;

//-------------------------------------------------------------------------------------------
CWatorGL::Fish::Fish(int a_x, int a_y, int a_energy, int a_age, int a_maxage)
:x(a_x)
, y(a_y)
, energy(a_energy)
, age(a_age)
, maxage(a_maxage)
{
    num++;
}

//-------------------------------------------------------------------------------------------
CWatorGL::Fish::~Fish()
{
    num--;
}

//-------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
//
//-------------------------------------------------------------------------------------------

CWatorGL::CWatorGL()
:CWnd()
{
    Reset();
    // Initialization is done in the OnCreate() handler
}

//-------------------------------------------------------------------------------------------
CWatorGL::CWatorGL(int a_iSizeX, int a_iSizeY)
:CWnd()
{
    Reset();

    // Initialization is done in the OnCreate() handler
    m_bPreview = true;
    m_iSimHeight = a_iSizeY;
    m_iSimWidth = a_iSizeX;
}

//-------------------------------------------------------------------------------------------
void CWatorGL::Reset()
{
    m_bStopSaver = false;

    m_ptLast = CPoint(-1, -1);

    m_pDC = NULL;
    m_hRC = NULL;
    m_iBreedTime = 0;
    m_iDimX = 0;
    m_iDimY = 0;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iSimHeight = 0;
    m_iSimWidth = 0;
    m_iPixSize = 0;
    m_iEnergyPerPrey = 0;
    m_iEnergyMin = 0;
    m_iFrames = 0;
    m_fFPS = 0;
    m_fScaleX = 1;
    m_fScaleY = 1;
    m_iFontBase = 0;
    m_iFrameBuf = 0;
    m_aiField = NULL;
    m_bPreview = false;
    m_bShowFPS = TRUE;
    m_bShowStat = TRUE;
    m_bShowParam = TRUE;
    m_iFrameTime = GetTickCount();
    m_fCircBufScale = 0;                 // scaling information to full screen width
    m_iTickCount = 0;

    std::srand((int)time(0));
    for (unsigned i = 0; i < tpCOUNT; ++i)
    {
        pRoot[i] = 0;
        m_iNum[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------
CWatorGL::~CWatorGL()
{
    KillScene();
    ReleaseGL();

    // added 20061122 to fix this Warning: c
    // calling DestroyWindow in CWnd::~CWnd; OnDestroy or PostNcDestroy in derived class 
    // will not be called.
    // (see http://support.microsoft.com/kb/q105081/)
    DestroyWindow();
}


//---------------------------------------------------------------------------------------
void CWatorGL::InitGL() const
{
    // Pixel pack alignment of 1 is important for 
    // glDrawPixels calls. In order to by in sync
    // with m_iFrameBuf size
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    //glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//---------------------------------------------------------------------------------------
// Based on tutorial 38 by Jeff Molofee (NeHe) from 
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=38
//
void CWatorGL::InitTextures()
{
    HBITMAP hBMP; // Handle Of The Bitmap
    BITMAP  BMP;	// Bitmap Structure

    // The ID Of The Bitmap Images We Want To Load From The Resource File
    unsigned char Texture[] = { IDB_PRED,
        IDB_PREY,
        IDB_SPLASH,
        IDB_INTRO };

    const int iNumTex = sizeof(Texture);
    glGenTextures(iNumTex, m_iTexture);	    // Generate Textures

    for (int loop = 0; loop < iNumTex; loop++)  // Loop Through All The ID's (Bitmap Images)
    {
        hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
            MAKEINTRESOURCE(Texture[loop]),
            IMAGE_BITMAP,
            0,
            0,
            LR_CREATEDIBSECTION);
        if (!hBMP) continue;
        GetObject(hBMP, sizeof(BMP), &BMP);	// Get The Object
        // hBMP: Handle To Graphics Object
        // sizeof(BMP): Size Of Buffer For Object Information
        // Buffer For Object Information

        glBindTexture(GL_TEXTURE_2D, m_iTexture[loop]);	                  // Bind Our Texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Mipmap Linear Filtering
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

        // Generate Mipmapped Texture (3 Bytes, Width, Height And Data From The BMP)
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
            BMP.bmWidth,
            BMP.bmHeight,
            GL_BGR_EXT,
            GL_UNSIGNED_BYTE,
            BMP.bmBits);
        DeleteObject(hBMP);  // Delete The Bitmap Object
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

//---------------------------------------------------------------------------------------
//
// Based on code by Jeff Molofee (NeHe)  found at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=13
//
void CWatorGL::Print(int xpos, int ypos, const std::wstring &sMsg) const
{
    glRasterPos2i(xpos, ypos); //-0.45f+0.05f*float(cos(cnt1)), 0.32f*float(sin(cnt2)));

    glPushAttrib(GL_LIST_BIT);		// Pushes The Display List Bits
    glListBase(m_iFontBase - 32); // Sets The Base Character to 32
    glCallLists((GLsizei)sMsg.length(), GL_UNSIGNED_BYTE, sMsg.c_str());	// Draws The Display List Text
    glPopAttrib();							  // Pops The Display List Bits
}

//---------------------------------------------------------------------------------------
void CWatorGL::DrawFishes() const
{
    glRasterPos2i(0, 0);

    glPixelZoom((GLfloat)m_iPixSize / (GLfloat)m_fScaleX, (GLfloat)m_iPixSize / (GLfloat)m_fScaleY);
    glDrawPixels(m_iDimX, m_iDimY, GL_RGB, GL_UNSIGNED_BYTE, m_iFrameBuf);
}

//---------------------------------------------------------------------------------------
void CWatorGL::DrawFPS() const
{
    wostringstream os;
    os << _T("FPS: ") << m_fFPS;

    glColor3ub(255, 255, 255);
    Print(20, m_iSimHeight - 30, os.str().c_str());
}

//---------------------------------------------------------------------------------------
void CWatorGL::DrawParam() const
{
    wostringstream os;
    glColor3ub(255, 255, 255);

    os << _T("Breed time: ") << m_iBreedTime;
    Print(m_iSimWidth - 220, m_iSimHeight - 30, os.str().c_str());
    os.str(_T(""));

    os << _T("Energy per prey: ") << m_iEnergyPerPrey;
    Print(m_iSimWidth - 220, m_iSimHeight - 50, os.str().c_str());
    os.str(_T(""));

    os << _T("Energymin: ") << m_iEnergyMin;
    Print(m_iSimWidth - 220, m_iSimHeight - 70, os.str().c_str());
    os.str(_T(""));
}

//---------------------------------------------------------------------------------------
void CWatorGL::DrawIntro() const
{
    // The splash screen is slowly fading away controlled by the frame count
    // after 100 frames it's gone
    // m_iTickCount
    if (m_iTickCount > 300)
        return;

    int width = (m_iSimWidth - 1) / 2;
    int height = (m_iSimHeight - 1) / 2;

    const int w2 = 200;
    const int h2 = 66;

    glBindTexture(GL_TEXTURE_2D, m_iTexture[3]);
    glColor4ub(255, 255, 255, (std::max)(0, 255 - (int)m_iTickCount));
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(width - w2, height - h2);
    glTexCoord2i(1, 0); glVertex2i(width + w2, height - h2);
    glTexCoord2i(1, 1); glVertex2i(width + w2, height + h2);
    glTexCoord2i(0, 1); glVertex2i(width - w2, height + h2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

//---------------------------------------------------------------------------------------
void CWatorGL::DrawChart() const
{
    unsigned iSize = m_CircBuf[0].Size();
    assert(m_CircBuf[1].Size() == iSize);

    int width = m_iSimWidth - 1;

    // Shark panel
    glBindTexture(GL_TEXTURE_2D, m_iTexture[0]);
    glColor4ub(255, 255, 255, 180);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, m_iChartY);
    glTexCoord2i(1, 0); glVertex2i(m_iSizeLabel, m_iChartY);
    glTexCoord2i(1, 1); glVertex2i(m_iSizeLabel - 1, m_iChartY + m_iChartH);
    glTexCoord2i(0, 1); glVertex2i(0, m_iChartY + m_iChartH);
    glEnd();

    // Fish panel
    glBindTexture(GL_TEXTURE_2D, m_iTexture[1]);
    glColor4ub(255, 255, 255, 180);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(width - m_iSizeLabel, m_iChartY);
    glTexCoord2i(1, 0); glVertex2i(width, m_iChartY);
    glTexCoord2i(1, 1); glVertex2i(width, m_iChartY + m_iChartH);
    glTexCoord2i(0, 1); glVertex2i(width - m_iSizeLabel, m_iChartY + m_iChartH);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    // Chart panel
    glColor4ub(10, 10, 20, m_iAlpha);
    glBegin(GL_QUADS);
    glVertex2i(m_iSizeLabel, m_iChartY);
    glVertex2i(width - m_iSizeLabel, m_iChartY);
    glVertex2i(width - m_iSizeLabel, m_iChartY + m_iChartH);
    glVertex2i(m_iSizeLabel, m_iChartY + m_iChartH);
    glEnd();

    // Draw labels
    wostringstream os;
    os << m_iNum[tpPRED];
    glColor3ub(100, 100, 252);
    Print(30, m_iChartY + m_iSizeLabel - 110, os.str());

    os.str(_T(""));
    os << m_iNum[tpPREY];
    glColor3ub(80, 212, 80);
    Print(width - m_iSizeLabel + 30, m_iChartY + m_iSizeLabel - 110, os.str());

    int max = std::max(m_CircBuf[tpPREY].Max(), m_CircBuf[tpPRED].Max()),
        min = std::min(m_CircBuf[tpPREY].Min(), m_CircBuf[tpPRED].Min());
    double mult = (double)m_iChartH / (double)(max - min);
    glBegin(GL_LINE_STRIP);
    glColor3ub(80, 212, 80);
    for (unsigned x = 1; x < iSize; ++x)
    {
        glVertex2i((GLint)(m_iSizeLabel + x*m_fCircBufScale), (GLint)(m_iChartY + (m_CircBuf[tpPREY][x] - min)*mult));
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3ub(100, 100, 252);
    for (unsigned x = 1; x < iSize; ++x)
    {
        glVertex2i((GLint)(m_iSizeLabel + x*m_fCircBufScale), (GLint)(m_iChartY + (m_CircBuf[tpPRED][x] - min)*mult));
    }
    glEnd();

    // outlines
    //  glColor3ub(55, 125, 155);
    glColor3ub(35, 105, 135);
    glPointSize(20);
    glBegin(GL_LINE_STRIP);
    // surrounding box 
    glVertex2i(m_iSizeLabel, m_iChartY);
    glVertex2i(width - m_iSizeLabel, m_iChartY);
    glVertex2i(width - m_iSizeLabel, m_iChartY + m_iChartH);
    glVertex2i(m_iSizeLabel, m_iChartY + m_iChartH);
    glVertex2i(m_iSizeLabel, m_iChartY);

    // left panel
    glVertex2i(0, m_iChartY);
    glVertex2i(m_iSizeLabel, m_iChartY);
    glVertex2i(m_iSizeLabel - 1, m_iChartY + m_iChartH);
    glVertex2i(0, m_iChartY + m_iChartH);
    glVertex2i(0, m_iChartY);

    // right panel
    glVertex2i(width - m_iSizeLabel, m_iChartY);
    glVertex2i(width, m_iChartY);
    glVertex2i(width, m_iChartY + m_iChartH);
    glVertex2i(width - m_iSizeLabel, m_iChartY + m_iChartH);
    glVertex2i(width - m_iSizeLabel, m_iChartY);
    glEnd();
}

//---------------------------------------------------------------------------------------
void CWatorGL::Draw()
{
    CClientDC clientDC(this);
    wglMakeCurrent(clientDC.m_hDC, m_hRC);

    // Get the rendering context
    m_iFrames++;

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_iSimWidth, 0, m_iSimHeight, 0, 1);

    DrawFishes();

    // Background pattern
    glBindTexture(GL_TEXTURE_2D, m_iTexture[2]);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR); //GL_SRC_COLOR);
    glColor4ub(255, 15, 15, 80);
    GLfloat f = (GLfloat)(m_iTickCount / 2000.0);
    glBegin(GL_QUADS);
    glTexCoord2f(f, 0); glVertex2i(0, 0);
    glTexCoord2f(f, 1); glVertex2i(0, m_iSimHeight);
    glTexCoord2f(f + 1, 1); glVertex2i(m_iSimWidth, m_iSimHeight);
    glTexCoord2f(f + 1, 0); glVertex2i(m_iSimWidth, 0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_bShowStat)
        DrawChart();

    if (m_bShowFPS)
        DrawFPS();

    if (m_bShowParam)
        DrawParam();

    DrawIntro();

    wglMakeCurrent(NULL, NULL);
}

//---------------------------------------------------------------------------------------
void CWatorGL::KillScene()
{
    ReleaseFishes();
    ReleaseField();
}

//---------------------------------------------------------------------------------------
void CWatorGL::InitScene()
{
    assert(m_iWidth >= 2); // we need a valid width here
    assert(m_iSimWidth >= 2 * m_iSizeLabel);

    // Init the buffer that stores the number of individuals
    for (int i = 0; i < tpCOUNT; ++i)
    {
        m_CircBuf[i].Reset(m_iCircBufSize);
        m_fCircBufScale = (double)(m_iSimWidth - 2 * m_iSizeLabel) / (double)m_iCircBufSize;
    }

    InitField();
    int num_pred((m_iDimX * m_iDimY) / 5),
        num_prey((m_iDimX * m_iDimY) / 5);
    InitPopulation(num_pred, num_prey);
}

//---------------------------------------------------------------------------------------
//
// Based on code by Jeff Molofee (NeHe)  found at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=13
//
void CWatorGL::InitFont(const string_type &sName, int iSize)
{
    HFONT	font, oldfont;
    CClientDC clientDC(this);

    m_iFontBase = glGenLists(96);
    assert(m_iFontBase);

    font = CreateFont(iSize,                     // Height Of Font
        0,                         // Width Of Font
        0,				                  // Angle Of Escapement
        0,                         // Orientation Angle
        FW_BOLD,                   // Font Weight
        FALSE,                     // Italic
        FALSE,                     // Underline
        FALSE,                     // Strikeout
        ANSI_CHARSET,              // Character Set Identifier
        OUT_TT_PRECIS,             // Output Precision
        CLIP_DEFAULT_PRECIS,       // Clipping Precision
        ANTIALIASED_QUALITY,       // Output Quality
        FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
        sName.c_str());	          // Font Name
    assert(font);

    oldfont = (HFONT)SelectObject(clientDC.m_hDC, font);    // Selects The Font We Want
    wglUseFontBitmaps(clientDC.m_hDC, 32, 96, m_iFontBase); // Builds 96 Characters Starting At Character 32 
    SelectObject(clientDC.m_hDC, oldfont);						      // Selects The Font We Want
    DeleteObject(font);									                    // Delete The Font
}


//---------------------------------------------------------------------------------------
void CWatorGL::InitField()
{
    ReleaseFishes();
    ReleaseField();

    m_iDimX = (int)(((double)m_iSimWidth / (double)m_iPixSize) * m_fScaleX);
    m_iDimY = (int)(((double)m_iSimHeight / (double)m_iPixSize) * m_fScaleY);

    m_iFrameBuf = new GLubyte[3 * m_iDimX * m_iDimY];
    ZeroMemory(m_iFrameBuf, m_iDimX * m_iDimY * 3);
    m_aiField = new int*[m_iDimX]; // columns

    for (int c = 0; c < m_iDimX; ++c)
    {
        m_aiField[c] = new int[m_iDimY]; // rows
        for (int r = 0; r < m_iDimY; ++r)
            m_aiField[c][r] = tpSEA;
    }
}


//---------------------------------------------------------------------------------------
void CWatorGL::InitPopulation(int a_iNumPred, int a_iNumPrey)
{
    if (a_iNumPred == 0 || a_iNumPrey == 0)
        return;

    fish_type *pFish1(NULL),
        *pFish2(NULL);
    int x, y;

    // 
    // Prey
    // 
    pRoot[tpPREY] = pFish2 = NewFish(tpPREY, 0, 0, 0, au::rnum(1, m_iBreedTime));
    pRoot[tpPREY]->pred = NULL;

    for (int a = 0; a < a_iNumPrey; a++)
    {
        do
        {
            x = au::rnum(0, m_iDimX - 1);
            y = au::rnum(0, m_iDimY - 1);
        } while (m_aiField[x][y] != tpSEA);

        pFish1 = NewFish(tpPREY, x, y, 0, au::rnum(1, m_iBreedTime));
        pFish1->age = au::rnum(200);
        pFish1->maxage = pFish1->age + au::rnum(1, 300);
        ASSERT(pFish1->maxage);

        pFish1->pred = pFish2;

        pFish2->next = pFish1;
        pFish2 = pFish1;
    }
    ASSERT(pFish1);
    pFish1->next = NULL;

    //
    // Predetors
    // 
    pRoot[tpPRED] = pFish2 = NewFish(tpPRED, 0, 0, au::rnum(100, 400), 0);
    pRoot[tpPRED]->pred = NULL;
    for (int a = 0; a < a_iNumPred; a++)
    {
        do
        {
            x = au::rnum(0, m_iDimX - 1);
            y = au::rnum(0, m_iDimY - 1);
        } while (m_aiField[x][y] != tpSEA);

        pFish1 = NewFish(tpPRED, x, y, au::rnum(400), 0);
        pFish1->age = au::rnum(200);
        pFish1->maxage = pFish1->age + au::rnum(200);
        pFish1->pred = pFish2;

        pFish2->next = pFish1;
        pFish2 = pFish1;
    }
    pFish1->next = NULL;
}


//---------------------------------------------------------------------------------------
void CWatorGL::ReleaseField()
{
    delete[] m_iFrameBuf;
    m_iFrameBuf = 0;

    if (m_aiField)
    {
        for (int i = 0; i < m_iDimX; ++i)
            delete[] m_aiField[i];

        delete[] m_aiField;
        m_aiField = 0;
    }
}

//---------------------------------------------------------------------------------------
void CWatorGL::ReleaseFishes()
{
    for (int i = 0; i < tpCOUNT; ++i)
    {
        fish_type *pItem = pRoot[i], *pBuf = 0;
        while (pItem)
        {
            pBuf = pItem->next;
            delete pItem;
            pItem = pBuf;
        } // while has child individuum
    } // for individual types
}

//---------------------------------------------------------------------------------------
void CWatorGL::ReadConfig()
{
    m_iBreedTime = AfxGetApp()->GetProfileInt(m_szConfig, _T("BreedTime"), 10);
    m_iPixSize = AfxGetApp()->GetProfileInt(m_szConfig, _T("PixSize"), 5);
    m_iEnergyPerPrey = AfxGetApp()->GetProfileInt(m_szConfig, _T("EnergyPerPrey"), 40);
    m_iEnergyMin = AfxGetApp()->GetProfileInt(m_szConfig, _T("EnergyMin"), 120);
    //  m_iTimeout       = AfxGetApp()->GetProfileInt(m_szConfig, _T("Timeout"), 20);
    m_bShowStat = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowStat"), TRUE);
    m_bShowFPS = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowFPS"), FALSE);
    m_bShowParam = AfxGetApp()->GetProfileInt(m_szConfig, _T("ShowParam"), FALSE);
}

//---------------------------------------------------------------------------------------
inline CWatorGL::fish_type* CWatorGL::NewFish(SeaStates kind, int x, int y, int e, int maxage)
{
    fish_type *buf = new fish_type(x, y, e, 0, maxage);
    m_iNum[kind]++;
    SetFish(x, y, kind);

    return buf;
}

//---------------------------------------------------------------------------------------
void CWatorGL::Tick()
{
    m_iTickCount++;
    Move();

    if (m_iTickCount % m_iCircBufSkip == 0)
    {
        for (int i = 0; i < tpCOUNT; ++i)
        {
            m_CircBuf[i].Push(m_iNum[i]);
        }
    }
}

//---------------------------------------------------------------------------------------
void CWatorGL::Move()
{
    int new_xp, new_yp;
    fish_type *pFish(0), *pFishChild(0), *pFishNext(0);

    ZeroMemory(m_iFrameBuf, m_iDimX * m_iDimY * 3);

    //
    // PREY
    // 
    pFish = pRoot[tpPREY];
    while (pFish)
    {
        // Test is prey is still alive.
        // Skip this test is only one individual is left since exctincion would end the simulation
        if (m_iNum[tpPREY] > 1 && (m_aiField[pFish->x][pFish->y] == tpPRED || m_aiField[pFish->x][pFish->y] == tpSEA))
        {
            pFishNext = pFish->next;
            if (pFish == pRoot[tpPREY])
            {
                // The root element dies make the next one the root element
                pRoot[tpPREY] = pRoot[tpPREY]->next;
                pRoot[tpPREY]->pred = NULL;
            }
            else
            {
                if (pFish->pred) (pFish->pred)->next = pFish->next;
                if (pFish->next) (pFish->next)->pred = pFish->pred;
            }

            delete pFish;
            pFish = pFishNext;
            m_iNum[tpPREY]--;
            continue;
        }

        // Movement of prey
        new_xp = pFish->x + au::rnum(-1, 1);
        new_yp = pFish->y + au::rnum(-1, 1);
        if (new_xp < 0)        new_xp = m_iDimX - 1;
        if (new_xp >= m_iDimX) new_xp = 0;
        if (new_yp<0)        new_yp = m_iDimY - 1;
        if (new_yp >= m_iDimY) new_yp = 0;

        //    pFish->age++;

        // The new field is empty, so prey can move
        if (m_aiField[new_xp][new_yp] == tpSEA)
        {
            // Beute Alterungsprozess
            pFish->age++;
            if (pFish->age > pFish->maxage)
            {
                // Fish has reached the age where it gives birth to a new individuum
                // create offspring
                pFishChild = NewFish(tpPREY, pFish->x, pFish->y, 0, m_iBreedTime);
                if (pRoot[tpPREY]->next) (pRoot[tpPREY]->next)->pred = pFishChild;
                pFishChild->next = pRoot[tpPREY]->next;
                pFishChild->pred = pRoot[tpPREY];
                pRoot[tpPREY]->next = pFishChild;

                // Reset the parent
                pFish->age = 0;
            }
            else
            {
                m_aiField[pFish->x][pFish->y] = tpSEA; // prey moved clear the old position
            }

            pFish->x = new_xp;
            pFish->y = new_yp;
        }

        SetFish(pFish->x, pFish->y, tpPREY);
        pFish = pFish->next;
    }

    //
    // Predator
    //
    pFish = pRoot[tpPRED];
    while (pFish)
    {
        pFishNext = pFish->next;
        new_xp = pFish->x + au::rnum(-1, 1);
        new_yp = pFish->y + au::rnum(-1, 1);
        if (new_xp < 0)        new_xp = m_iDimX - 1;
        if (new_xp >= m_iDimX) new_xp = 0;
        if (new_yp < 0)        new_yp = m_iDimY - 1;
        if (new_yp >= m_iDimY) new_yp = 0;

        // Movement
        pFish->energy--;

        if (m_aiField[new_xp][new_yp] != tpPRED)
        {
            if (m_aiField[new_xp][new_yp] == tpPREY)
            {
                m_aiField[new_xp][new_yp] = tpSEA;
                pFish->energy += m_iEnergyPerPrey;
            }

            if (pFish->energy > m_iEnergyMin)
            {
                pFishChild = NewFish(tpPRED, pFish->x, pFish->y, pFish->energy / 2, 0);
                if (pRoot[tpPRED]->next) (pRoot[tpPRED]->next)->pred = pFishChild;
                pFishChild->next = pRoot[tpPRED]->next;
                pFishChild->pred = pRoot[tpPRED];
                pRoot[tpPRED]->next = pFishChild;
                pFish->energy /= 2;
                pFish->age = 0;
            }
            else
            {
                m_aiField[pFish->x][pFish->y] = tpSEA;
            }

            pFish->x = new_xp;
            pFish->y = new_yp;
        }
        SetFish(pFish->x, pFish->y, tpPRED);

        // Predator starvation prozess
        if (pFish->energy <= 0 && m_iNum[tpPRED] > 1)
        {
            m_aiField[pFish->x][pFish->y] = tpSEA;
            if (pFish == pRoot[tpPRED])
            {
                pRoot[tpPRED] = pRoot[tpPRED]->next;
                pRoot[tpPRED]->pred = NULL;
            }
            else
            {
                if (pFish->pred) (pFish->pred)->next = pFish->next;
                if (pFish->next) (pFish->next)->pred = pFish->pred;
            }

            delete pFish;
            m_iNum[tpPRED]--;
        }

        pFish = pFishNext;
    }
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetFish(int x, int y, SeaStates state)
{
    ASSERT(x >= 0);
    ASSERT(y >= 0);
    ASSERT(x < m_iDimX);
    ASSERT(y < m_iDimY);

    m_aiField[x][y] = state;

    int idx = m_iDimX * y + x;
    GLubyte r, g, b;

    if (state == tpPREY)
        r = 80, g = 212, b = 80;
    else
        r = 100, g = 100, b = 252;

    assert(idx >= 0);
    m_iFrameBuf[3 * idx] = r;
    m_iFrameBuf[3 * idx + 1] = g;
    m_iFrameBuf[3 * idx + 2] = b;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetSpeed(int iSpeed)
{
    KillTimer(1);
    VERIFY(SetTimer(1, iSpeed, NULL));
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetBreedTime(int iBreedTime)
{
    m_iBreedTime = iBreedTime;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetEnergyPerPrey(int iEnergy)
{
    m_iEnergyPerPrey = iEnergy;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetEnergyMin(int iEnergy)
{
    m_iEnergyMin = iEnergy;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetShowFPS(BOOL bStat)
{
    m_bShowFPS = bStat;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetShowParam(BOOL bStat)
{
    m_bShowParam = bStat;
}

//---------------------------------------------------------------------------------------
void CWatorGL::SetShowStat(BOOL bStat)
{
    m_bShowStat = bStat;
}

//---------------------------------------------------------------------------------------
//
// Message handling
//
//---------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CWatorGL, CWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_MBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_KEYDOWN()
    ON_WM_SYSKEYDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
int CWatorGL::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CClientDC clientDC(this);

    // Define the pixel format
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL;

#ifdef WATOR_DOUBLE_BUFFER
    pfd.dwFlags |= PFD_DOUBLEBUFFER;
#endif

    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int iPixFmt;
    if ((iPixFmt = ::ChoosePixelFormat(clientDC.m_hDC, &pfd)) == 0)
    {
        ::MessageBox(NULL, _T("ChoosePixelFormat Failed"), NULL, MB_OK);
        return 0;
    }

    if (SetPixelFormat(clientDC.m_hDC, iPixFmt, &pfd) == 0)
    {
        ::MessageBox(NULL, _T("SetPixelFormat Failed"), NULL, MB_OK);
        return 0;
    };

    // Get the rendering context
    m_hRC = wglCreateContext(clientDC.m_hDC);
    wglMakeCurrent(clientDC.m_hDC, m_hRC);

    // Initialise OpenGL rendering context, textures and font
    InitGL();
    InitTextures();
    InitFont(_T("Verdana"), (int)(24.0 / m_fScaleX));

    // Load the configuration from Registry
    ReadConfig();
    InitScene();

    // Start Frame counter
    SetTimer(2, 1000, NULL);
    SetSpeed(10);

    return 0;
}

//---------------------------------------------------------------------------------------
BOOL CWatorGL::OnEraseBkgnd(CDC* pDC)
{
    return true;
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnDestroy()
{
    // Alle anderen Saverfenster werden jetzt auch geschlossen
    m_bStopSaver = true;

    ReleaseGL();
    CWnd::OnDestroy();
}

//---------------------------------------------------------------------------------------
void CWatorGL::ReleaseGL()
{
    // make rendering context not current
    if (wglGetCurrentContext() != NULL)
        wglMakeCurrent(NULL, NULL);

    if (m_hRC != NULL)
    {
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }
}

//---------------------------------------------------------------------------------------
BOOL CWatorGL::Create(DWORD dwExStyle,
    DWORD dwStyle,
    const RECT& rect,
    CWnd* pParentWnd,
    UINT nID,
    CCreateContext* pContext)
{
    if (m_lpszClassName == NULL && !m_bPreview)
    {
        m_lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
            ::LoadCursor(AfxGetResourceHandle(),
            MAKEINTRESOURCE(IDC_NULLCURSOR)));
    }

    // Set Size and scaling information
    m_iWidth = rect.right - rect.left;
    m_iHeight = rect.bottom - rect.top;
    if (!m_bPreview)
    {
        m_iSimWidth = m_iWidth;
        m_iSimHeight = m_iHeight;
    }
    else
    {
        assert(m_iSimWidth);
        assert(m_iSimHeight);
    }

    m_fScaleX = (double)m_iSimWidth / (double)m_iWidth;
    m_fScaleY = (double)m_iSimHeight / (double)m_iHeight;

    BOOL bStat = CreateEx(dwExStyle,
        m_lpszClassName,
        _T(""),
        dwStyle,
        rect.left,
        rect.top,
        m_iWidth,
        m_iHeight,
        pParentWnd->GetSafeHwnd(),
        NULL,
        NULL);
    ASSERT(bStat);
    ShowWindow(SW_SHOW);
    return bStat;
}

//---------------------------------------------------------------------------------------
BOOL CWatorGL::Create(const RECT &a_Rect)
{
    DWORD dwExStyle = WS_EX_TOPMOST,
        dwStyle = WS_VISIBLE | WS_POPUP;

    return CWatorGL::Create(dwExStyle,
        dwStyle,
        a_Rect,
        NULL,
        0,
        NULL);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (m_bPreview) return;

    PostMessage(WM_CLOSE);
    //  CWnd::OnRButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_bPreview) return;

    PostMessage(WM_CLOSE);
    //  CWnd::OnLButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnMButtonDown(UINT nFlags, CPoint point)
{
    if (m_bPreview) return;

    PostMessage(WM_CLOSE);
    //  CWnd::OnMButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_bPreview) return;

    PostMessage(WM_CLOSE);
    //  CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_bPreview) return;

    PostMessage(WM_CLOSE);
    //  CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bPreview) return;

    if (m_ptLast == CPoint(-1, -1))
    {
        m_ptLast = point;
    }
    else if (m_ptLast != point)
    {
        PostMessage(WM_CLOSE);
    }
    CWnd::OnMouseMove(nFlags, point);
}

//---------------------------------------------------------------------------------------
BOOL CWatorGL::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    return CWnd::PreCreateWindow(cs);
}

//---------------------------------------------------------------------------------------
void CWatorGL::OnTimer(UINT nIDEvent)
{
    if (m_bStopSaver)
    {
        PostMessage(WM_CLOSE);
        return;
    }

    switch (nIDEvent)
    {
        // Movement
    case 1:
    {
              Tick();
              Draw();

#ifdef WATOR_DOUBLE_BUFFER
              CClientDC clientDC(this);
              BOOL bStat = SwapBuffers(clientDC.m_hDC);
              ASSERT(bStat);
#endif
    }
        break;

        // Frame counter
    case 2:
        m_fFPS = 1000 * m_iFrames / ((double)GetTickCount() - (double)m_iFrameTime);
        m_iFrameTime = GetTickCount();
        m_iFrames = 0;
        break;

    default:
        CWnd::OnTimer(nIDEvent);
    }
}
