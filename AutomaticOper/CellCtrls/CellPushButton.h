#pragma once
/*****************************************************************************

    $Revision: $

    Copyright 2013 by GARMIN Corporation

    Adapted from http://www.codeproject.com/Articles/235278/An-MFC-CListCtrl-derived-class-that-allows-other-c
    Local copy at D:\Projects\ConfigurableCtrl\VS2008ConfigurableCtrl_Alex

******************************************************************************/

#include "CellButton.h"

class CCellPushButton : public CCellButton
{
public:
    CCellPushButton
        (
        HWND aNotifyee = 0,
        UINT aNotifyMsg = 0,
        WPARAM aNotifyWParam = 0,
        LPARAM aNotifyLParam = 0
        );
    virtual ~CCellPushButton();
    virtual void DrawCtrl(CDC *pDC, const LPRECT prcCell, UINT uiItemState = 0);
    virtual int GetMinWidth();
    virtual const RECT &GetTextRect() const;
    virtual void Initialize(HWND hParentWnd, BOOL *pParentEnabled, LPCTSTR strText);
    virtual BOOL OnPressButton(UINT nFlags, CPoint point);
    virtual void OnEnter();
    virtual void AddToCtrlTypeStyle(DWORD dwFlags);
    virtual void RemoveFromCtrlTypeStyle(DWORD dwFlags);
    virtual LONG GetStyle();
    CString GetDisplayedText();

    virtual const RECT &GetModDrawAreaRect() const
    {
        return m_rcArea;
    }

protected:
    virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
    virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );

private:
    RECT m_rcArea;
    static DWORD m_dwPushButtonStyle;
    HWND mNotifyee;
    UINT mNotifyMsg;
    WPARAM mNotifyWParam;
    LPARAM mNotifyLParam;
};