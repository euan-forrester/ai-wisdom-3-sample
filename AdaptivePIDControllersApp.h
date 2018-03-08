// AdaptivePIDControllersApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CAdaptivePIDControllersApp:
// See Intelligent Steering Using Adaptive PID Controllers.cpp for the implementation of this class
//

class CAdaptivePIDControllersApp : public CWinApp
{
public:
    CAdaptivePIDControllersApp();

// Overrides
    public:
    virtual BOOL InitInstance();

// Implementation

    DECLARE_MESSAGE_MAP()
};

extern CAdaptivePIDControllersApp theApp;