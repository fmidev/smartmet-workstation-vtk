
// volumevisMFC.h : main header file for the volumevisMFC application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CvolumevisMFCApp:
// See volumevisMFC.cpp for the implementation of this class
//

class CvolumevisMFCApp : public CWinApp
{
public:
	CvolumevisMFCApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CvolumevisMFCApp theApp;
