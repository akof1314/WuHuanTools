// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IgnoreAssert.h"
#include "SettingsDlg.h"
#include "SelectPIDDlg.h"

// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, m_timeShutDown(0)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_AUTOEXE, m_chkAutoRun);
	DDX_Control(pDX, IDC_CHECK_AUTOSTART, m_chkAutoStart);
	DDX_Control(pDX, IDC_EDIT_MINSTART, m_edtTimeAutoStart);
	DDX_Control(pDX, IDC_COMBO_AUTOSTOP, m_comAutoStop);
	DDX_Control(pDX, IDC_CHECK_AUTOSHUTDOWN, m_chkAutoShutDown);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TIME, m_dtcTimeShutDown);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_TIME, m_timeShutDown);
	DDX_Control(pDX, IDC_BUTTON_HIGH, m_btnPID);
	DDX_Control(pDX, IDC_CHECK_PID, m_chkPID);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_HIGH, &CSettingsDlg::OnBnClickedButtonHigh)
END_MESSAGE_MAP()


// CSettingsDlg message handlers

void CSettingsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString strValue;
	theApp.WriteInt(STR_REG_AUTORUN, m_chkAutoRun.GetCheck());
	theApp.WriteInt(STR_REG_AUTOSTART, m_chkAutoStart.GetCheck());
	m_edtTimeAutoStart.GetWindowText(strValue);
	theApp.WriteInt(STR_REG_TIMEAUTOSTART, atoi(CT2A(strValue)));
	theApp.WriteInt(STR_REG_AUTOSTOP, m_comAutoStop.GetCurSel());
	theApp.WriteInt(STR_REG_AUTOSHUTDOWN, m_chkAutoShutDown.GetCheck());
	strValue.Format(_T("%I64d"), m_timeShutDown);
	theApp.WriteString(STR_REG_TIMESHUTDOWN, strValue);
	CSelectPIDDlg::s_bFilterPID = m_chkPID.GetCheck();

	OnOK();
}

BOOL CSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strValue;
	m_chkAutoRun.SetCheck(theApp.GetInt(STR_REG_AUTORUN));
	m_chkAutoStart.SetCheck(theApp.GetInt(STR_REG_AUTOSTART));
	strValue.Format(_T("%d"), theApp.GetInt(STR_REG_TIMEAUTOSTART, 5));
	if (strValue.Trim().IsEmpty())
	{
		strValue = _T("5");
	}
	m_edtTimeAutoStart.SetWindowText(strValue);
	m_comAutoStop.SetCurSel(theApp.GetInt(STR_REG_AUTOSTOP));
	m_chkAutoShutDown.SetCheck(theApp.GetInt(STR_REG_AUTOSHUTDOWN));
	m_timeShutDown = _atoi64(CT2A(theApp.GetString(STR_REG_TIMESHUTDOWN).GetBuffer(0)));
	m_chkPID.SetCheck(CSelectPIDDlg::s_bFilterPID);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnBnClickedButtonHigh()
{
}

LRESULT CSettingsDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_LBUTTONDOWN == pMsg->message)
	{
		if (pMsg->hwnd == m_btnPID.GetSafeHwnd())
		{
			CSelectPIDDlg dlg;
			dlg.DoModal();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
