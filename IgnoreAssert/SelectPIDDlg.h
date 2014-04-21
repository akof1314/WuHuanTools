#pragma once
#include "afxcmn.h"


// CSelectPIDDlg dialog

class CSelectPIDDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectPIDDlg)

public:
	CSelectPIDDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectPIDDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CListCtrl m_ListPID;
	CImageList m_ImageList;

	void InitListCtrl();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	static BOOL s_bFilterPID;
	static CUIntArray s_aryFilterPID;		// 过滤进程PID列表

	static BOOL IsFilterPID(HWND hWnd);
};
