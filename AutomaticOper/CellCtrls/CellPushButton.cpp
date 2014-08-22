/*****************************************************************************

    $Revision: $

    Copyright 2013 by GARMIN Corporation

    Adapted from http://www.codeproject.com/Articles/235278/An-MFC-CListCtrl-derived-class-that-allows-other-c
    Local copy at D:\Projects\ConfigurableCtrl\VS2008ConfigurableCtrl_Alex

******************************************************************************/

#include "stdafx.h"
#include "CellPushButton.h"
#include "..\utilities.h"
#include "..\ConfigListCtrl.h"

#define IMAGE_SIZE            20
#define BOX_SEP                3

DWORD CCellPushButton::m_dwPushButtonStyle = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// optional parameters configure a SendMessage that will be sent when pressed if notifyee is not zero
CCellPushButton::CCellPushButton
    (
    HWND aNotifyee,
    UINT aNotifyMsg,
    WPARAM aNotifyWParam,
    LPARAM aNotifyLParam
    )
    : CCellButton()
    , mNotifyee( aNotifyee )
    , mNotifyMsg( aNotifyMsg )
    , mNotifyWParam( aNotifyWParam )
    , mNotifyLParam( aNotifyLParam )
{
    m_dwStyle = CLCS_ALIGN_CENTER;
    ::ZeroMemory(&m_rcArea, sizeof(RECT));
}

CCellPushButton::~CCellPushButton()
{
}

void CCellPushButton::Initialize( HWND hParentWnd, BOOL *pParentEnabled, LPCTSTR strText )
{
    CCellCtrl::Initialize( hParentWnd, pParentEnabled, strText );
}

CString CCellPushButton::GetDisplayedText()
{
    return m_strText;
}

