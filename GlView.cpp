// GlView.cpp : implementation file
//

#include "stdafx.h"
#include "AdaptivePIDControllersApp.h"
#include "GlView.h"

#include "Texture.h"
#include "CWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlView

CGlView::CGlView(CWnd *pclWnd)
{

    m_pclWnd = pclWnd;
    m_hWnd   = pclWnd->m_hWnd;
    m_hDC    = ::GetDC(m_hWnd);

}

CGlView::~CGlView()
{
}


BEGIN_MESSAGE_MAP(CGlView, CWnd)
    //{{AFX_MSG_MAP(CGlView)
//  ON_WM_DESTROY()
//  ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGlView message handlers


int CGlView::OnCreate() 
{
    if (!SetPixelformat(m_hDC))
    {
        ::MessageBox(::GetFocus(),"SetPixelformat Failed!","Error",MB_OK);
        return -1;
    }

    m_hglRC = wglCreateContext(m_hDC);
    int i = wglMakeCurrent(m_hDC,m_hglRC);

    InitGL();

    return 0;
}

/*
void CGlView::OnDestroy() 
{
    CWnd::OnDestroy();

    // TODO: Add your message handler code here
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(m_hglRC);  

}


BOOL CGlView::OnEraseBkgnd(CDC* pDC) 
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;    
//  return CWnd::OnEraseBkgnd(pDC);
}
*/

BOOL CGlView::SetPixelformat(HDC hdc)
{

    PIXELFORMATDESCRIPTOR *ppfd; 
    int pixelformat; 
 
    PIXELFORMATDESCRIPTOR pfd = 
    {
        sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd
        1,                     // version number
        PFD_DRAW_TO_WINDOW |   // support window
        PFD_SUPPORT_OPENGL |   // support OpenGL
        PFD_GENERIC_FORMAT |
        PFD_DOUBLEBUFFER,      // double buffered
        PFD_TYPE_RGBA,         // RGBA type
        32,                    // 24-bit color depth
        0, 0, 0, 0, 0, 0,      // color bits ignored
        8,                     // no alpha buffer
        0,                     // shift bit ignored
        8,                     // no accumulation buffer
        0, 0, 0, 0,            // accum bits ignored
        64,                    // 32-bit z-buffer
        8,                     // no stencil buffer
        8,                     // no auxiliary buffer
        PFD_MAIN_PLANE,        // main layer
        0,                     // reserved
        0, 0, 0                // layer masks ignored
    }; 

    ppfd = &pfd;
 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
    { 
        ::MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        ::MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 

}

const float NearClipPlane = -10.0f;
const float FarClipPlane = 10.0f;


GLvoid CGlView::ReSizeGLScene(GLsizei width, GLsizei height, CWorld *world) // Resize And Initialize The GL Window
{
    if (height == 0)                                    // Prevent A Divide By Zero By
    {
        height = 1;                                     // Making Height Equal One
    }

    glViewport(0, 0, width, height);    // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);        // Select The Projection Matrix
    glLoadIdentity();                   // Reset The Projection Matrix

    CVector2 *world_center = world->GetCenter();

    glOrtho(world_center->x - (world->GetSize() / 2.0f), world_center->x + (world->GetSize() / 2.0f),
            world_center->y - (world->GetSize() / 2.0f), world_center->y + (world->GetSize() / 2.0f),
            NearClipPlane, FarClipPlane);

    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();                                   // Reset The Modelview Matrix
}


int CGlView::InitGL(GLvoid)                             // All Setup For OpenGL Goes Here
{
    glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);               // White Background
    glDisable(GL_DEPTH_TEST);                           // Disables Depth Testing

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    return TRUE;                                        // Initialization Went OK
}

int CGlView::BeginDrawGLScene(GLvoid)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer

    return TRUE;
}

int CGlView::EndDrawGLScene(GLvoid)
{
    SwapBuffers(m_hDC);

    return TRUE;
}
