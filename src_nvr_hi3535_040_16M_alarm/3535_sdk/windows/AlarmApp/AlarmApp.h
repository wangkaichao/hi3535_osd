
// AlarmApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAlarmAppApp: 
// �йش����ʵ�֣������ AlarmApp.cpp
//

class CAlarmAppApp : public CWinApp
{
public:
	CAlarmAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAlarmAppApp theApp;