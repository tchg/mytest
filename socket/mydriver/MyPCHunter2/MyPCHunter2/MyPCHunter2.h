
// MyPCHunter2.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CMyPCHunter2App:
// 有关此类的实现，请参阅 MyPCHunter2.cpp
//

class CMyPCHunter2App : public CWinApp
{
public:
	CMyPCHunter2App();

// 重写
public:
	virtual BOOL InitInstance();
	HANDLE m_hDevice;
// 实现

	
	DECLARE_MESSAGE_MAP()
};

extern CMyPCHunter2App theApp;
