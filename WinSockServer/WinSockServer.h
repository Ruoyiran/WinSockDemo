
// WinSockServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWinSockServerApp: 
// �йش����ʵ�֣������ WinSockServer.cpp
//

class CWinSockServerApp : public CWinApp
{
public:
	CWinSockServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWinSockServerApp theApp;