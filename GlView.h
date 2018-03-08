#if !defined(AFX_GLVIEW_H__6A408C2D_B713_4D84_8500_C4FD27AB2713__INCLUDED_)
#define AFX_GLVIEW_H__6A408C2D_B713_4D84_8500_C4FD27AB2713__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGlView window

#include <gl/gl.h> 
#include <gl/glu.h> 

class CWorld;

class CGlView : public CWnd
{
// Construction
public:
    CGlView(CWnd *pclWnd);

// Attributes
public:

    HDC     m_hDC;      // GDI Device Context
    HGLRC   m_hglRC;        // Rendering Context

    CWnd*   m_pclWnd;
    HWND    m_hWnd;
 
// Operations
public:
    BOOL    SetPixelformat(HDC hdc);
    GLvoid  ReSizeGLScene(GLsizei width, GLsizei height, CWorld *world);
    int     InitGL(GLvoid);
    int     BeginDrawGLScene(GLvoid);
    int     EndDrawGLScene(GLvoid);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGlView)
    protected:
//  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CGlView();

    // Generated message map functions
protected:

public:
    int OnCreate();

    //{{AFX_MSG(CGlView)
//  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//  afx_msg void OnDestroy();
//  afx_msg void OnSize(UINT nType, int cx, int cy);
//  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLVIEW_H__6A408C2D_B713_4D84_8500_C4FD27AB2713__INCLUDED_)