void CCellPushButton::DrawCtrl( CDC *pDC, const LPRECT prcCell, UINT uiItemState /*= 0*/ )
{
    CBrush Brush;
    CSize szSize;
    int iSavedDC;

    iSavedDC = pDC->SaveDC();
    ::ZeroMemory( &m_rcClickRect, sizeof(RECT) ); // or use SetRectEmpty

    CopyRect( &m_rcArea, prcCell );

    if (IsDisabled())
    {
        Brush.CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
        pDC->SetTextColor( GetSysColor( COLOR_GRAYTEXT ) ); // or COLOR_INACTIVECAPTION. Seems same
    }
    else if ( uiItemState & (ODS_FOCUS | ODS_SELECTED) )
    {
        //  A button does not look good like this.
        //  However, if a different style of button for highlighted row is desired, set it here.
        /*
        Brush.CreateSolidBrush( GetSysColor( COLOR_HIGHLIGHT ) );
        pDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
        */
        // use the same color scheme as without highlight
        Brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
        pDC->SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
    }
    else
    {
        Brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
        pDC->SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
    }

    pDC->FillRect( prcCell, &Brush );
    SelectFont( pDC );
    szSize = pDC->GetTextExtent( GetDisplayedText() );

    HTHEME hTheme = OpenThemeData( m_hParentWnd, L"Button" );

    if( hTheme)
    {
        // Theme support
        SIZE sz;
        int StateId = GetThemeButtonStateId(m_bActive, PBS_NORMAL, PBS_HOT, PBS_PRESSED, PBS_DISABLED);
        HRESULT hRes = GetThemePartSize(hTheme, pDC->m_hDC, BP_PUSHBUTTON, StateId, prcCell, TS_DRAW, &sz);
        szSize.cx += sz.cx + BOX_SEP;    // Have to add the image list width.

        if( szSize.cx > prcCell->right - prcCell->left )
        {
            m_rcClickRect.left = prcCell->left;
            m_rcClickRect.right = prcCell->left + sz.cx;
        }
        else
        {
            if( m_dwStyle & CLCS_ALIGN_CENTER )
            {
                m_rcClickRect.left = prcCell->left + ( prcCell->right - prcCell->left - szSize.cx )/2;
                m_rcClickRect.right = prcCell->left + ( prcCell->right - prcCell->left - szSize.cx )/2 + sz.cx;
            }
            else if( m_dwStyle & CLCS_ALIGN_RIGHT )
            {
                if( szSize.cy )
                {
                    m_rcClickRect.right = prcCell->right - szSize.cx - BOX_SEP;
                }
                else
                {
                    m_rcClickRect.right = prcCell->right;
                }

                m_rcClickRect.left = m_rcClickRect.right - sz.cx;
            }
            else
            {
                m_rcClickRect.left = prcCell->left;
                m_rcClickRect.right = prcCell->left + sz.cx;
            }
        }

        m_rcClickRect.top = prcCell->top + (prcCell->bottom - prcCell->top - sz.cy)/2;
        m_rcClickRect.bottom = prcCell->top + (prcCell->bottom - prcCell->top - sz.cy)/2 + sz.cy;
        hRes = DrawThemeBackground (hTheme, pDC->m_hDC, BP_PUSHBUTTON, StateId, &m_rcClickRect, NULL );
        CString theText = GetDisplayedText();
#ifdef TEST_UI_STATES
        // overwrite the text to show the state
        if( m_bActive )
        {
            theText = "A";
        }
        else
        {
            theText = "a";
        }

        if( m_bIsButtonHot )
        {
            theText += "H";
        }
        else
        {
            theText += "h";
        }

        TCHAR nchar = L'0' + StateId;
        theText += nchar;
        theText += L'.';
#endif
        pDC->DrawText( theText, &m_rcArea, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
        CloseThemeData( hTheme );
    }
    else
    {
        // Theme support not available
        // See my application adaptation for code from check button purporting to handle this.
        // However, it does not work there either, for pushbutton.
		// Code below added by Christopher Camacho so when 'classic' theme is set, button is still displayed:
		CopyRect(&m_rcClickRect, prcCell);
		m_rcClickRect.right -= ::GetSystemMetrics(SM_CXEDGE);
		CBrush			Brush;		
		Brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		pDC->SelectObject(Brush);
		pDC->Rectangle(&m_rcClickRect);
		Brush.DeleteObject();

		RECT rcTxtArea;

		CopyRect(&rcTxtArea, &m_rcArea);

		if (m_bActive && !IsDisabled())
		{
			pDC->DrawEdge(&m_rcClickRect, EDGE_ETCHED, BF_RECT);
			OffsetRect(&rcTxtArea, ::GetSystemMetrics(SM_CXBORDER), ::GetSystemMetrics(SM_CYBORDER));
		}
		else
		{
			pDC->DrawEdge(&m_rcClickRect, EDGE_RAISED, BF_RECT);
		}
		// Alignment of text not done - always centered here.
		pDC->DrawText( GetDisplayedText(), &rcTxtArea, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );

		// End of code added by Christopher Camacho
    }

    Brush.DeleteObject();
    pDC->RestoreDC( iSavedDC );
}

BOOL CCellPushButton::OnPressButton( UINT /*nFlags*/, CPoint /*point*/ )
{
    if( mNotifyee )
    {
        ::PostMessage( mNotifyee, mNotifyMsg, mNotifyWParam, mNotifyLParam );
    }

    return TRUE;
}

BOOL CCellPushButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_bActive = TRUE;
    CCellButton::OnLButtonDown( nFlags, point );

    // since the cell control processed the message, do not propagate it to the list control.
    return FALSE;
}

BOOL CCellPushButton::OnLButtonUp( UINT nFlags, CPoint point )
{
    m_bActive = FALSE;
    GetWindowFromHandle()->InvalidateRect( &m_rcClickRect, FALSE );
    CCellButton::OnLButtonUp( nFlags, point );

    // since the cell control processed the message, do not propagate it to the list control.
    return FALSE;
}

void CCellPushButton::OnEnter()
{
    if( m_bActive )
    {
        GetWindowFromHandle()->InvalidateRect( &m_rcClickRect, FALSE );
    }
}

int CCellPushButton::GetMinWidth()
{
    CDC *pDC;
    int iMinWidth = 0;
    CSize szSize;
    CString strText = GetDisplayedText() + _T("i");

    pDC = GetWindowFromHandle()->GetDC();

    SelectFont(pDC);

    if( m_strText != _T("") )
    {
        szSize = pDC->GetTextExtent( strText );
        iMinWidth = szSize.cx;
    }

    iMinWidth += IMAGE_SIZE;
    return iMinWidth;
}

const RECT &CCellPushButton::GetTextRect() const
{
    return m_rcClickRect;
}

void CCellPushButton::AddToCtrlTypeStyle(DWORD dwFlags)
{
    m_dwPushButtonStyle |= dwFlags;
}

void CCellPushButton::RemoveFromCtrlTypeStyle(DWORD dwFlags)
{
    m_dwPushButtonStyle &= ~dwFlags;
}

LONG CCellPushButton::GetStyle()
{
    return m_dwPushButtonStyle | m_dwStyle;
}
