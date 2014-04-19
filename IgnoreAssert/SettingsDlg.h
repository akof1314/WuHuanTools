#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


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
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CButton m_chkAutoRun;
	CButton m_chkAutoStart;
	CEdit m_edtTimeAutoStart;
	CComboBox m_comAutoStop;
	CButton m_chkAutoShutDown;
	CDateTimeCtrl m_dtcTimeShutDown;
	CTime m_timeShutDown;
};
