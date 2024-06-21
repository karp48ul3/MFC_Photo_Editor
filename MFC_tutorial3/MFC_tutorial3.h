
// MFC_tutorial3.h : main header file for the MFC_tutorial3 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCtutorial3App:
// See MFC_tutorial3.cpp for the implementation of this class
//

class CMFCtutorial3App : public CWinAppEx
{
public:
	CMFCtutorial3App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCtutorial3App theApp;
