
// SMITerminal.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSMITerminalApp:
// See SMITerminal.cpp for the implementation of this class
//

class CSMITerminalApp : public CWinApp
{
public:
	CSMITerminalApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSMITerminalApp theApp;
