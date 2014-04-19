
// IgnoreAssertDlg.h : 头文件
//

#pragma once


// CIgnoreAssertDlg 对话框
class CIgnoreAssertDlg : public CDialogEx
{
// 构造
public:
	CIgnoreAssertDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CIgnoreAssertDlg();

// 对话框数据
	enum { IDD = IDD_IGNOREASSERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	NOTIFYICONDATA	m_nid;			// struct for Shell_NotifyIcon args
	bool m_bStarting;
	bool m_bAutoRun;
	bool m_bAutoStart;
	int m_nTimeAutoStart;
	bool m_bAutoStop;
	bool m_bAutoShutDown;
	CTime m_timeTimeAutoShutDown;
	bool m_bStartingByAutoStart;	// 自动开始引起的启动
	bool m_bTimeOutShutDown;		// 正在关机进行中

	void OnTrayContextMenu();
	void FindAllAssertDlg();
	void LoadRegValue();
	void SetAutoRun(bool bRun);
	void CheckAutoStart();
	void CheckAutoShutDown();
	BOOL MySystemShutdown(LPTSTR lpMsg);
	BOOL PreventSystemShutdown();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wp, LPARAM lp);
	afx_msg void OnMenuOpen();
	afx_msg void OnMenuClose();
	afx_msg void OnUpdateMenuClose(CCmdUI *pCmdUI);
	afx_msg void OnMenuSet();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	afx_msg void OnClose();
	afx_msg void OnUpdateMenuOpen(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};
