
// WinSockClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWinSockClientApp: 
// �йش����ʵ�֣������ WinSockClient.cpp
//

class CWinSockClientApp : public CWinApp
{
public:
	CWinSockClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWinSockClientApp theApp;