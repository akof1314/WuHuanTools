
// IgnoreAssert.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "IgnoreAssert.h"
#include "IgnoreAssertDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIgnoreAssertApp

BEGIN_MESSAGE_MAP(CIgnoreAssertApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIgnoreAssertApp 构造

CIgnoreAssertApp::CIgnoreAssertApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CIgnoreAssertApp 对象

CIgnoreAssertApp theApp;


// CIgnoreAssertApp 初始化

BOOL CIgnoreAssertApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	fixFont();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("JsNdTools"));
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));

	CIgnoreAssertDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
static const CString strOfficeFontName = _T("宋体");
static const CString strOffice2007FontName = _T("微软雅黑");

static int CALLBACK FontFamalyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR* /*lptm*/, ULONG /*ulFontType*/, LPARAM lParam)
{
	ENSURE(lplf != NULL);
	ENSURE(lParam != NULL);

	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase((LPCTSTR) lParam) == 0 ? 0 : 1;
}

void CIgnoreAssertApp::fixFont()
{
	CWindowDC dc(NULL);
	LOGFONT lf;
	CString strNewName = strOfficeFontName;
	BYTE bNewQuality = 0;
	if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamalyProcFonts, (LPARAM)(LPCTSTR) strOffice2007FontName) == 0)
	{
		strNewName = strOffice2007FontName;
		bNewQuality = 5 /*CLEARTYPE_QUALITY*/;
	}

	afxGlobalData.fontRegular.GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, strNewName);
	lf.lfQuality = bNewQuality;
	afxGlobalData.fontRegular.DeleteObject();
	afxGlobalData.fontRegular.CreateFontIndirect(&lf);

	afxGlobalData.fontSmall.GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, strNewName);
	lf.lfQuality = bNewQuality;
	afxGlobalData.fontSmall.DeleteObject();
	afxGlobalData.fontSmall.CreateFontIndirect(&lf);

	afxGlobalData.fontTooltip.GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, strNewName);
	lf.lfQuality = bNewQuality;
	afxGlobalData.fontTooltip.DeleteObject();
	afxGlobalData.fontTooltip.CreateFontIndirect(&lf);

	afxGlobalData.fontUnderline.GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, strNewName);
	lf.lfQuality = bNewQuality;
	afxGlobalData.fontUnderline.DeleteObject();
	afxGlobalData.fontUnderline.CreateFontIndirect(&lf);

	afxGlobalData.fontBold.GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, strNewName);
	lf.lfQuality = bNewQuality;
	afxGlobalData.fontBold.DeleteObject();
	afxGlobalData.fontBold.CreateFontIndirect(&lf);

}
