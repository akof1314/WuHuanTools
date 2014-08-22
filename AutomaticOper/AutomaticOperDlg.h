
// AutomaticOperDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "ConfigListCtrl.h"
#include "vector"
#include "afxwin.h"

enum OperType{
	Oper_Click,
	Oper_DBClick,
};

enum StopType{
	Stop_None,
	Stop_Color,
};

struct PointOperInfo{
	HWND hwnd;
	OperType oper;
	StopType stop;
	LPARAM pos;
	COLORREF color;
	DWORD wait;

	PointOperInfo(HWND hWnd, LPARAM point, COLORREF clr)
		: hwnd(hWnd)
		, pos(point)
		, oper(Oper_Click)
		, stop(Stop_None)
		, color(clr)
		, wait(0)
	{
	}
};

// CAutomaticOperDlg 对话框
class CAutomaticOperDlg : public CDialog
{
// 构造
public:
	CAutomaticOperDlg(CWnd* pParent = NULL);	// 标准构造函数
	void StartAuto();

// 对话框数据
	enum { IDD = IDD_AUTOMATICOPER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CConfigListCtrl m_ConfigListCtrl;
	HACCEL m_hAccel;
	CButton m_BtnAuto;
	BOOL m_bKeyUp;
	BOOL m_bIsAuto;
	std::vector<PointOperInfo> m_vecPointOperInfo;

	void InitListInfo();

	DECLARE_MESSAGE_MAP()
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonAuto();
	afx_msg void OnBnClickedButtonSave();
};
