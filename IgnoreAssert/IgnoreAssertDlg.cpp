
// IgnoreAssertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IgnoreAssert.h"
#include "IgnoreAssertDlg.h"
#include "SettingsDlg.h"
#include "SelectPIDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIgnoreAssertDlg 对话框


#define UM_TRAYNOTIFY	(WM_USER + 1)
#define TIMER_IGNORE	1

CIgnoreAssertDlg::CIgnoreAssertDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIgnoreAssertDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize NOTIFYICONDATA
	memset(&m_nid, 0 , sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_INFO;

	m_bStarting = false;
	m_bAutoRun = false;
	m_bAutoStart = false;
	m_nTimeAutoStart = 0;
	m_bAutoStop = true;
	m_bAutoShutDown = false;
	m_timeTimeAutoShutDown = 0;
	m_bStartingByAutoStart = false;
	m_bTimeOutShutDown = false;
}

CIgnoreAssertDlg::~CIgnoreAssertDlg()
{
	m_nid.hIcon = NULL;
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

void CIgnoreAssertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIgnoreAssertDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_TRAYNOTIFY, OnTrayNotify)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENU_OPEN, &CIgnoreAssertDlg::OnMenuOpen)
	ON_COMMAND(ID_MENU_CLOSE, &CIgnoreAssertDlg::OnMenuClose)
	ON_UPDATE_COMMAND_UI(ID_MENU_CLOSE, &CIgnoreAssertDlg::OnUpdateMenuClose)
	ON_COMMAND(ID_MENU_SET, &CIgnoreAssertDlg::OnMenuSet)
	ON_COMMAND(ID_MENU_ABOUT, &CIgnoreAssertDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, &CIgnoreAssertDlg::OnMenuExit)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_MENU_OPEN, &CIgnoreAssertDlg::OnUpdateMenuOpen)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIgnoreAssertDlg 消息处理程序

BOOL CIgnoreAssertDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	// Set tray notification window:
	m_nid.hWnd = GetSafeHwnd();
	m_nid.uCallbackMessage = UM_TRAYNOTIFY;

	// Set tray icon and tooltip:
	m_nid.hIcon = m_hIcon;

	CString strToolTip = _T("自动忽略断言");
	_tcsncpy_s (m_nid.szTip, strToolTip, strToolTip.GetLength ());
	CString strToolInfo = _T("自动忽略一切断言，服务器端必备");
	_tcsncpy_s (m_nid.szInfo, strToolInfo, strToolInfo.GetLength ());
	CString strToolInfoTitle = _T("自动忽略断言");
	_tcsncpy_s (m_nid.szInfoTitle, strToolInfoTitle, strToolInfoTitle.GetLength ());
	m_nid.dwInfoFlags = NIIF_INFO;
	m_nid.uTimeout = 3000;

	Shell_NotifyIcon (NIM_ADD, &m_nid);

	CMFCToolBar::AddToolBarForImageCollection(IDR_TOOLBAR1, IDB_BITMAP1);

	SetTimer(TIMER_IGNORE, 1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIgnoreAssertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIgnoreAssertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CIgnoreAssertDlg::OnTrayNotify(WPARAM wp, LPARAM lp)
{
	UINT uiMsg = (UINT) lp;

	switch (uiMsg)
	{
	case WM_RBUTTONUP:
		OnTrayContextMenu();
		return 1;
	}

	return 0;
}

void CIgnoreAssertDlg::OnTrayContextMenu()
{
	CPoint point;
	::GetCursorPos(&point);

	CMenu menu;
	menu.LoadMenu(IDR_MENU1);

	CMFCPopupMenu::SetForceShadow(TRUE);

	HMENU hMenu = menu.GetSubMenu(0)->Detach();
	CMFCPopupMenu* pMenu = theApp.GetContextMenuManager()->ShowPopupMenu(hMenu, point.x, point.y, this, TRUE);

	pMenu->SetForegroundWindow();
}


void CIgnoreAssertDlg::OnMenuOpen()
{
	if (m_bStarting)
	{
		return;
	}

	m_bStarting = true;
}

void CIgnoreAssertDlg::OnUpdateMenuOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bStarting);
}

void CIgnoreAssertDlg::OnMenuClose()
{
	if (!m_bStarting)
	{
		return;
	}

	m_bStarting = false;
	m_bStartingByAutoStart = false;
}

void CIgnoreAssertDlg::OnUpdateMenuClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bStarting);
}

void CIgnoreAssertDlg::OnMenuSet()
{
	CSettingsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		LoadRegValue();
	}
}

void CIgnoreAssertDlg::OnMenuAbout()
{
	MessageBox(_T("自动忽略断言，帮您自动忽略程序弹出的断言对话框！"), _T("关于"));
}

void CIgnoreAssertDlg::OnMenuExit()
{
	PostMessage (WM_CLOSE);
}

void CIgnoreAssertDlg::OnClose()
{
	if (m_bTimeOutShutDown)
	{
		PreventSystemShutdown();
	}
	KillTimer(TIMER_IGNORE);
	CDialogEx::OnClose();
	AfxPostQuitMessage(0);
}

void CIgnoreAssertDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_IGNORE:
		{
			FindAllAssertDlg();
			CheckAutoStart();
			CheckAutoShutDown();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CIgnoreAssertDlg::FindAllAssertDlg()
{
	if (!m_bStarting)
	{
		return;
	}

	CPtrList listWnd;
	CWnd *pDlg = NULL;
	HWND hChildAfter = NULL;
	do 
	{
		pDlg = FindWindowEx(NULL, hChildAfter, _T("#32770"), _T("Microsoft Visual C++ Runtime Library"));
		if (pDlg == NULL)
		{
			pDlg = FindWindowEx(NULL, hChildAfter, _T("#32770"), _T("Microsoft Visual C++ Debug Library"));
		}

		if (pDlg)
		{
			listWnd.AddTail(pDlg);
			hChildAfter = pDlg->GetSafeHwnd();
		}
	} while (pDlg);

	POSITION pos;
	for (pos = listWnd.GetHeadPosition(); pos != NULL;)
	{
		CWnd *pParentDlg = (CWnd*)listWnd.GetNext(pos);
		if (pParentDlg && CSelectPIDDlg::IsFilterPID(pParentDlg->GetSafeHwnd()))
		{
			CWnd *pButton = FindWindowEx(pParentDlg->GetSafeHwnd(), NULL, _T("Button"), _T("忽略(&I)"));
			if (pButton == NULL)
			{
				pButton = FindWindowEx(pParentDlg->GetSafeHwnd(), NULL, _T("Button"), _T("Ignore"));
			}
			if (pButton)
			{
				pButton->PostMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
				pButton->PostMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
			}
		}
	}
}

void CIgnoreAssertDlg::LoadRegValue()
{
	bool bAutoRun = theApp.GetInt(STR_REG_AUTORUN);
	bool bAutoStart = theApp.GetInt(STR_REG_AUTOSTART);
	int nTimeAutoStart = theApp.GetInt(STR_REG_TIMEAUTOSTART);
	int bAutoStop = theApp.GetInt(STR_REG_AUTOSTOP) == 0;
	bool bAutoShutDown = theApp.GetInt(STR_REG_AUTOSHUTDOWN);
	CTime timeTimeAutoShutDown = _atoi64(CT2A(theApp.GetString(STR_REG_TIMESHUTDOWN).GetBuffer(0)));

	if (m_bAutoRun != bAutoRun)
	{
		SetAutoRun(bAutoRun);
	}
	m_bAutoRun = bAutoRun;
	m_bAutoStart = bAutoStart;
	m_nTimeAutoStart = nTimeAutoStart;
	m_bAutoStop = bAutoStop;
	m_bAutoShutDown = bAutoShutDown;
	m_timeTimeAutoShutDown = timeTimeAutoShutDown;
}

void CIgnoreAssertDlg::SetAutoRun(bool bRun)
{
	CSettingsStoreSP regSP;
	CSettingsStore& reg = regSP.Create(FALSE, FALSE);

	if (reg.Open(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")))
	{
		if (bRun)
		{
			TCHAR cFilePath[MAX_PATH] = {0};
			HMODULE hFilePath = GetModuleHandle(NULL);
			GetModuleFileName(hFilePath, cFilePath, MAX_PATH);
			CString strFilePath(cFilePath);
			strFilePath.Insert(0, '\"');
			strFilePath.AppendChar('\"');
			reg.Write(_T("IgnoreAssert"), strFilePath);
		} 
		else
		{
			reg.DeleteValue(_T("IgnoreAssert"));
		}
	}
}

void CIgnoreAssertDlg::CheckAutoStart()
{
	if (!m_bAutoStart)
	{
		return;
	}

	LASTINPUTINFO lpi;
	lpi.cbSize = sizeof(lpi);
	GetLastInputInfo(&lpi);
	int nEmptyTime = (GetTickCount() - lpi.dwTime) / 1000;

	if (nEmptyTime > (m_nTimeAutoStart * 60))
	{
		if (!m_bStarting)
		{
			m_bStarting = true;
			m_bStartingByAutoStart = true;
		}
	} 
	else
	{
		if (m_bStarting && m_bStartingByAutoStart && m_bAutoStop)
		{
			m_bStarting = false;
			m_bStartingByAutoStart = false;
		}
	}
}

void CIgnoreAssertDlg::CheckAutoShutDown()
{
	if (!m_bAutoShutDown)
	{
		return;
	}

	if (m_bTimeOutShutDown)
	{
		return;
	}

	CTime timeNow = CTime::GetCurrentTime();
	if (timeNow.GetHour() == m_timeTimeAutoShutDown.GetHour()
		&& timeNow.GetMinute() >= m_timeTimeAutoShutDown.GetMinute()
		&& timeNow.GetSecond() >= m_timeTimeAutoShutDown.GetSecond())
	{
		m_bTimeOutShutDown = MySystemShutdown(_T("自动忽略断言程序正在帮您关闭计算机，若要取消关闭，请退出此程序。"));
	}
}

BOOL CIgnoreAssertDlg::MySystemShutdown(LPTSTR lpMsg)
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;       // pointer to token structure 

	BOOL fResult;               // system shutdown flag 

	// Get the current process token handle so we can get shutdown 
	// privilege. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return FALSE; 

	// Get the LUID for shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 

	// Cannot test the return value of AdjustTokenPrivileges. 

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	// Display the shutdown dialog box and start the countdown. 

	fResult = InitiateSystemShutdown( 
		NULL,    // shut down local computer 
		lpMsg,   // message for user
		30,      // time-out period, in seconds 
		FALSE,   // ask user to close apps 
		FALSE);   // reboot after shutdown 

	if (!fResult) 
		return FALSE; 

	// Disable shutdown privilege. 

	tkp.Privileges[0].Attributes = 0; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 

	return TRUE; 
}

BOOL CIgnoreAssertDlg::PreventSystemShutdown()
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;       // pointer to token structure 

	// Get the current process token handle  so we can get shutdown 
	// privilege. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return FALSE; 

	// Get the LUID for shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	// Prevent the system from shutting down. 

	if ( !AbortSystemShutdown(NULL) ) 
		return FALSE; 

	// Disable shutdown privilege. 

	tkp.Privileges[0].Attributes = 0; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 

	return TRUE;
}
