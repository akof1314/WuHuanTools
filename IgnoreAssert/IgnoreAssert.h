
// IgnoreAssert.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#define STR_REG_AUTORUN _T("autorun")
#define STR_REG_AUTOSTART _T("autostart")
#define STR_REG_TIMEAUTOSTART _T("timeautostart")
#define STR_REG_AUTOSTOP _T("autostop")
#define STR_REG_AUTOSHUTDOWN _T("autoshutdown")
#define STR_REG_TIMESHUTDOWN _T("timeshutdown")

// CIgnoreAssertApp:
// 有关此类的实现，请参阅 IgnoreAssert.cpp
//

class CIgnoreAssertApp : public CWinAppEx
{
public:
	CIgnoreAssertApp();

// 重写
public:
	virtual BOOL InitInstance();
	void fixFont();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CIgnoreAssertApp theApp;