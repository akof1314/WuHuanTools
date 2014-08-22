
// AutomaticOperDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutomaticOper.h"
#include "AutomaticOperDlg.h"

#include "CellCtrls\CellEdit.h"
#include "CellCtrls\CellComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutomaticOperDlg 对话框




CAutomaticOperDlg::CAutomaticOperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutomaticOperDlg::IDD, pParent)
	, m_bKeyUp(FALSE)
	, m_bIsAuto(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

void CAutomaticOperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONFIG, m_ConfigListCtrl);
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_BtnAuto);
}

BEGIN_MESSAGE_MAP(CAutomaticOperDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CAutomaticOperDlg::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, &CAutomaticOperDlg::OnBnClickedButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAutomaticOperDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CAutomaticOperDlg 消息处理程序

BOOL CAutomaticOperDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitListInfo();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutomaticOperDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutomaticOperDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutomaticOperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutomaticOperDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	m_ConfigListCtrl.OnParentMove(x, y);
}

void CAutomaticOperDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	m_ConfigListCtrl.OnSizing(fwSide, pRect);
}

BOOL CAutomaticOperDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccel) 
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
		{
			return(TRUE);
		}
	}

	if(pMsg->message==WM_KEYDOWN) 
		m_bKeyUp = FALSE;
	if(pMsg->message==WM_KEYUP) 
		m_bKeyUp = TRUE;
	if((pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		if (m_bKeyUp)
		{
			m_bKeyUp = FALSE;
			return m_ConfigListCtrl.OnEnterEsc(pMsg->wParam);
		}
		else
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CAutomaticOperDlg::InitListInfo()
{
	m_ConfigListCtrl.InsertColumn(0, _T("序号"));
	m_ConfigListCtrl.SetColumnWidth(0, 80);
	m_ConfigListCtrl.InsertColumn(1, _T("操作"));
	m_ConfigListCtrl.SetColumnWidth(1, 100);
	m_ConfigListCtrl.InsertColumn(2, _T("停止"));
	m_ConfigListCtrl.SetColumnWidth(2, 100);
	m_ConfigListCtrl.InsertColumn(3, _T("休息多少秒"));
	m_ConfigListCtrl.SetColumnWidth(3, 100);
}

void CAutomaticOperDlg::OnBnClickedButtonCapture()
{
	if (m_bIsAuto)
	{
		return;
	}

	CPoint ptPos;
	GetCursorPos(&ptPos);
	CWnd *pWnd = WindowFromPoint(ptPos);
	if (!pWnd)
	{
		return;
	}

	while (pWnd->GetParent() && pWnd->GetParent()->GetParent())
	{
		pWnd = pWnd->GetParent();
	}
	pWnd->ScreenToClient(&ptPos);
	PointOperInfo info(pWnd->GetSafeHwnd(), MAKELPARAM(ptPos.x, ptPos.y), pWnd->GetDC()->GetPixel(ptPos.x, ptPos.y));
	m_vecPointOperInfo.push_back(info);

	int nCurIndex = m_ConfigListCtrl.InsertItem(m_ConfigListCtrl.GetItemCount(), _T(""));
	CString strCurIndex;
	strCurIndex.Format(_T("%d"), nCurIndex);
	m_ConfigListCtrl.SetItem(nCurIndex, 0, strCurIndex);
	m_ConfigListCtrl.SetItem(nCurIndex, 1, new CCellComboBox, 
		_T("单击\n单击\n双击"));
	m_ConfigListCtrl.SetItem(nCurIndex, 2, new CCellComboBox, 
		_T("\n像素消失"));
	m_ConfigListCtrl.SetItem(nCurIndex, 3, new CCellEdit, 
		_T("0"));
}

UINT AutoThread(LPVOID pParam) 
{ 
	CAutomaticOperDlg *pThis = (CAutomaticOperDlg*)pParam;
	if (pThis)
	{
		pThis->StartAuto();
	}
	return  0 ; 
} 

void CAutomaticOperDlg::OnBnClickedButtonAuto()
{
	if (m_bIsAuto)
	{
		m_bIsAuto = FALSE;
		m_BtnAuto.SetWindowText(_T("自动操作"));
	} 
	else
	{
		if (m_vecPointOperInfo.size() == 0)
		{
			AfxMessageBox(_T("没有任何操作"));
			return;
		}

		for (int i = 0; i < m_ConfigListCtrl.GetItemCount() && i < m_vecPointOperInfo.size(); ++i)
		{
			CString strOperType = m_ConfigListCtrl.GetItemText(i, 1);
			if (strOperType == "双击")
			{
				m_vecPointOperInfo[i].oper = Oper_DBClick;
			}
			CString strStopType = m_ConfigListCtrl.GetItemText(i, 2);
			if (strStopType != "")
			{
				m_vecPointOperInfo[i].stop = Stop_Color;
			}
			CString strWait = m_ConfigListCtrl.GetItemText(i, 3);
			m_vecPointOperInfo[i].wait = _ttoi(strWait);
		}

		m_bIsAuto = TRUE;
		m_BtnAuto.SetWindowText(_T("停止"));
		AfxBeginThread(AutoThread, this);
	}
}

void CAutomaticOperDlg::OnBnClickedButtonSave()
{
	if (m_bIsAuto)
	{
		return;
	}
	
	m_ConfigListCtrl.DeleteAllItems();
	m_vecPointOperInfo.clear();
}

void CAutomaticOperDlg::StartAuto()
{
	int nCurIndex = 0;
	while (m_bIsAuto)
	{
		CWnd *pWnd = CWnd::FromHandle(m_vecPointOperInfo[nCurIndex].hwnd);
		if (pWnd)
		{
			BOOL bContinue = FALSE;
			do 
			{
				bContinue = FALSE;
				if (m_vecPointOperInfo[nCurIndex].stop == Stop_Color)
				{
					COLORREF curColor = pWnd->GetDC()->GetPixel(LOWORD(m_vecPointOperInfo[nCurIndex].pos), HIWORD(m_vecPointOperInfo[nCurIndex].pos));
					if (curColor == m_vecPointOperInfo[nCurIndex].color)
					{
						bContinue = TRUE;
					}
					else
					{
						break;
					}
				}

				if (m_vecPointOperInfo[nCurIndex].oper == Oper_Click)
				{
					pWnd->SendMessage(WM_LBUTTONDOWN, MK_LBUTTON, m_vecPointOperInfo[nCurIndex].pos);
					pWnd->SendMessage(WM_LBUTTONUP, MK_LBUTTON, m_vecPointOperInfo[nCurIndex].pos);
				}
				else
				{
					pWnd->SendMessage(WM_LBUTTONDBLCLK, MK_LBUTTON, m_vecPointOperInfo[nCurIndex].pos);
				}

				if (bContinue)
				{
					Sleep(2000);
				}
			} while (bContinue);
		}
		Sleep(m_vecPointOperInfo[nCurIndex].wait * 1000);

		nCurIndex++;
		if (nCurIndex >= m_vecPointOperInfo.size())
		{
			nCurIndex = 0;
		}
	}
}
