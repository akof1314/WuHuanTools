#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxlinkctrl.h"


// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	CButton m_chkAutoRun;
	CButton m_chkAutoStart;
	CEdit m_edtTimeAutoStart;
	CComboBox m_comAutoStop;
	CButton m_chkAutoShutDown;
	CDateTimeCtrl m_dtcTimeShutDown;
	CTime m_timeShutDown;
	CMFCLinkCtrl m_btnPID;
	CButton m_chkPID;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonHigh();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
