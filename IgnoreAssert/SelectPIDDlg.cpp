// SelectPIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IgnoreAssert.h"
#include "SelectPIDDlg.h"

BOOL CSelectPIDDlg::s_bFilterPID = false;
CUIntArray CSelectPIDDlg::s_aryFilterPID;
// CSelectPIDDlg dialog

IMPLEMENT_DYNAMIC(CSelectPIDDlg, CDialog)

CSelectPIDDlg::CSelectPIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPIDDlg::IDD, pParent)
{

}

CSelectPIDDlg::~CSelectPIDDlg()
{
}

void CSelectPIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALL, m_ListPID);
}


BEGIN_MESSAGE_MAP(CSelectPIDDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectPIDDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectPIDDlg message handlers

BOOL CSelectPIDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

typedef BOOL  (WINAPI * EnumProcessModulesProc )(HANDLE, HMODULE *, DWORD, LPDWORD);
typedef DWORD (WINAPI * GetModuleBaseNameProc  )(HANDLE, HMODULE, LPTSTR, DWORD);
typedef DWORD (WINAPI * GetModuleFileNameExProc)(HANDLE, HMODULE, LPTSTR, DWORD);

//
// This uses PSAPI.DLL, which is only available under NT/2000/XP I think,
// so we dynamically load this library, so that we can still run under 9x.
//
//	dwProcessId  [in]
//  szName       [out]
//  nNameSize    [in]
//  szPath       [out]
//  nPathSize    [in]	
//
BOOL GetProcessNameByPid(DWORD dwProcessId, TCHAR szName[], DWORD nNameSize, TCHAR szPath[], DWORD nPathSize)
{
	HMODULE hPSAPI;
	HANDLE hProcess;

	HMODULE hModule;
	DWORD   dwNumModules;

	EnumProcessModulesProc  fnEnumProcessModules;
	GetModuleBaseNameProc   fnGetModuleBaseName;
	GetModuleFileNameExProc fnGetModuleFileNameEx;

	// Attempt to load Process Helper library
	hPSAPI = LoadLibrary(_T("psapi.dll"));

	if(!hPSAPI) 
	{
		szName[0] = '\0';
		return FALSE;
	}

	// OK, we have access to the PSAPI functions, so open the process
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

	if(!hProcess) 
	{
		FreeLibrary(hPSAPI);
		return FALSE;
	}


	fnEnumProcessModules  = (EnumProcessModulesProc)GetProcAddress(hPSAPI, "EnumProcessModules");

#ifdef UNICODE	
	fnGetModuleBaseName   = (GetModuleBaseNameProc)  GetProcAddress(hPSAPI, "GetModuleBaseNameW");
	fnGetModuleFileNameEx = (GetModuleFileNameExProc)GetProcAddress(hPSAPI, "GetModuleFileNameExW");
#else
	fnGetModuleBaseName   = (GetModuleBaseNameProc)  GetProcAddress(hPSAPI, "GetModuleBaseNameA");
	fnGetModuleFileNameEx = (GetModuleFileNameExProc)GetProcAddress(hPSAPI, "GetModuleFileNameExA");
#endif

	if(!fnEnumProcessModules || !fnGetModuleBaseName)
	{
		CloseHandle(hProcess);
		FreeLibrary(hPSAPI);
		return FALSE;
	}

	// Find the first module
	if(fnEnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwNumModules))
	{
		// Now get the module name
		if(szName)
			fnGetModuleBaseName(hProcess, hModule, szName, nNameSize);

		// get module filename
		if(szPath)
			fnGetModuleFileNameEx(hProcess, hModule, szPath, nPathSize);
	}
	else
	{
		szName[0] = _T('\0');
		szPath[0] = _T('\0');
	}

	CloseHandle(hProcess);
	FreeLibrary(hPSAPI);

	return TRUE;
}

BOOL CALLBACK AllWindowProc(HWND hwnd, LPARAM lParam)
{
	HWND hwndList = (HWND)lParam;
	CListCtrl *pList = (CListCtrl*)CWnd::FromHandle(hwndList);

	DWORD dwProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);

	for (int i = 0; i < pList->GetItemCount(); ++i)
	{
		CString strHasPID = pList->GetItemText(i, 1);
		if (_ttol(strHasPID.GetBuffer(0)) == dwProcessId)
		{
			return TRUE;
		}
	}

	TCHAR szName[100] = _T("");
	TCHAR szPath[MAX_PATH] = _T("");
	GetProcessNameByPid(dwProcessId, szName, 100, szPath, MAX_PATH);

	SHFILEINFO shfi = {0};
	SHGetFileInfo(szPath, 0, &shfi, sizeof(shfi), SHGFI_SMALLICON | SHGFI_ICON);

	CImageList *pImageList = pList->GetImageList(LVSIL_SMALL);
	int nImage = pImageList->Add(shfi.hIcon);

	CString strPID;
	strPID.Format(_T("%u"), dwProcessId);
	int nRow = pList->InsertItem(pList->GetItemCount(), szName, nImage);
	pList->SetItemText(nRow, 1, strPID);
	pList->SetItemText(nRow, 2, szPath);

	pList->SetCheck(nRow, FALSE);
	for (int i = 0; i < CSelectPIDDlg::s_aryFilterPID.GetSize(); ++i)
	{
		if (CSelectPIDDlg::s_aryFilterPID.GetAt(i) == dwProcessId)
		{
			pList->SetCheck(nRow, TRUE);
		}
	}
	
	return TRUE;
}

void CSelectPIDDlg::InitListCtrl()
{
	m_ImageList.Create(16, 16, ILC_COLOR32 /*ILC_COLORDDB*/|ILC_MASK, 10, 1);
	m_ListPID.SetExtendedStyle(m_ListPID.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	m_ListPID.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListPID.InsertColumn(0, _T("Ãû³Æ"), LVCFMT_LEFT, 150);
	m_ListPID.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 50);
	m_ListPID.InsertColumn(2, _T("Â·¾¶"), LVCFMT_LEFT, 250);

	m_ListPID.SetRedraw(FALSE);
	EnumWindows(AllWindowProc, (LPARAM)m_ListPID.GetSafeHwnd());
	m_ListPID.SetRedraw(TRUE);
	m_ListPID.Invalidate();
}

void CSelectPIDDlg::OnBnClickedOk()
{
	CSelectPIDDlg::s_aryFilterPID.RemoveAll();
	for (int i = 0; i < m_ListPID.GetItemCount(); ++i)
	{
		if (m_ListPID.GetCheck(i))
		{
			CString strHasPID = m_ListPID.GetItemText(i, 1);
			CSelectPIDDlg::s_aryFilterPID.Add(_ttol(strHasPID.GetBuffer(0)));
		}
	}
	OnOK();
}

BOOL CSelectPIDDlg::IsFilterPID(HWND hWnd)
{
	if (s_bFilterPID)
	{
		DWORD dwProcessId;
		GetWindowThreadProcessId(hWnd, &dwProcessId);

		for (int i = 0; i < s_aryFilterPID.GetSize(); ++i)
		{
			if (s_aryFilterPID.GetAt(i) == dwProcessId)
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}
