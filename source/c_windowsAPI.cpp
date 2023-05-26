/*
 * MINIGUI - Harbour Win32 GUI library source code
 *
 * Copyright 2002-2010 Roberto Lopez <harbourminigui@gmail.com>
 * http://harbourminigui.googlepages.com/
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this software; see the file COPYING. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA (or
 * visit the web site http://www.gnu.org/).
 *
 * As a special exception, you have permission for additional uses of the text
 * contained in this release of Harbour Minigui.
 *
 * The exception is that, if you link the Harbour Minigui library with other
 * files to produce an executable, this does not by itself cause the resulting
 * executable to be covered by the GNU General Public License.
 * Your use of that executable is in no way restricted on account of linking the
 * Harbour-Minigui library code into it.
 *
 * Parts of this project are based upon:
 *
 * "Harbour GUI framework for Win32"
 * Copyright 2001 Alexander S.Kresin <alex@kresin.ru>
 * Copyright 2001 Antonio Linares <alinares@fivetech.com>
 * www - https://harbour.github.io/
 *
 * "Harbour Project"
 * Copyright 1999-2022, https://harbour.github.io/
 *
 * "WHAT32"
 * Copyright 2002 AJ Wos <andrwos@aust1.net>
 *
 * "HWGUI"
 * Copyright 2001-2021 Alexander S.Kresin <alex@kresin.ru>
 */

#define _WIN32_IE     0x0501

#include "mgdefs.hpp"
#if ( defined ( __MINGW32__ ) ) && ( _WIN32_WINNT < 0x0500 )
#define _WIN32_WINNT  0x0500
#endif

#include <commctrl.h>
#if defined(_MSC_VER)
#pragma warning ( disable:4201 )
#endif
#include <richedit.h>
#include <shellapi.h>

#include <hbapiitm.hpp>
#include <hbapierr.hpp>
#include <hbvm.hpp>

#define WM_TASKBAR  WM_USER + 1043

// extern functions
#ifdef UNICODE
LPWSTR AnsiToWide(LPCSTR);
LPSTR  WideToAnsi(LPWSTR);
#endif
HINSTANCE      GetResources(void);
extern HB_PTRUINT wapi_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
extern void    hmg_ErrorExit(LPCTSTR lpMessage, DWORD dwError, BOOL bExit);

// local  function
HRGN           BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);

// global variables
HWND   g_hWndMain = nullptr;
HACCEL g_hAccel   = nullptr;
// static variables
static HWND hDlgModeless = nullptr;

BOOL SetAcceleratorTable(HWND hWnd, HACCEL hHaccel)
{
   g_hWndMain = hWnd;
   g_hAccel   = hHaccel;

   return TRUE;
}

HB_FUNC( DOMESSAGELOOP )
{
   MSG Msg;
   int status;

   while( ( status = GetMessage(&Msg, nullptr, 0, 0) ) != 0 )
   {
      if( status == -1 )  // Exception
      {
         // handle the error and possibly exit
         if( hb_parldef(1, HB_TRUE) )
         {
            hmg_ErrorExit(TEXT("DOMESSAGELOOP"), 0, TRUE);
         }
      }
      else
      {
         hDlgModeless = GetActiveWindow();

         if( hDlgModeless == nullptr || (
                !IsDialogMessage(hDlgModeless, &Msg) &&
                !TranslateAccelerator( g_hWndMain, g_hAccel, &Msg ) ) )
         {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
         }
      }
   }
}

/*
 * DoEvents is a statement that yields execution of the current
 * thread so that the operating system can process other events.
 * This function cleans out the message loop and executes any other pending
 * business.
 */
HB_FUNC( DOEVENTS )
{
   MSG Msg;

   while( PeekMessage(( LPMSG ) &Msg, 0, 0, 0, PM_REMOVE) )
   {
      hDlgModeless = GetActiveWindow();

      if( hDlgModeless == nullptr || !IsDialogMessage(hDlgModeless, &Msg) )
      {
         TranslateMessage(&Msg);
         DispatchMessage(&Msg);
      }
   }
}

HB_FUNC( EXITPROCESS )
{
   ExitProcess(HB_ISNUM(1) ? hb_parni(1) : 0);
}

HB_FUNC( SHOWWINDOW )
{
   ShowWindow(hmg_par_HWND(1), HB_ISNUM(2) ? hb_parni(2) : SW_SHOW);
}

HB_FUNC( GETACTIVEWINDOW )
{
   HWND hwnd = GetActiveWindow();

   hmg_ret_HWND(hwnd);
}

HB_FUNC( SETACTIVEWINDOW )
{
   SetActiveWindow(hmg_par_HWND(1));
}

HB_FUNC( POSTQUITMESSAGE )
{
   PostQuitMessage(hb_parni(1));
}

HB_FUNC( DESTROYWINDOW )
{
   DestroyWindow(hmg_par_HWND(1));
}

HB_FUNC( ISWINDOWVISIBLE )
{
   hb_retl(IsWindowVisible(hmg_par_HWND(1)));
}

HB_FUNC( ISWINDOWENABLED )
{
   hb_retl(IsWindowEnabled(hmg_par_HWND(1)));
}

HB_FUNC( ENABLEWINDOW )
{
   EnableWindow(hmg_par_HWND(1), TRUE);
}

HB_FUNC( DISABLEWINDOW )
{
   EnableWindow(hmg_par_HWND(1), FALSE);
}

HB_FUNC( SETFOREGROUNDWINDOW )
{
   SetForegroundWindow(hmg_par_HWND(1));
}

HB_FUNC( BRINGWINDOWTOTOP )
{
   BringWindowToTop(hmg_par_HWND(1));
}

HB_FUNC( GETFOREGROUNDWINDOW )
{
   HWND hwnd;

   hwnd = GetForegroundWindow();
   hmg_ret_HWND(hwnd);
}

HB_FUNC( SETWINDOWTEXT )
{
#ifndef UNICODE
   LPCSTR lpString = ( LPCSTR ) hb_parc(2);
#else
   LPCWSTR lpString = AnsiToWide(( char * ) hb_parc(2));
#endif
   SetWindowText(hmg_par_HWND(1), lpString);

#ifdef UNICODE
   hb_xfree(( TCHAR * ) lpString);
#endif
}

HB_FUNC( SETWINDOWTEXTW )
{
   SetWindowTextW(hmg_par_HWND(1), ( LPCWSTR ) hb_parc(2));
}

HB_FUNC( SETWINDOWPOS )
{
   hb_retl(( BOOL ) SetWindowPos(hmg_par_HWND(1), hmg_par_HWND(2), hb_parni(3), hb_parni(4), hb_parni(5), hb_parni(6), hb_parni(7)));
}

HB_FUNC( ANIMATEWINDOW )
{
   HWND  hWnd    = hmg_par_HWND(1);
   DWORD dwTime  = hmg_par_DWORD(2);
   DWORD dwFlags = hmg_par_DWORD(3);

   hb_retl(( BOOL ) AnimateWindow(hWnd, dwTime, dwFlags));
}

HB_FUNC( FLASHWINDOWEX )
{
   FLASHWINFO FlashWinInfo;

   FlashWinInfo.cbSize    = sizeof(FLASHWINFO);
   FlashWinInfo.hwnd      = hmg_par_HWND(1);
   FlashWinInfo.dwFlags   = hmg_par_DWORD(2);
   FlashWinInfo.uCount    = hmg_par_UINT(3);
   FlashWinInfo.dwTimeout = hmg_par_DWORD(4);

   hb_retl(( BOOL ) FlashWindowEx(&FlashWinInfo));
}

HB_FUNC( SETLAYEREDWINDOWATTRIBUTES )
{
   HWND hWnd = hmg_par_HWND(1);

   if( IsWindow(hWnd) )
   {
      HMODULE hDll = GetModuleHandle(TEXT("user32.dll"));

      hb_retl(false);

      if( hDll != nullptr )
      {
         typedef BOOL ( __stdcall * SetLayeredWindowAttributes_ptr )( HWND, COLORREF, BYTE, DWORD );

         SetLayeredWindowAttributes_ptr fn_SetLayeredWindowAttributes =
            ( SetLayeredWindowAttributes_ptr ) wapi_GetProcAddress(hDll, "SetLayeredWindowAttributes");

         if( fn_SetLayeredWindowAttributes != nullptr )
         {
            COLORREF crKey   = hmg_par_COLORREF(2);
            BYTE     bAlpha  = hmg_par_BYTE(3);
            DWORD    dwFlags = hmg_par_DWORD(4);

            if( !( GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED ) )
            {
               SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr( hWnd, GWL_EXSTYLE ) | WS_EX_LAYERED);
            }

            hb_retl(fn_SetLayeredWindowAttributes(hWnd, crKey, bAlpha, dwFlags) ? HB_TRUE : HB_FALSE);
         }
      }
   }
   else
   {
      hb_errRT_BASE_SubstR(EG_ARG, 3012, "MiniGUI Error", HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS);
   }
}

static BOOL CenterIntoParent(HWND hwnd)
{
   HWND hwndParent;
   RECT rect, rectP;
   int  width, height;
   int  screenwidth, screenheight;
   int  x, y;

   // make the window relative to its parent
   hwndParent = GetParent(hwnd);

   GetWindowRect(hwnd, &rect);
   GetWindowRect(hwndParent, &rectP);

   width  = rect.right - rect.left;
   height = rect.bottom - rect.top;

   x = ( ( rectP.right - rectP.left ) - width ) / 2 + rectP.left;
   y = ( ( rectP.bottom - rectP.top ) - height ) / 2 + rectP.top;

   screenwidth  = GetSystemMetrics(SM_CXSCREEN);
   screenheight = GetSystemMetrics(SM_CYSCREEN);

   // make sure that the child window never moves outside of the screen
   if( x < 0 )
   {
      x = 0;
   }
   if( y < 0 )
   {
      y = 0;
   }
   if( x + width > screenwidth )
   {
      x = screenwidth - width;
   }
   if( y + height > screenheight )
   {
      y = screenheight - height;
   }

   MoveWindow(hwnd, x, y, width, height, FALSE);

   return TRUE;
}

HB_FUNC( C_CENTER )
{
   HWND hwnd;
   RECT rect;
   int  w, h, x, y;

   hwnd = hmg_par_HWND(1);

   if( hb_parl(2) )
   {
      CenterIntoParent(hwnd);
   }
   else
   {
      GetWindowRect(hwnd, &rect);
      w = rect.right - rect.left;
      h = rect.bottom - rect.top;
      x = GetSystemMetrics(SM_CXSCREEN);
      SystemParametersInfo(SPI_GETWORKAREA, 1, &rect, 0);
      y = rect.bottom - rect.top;

      SetWindowPos(hwnd, HWND_TOP, ( x - w ) / 2, ( y - h ) / 2, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
   }
}

HB_FUNC( GETWINDOWTEXT )
{
#ifdef UNICODE
   LPSTR pStr;
#endif
   HWND   hWnd   = hmg_par_HWND(1);
   int    iLen   = GetWindowTextLength(hWnd);
   LPTSTR szText = ( TCHAR * ) hb_xgrab((iLen + 1) * sizeof(TCHAR));

#ifndef UNICODE
   iLen = GetWindowText(hWnd, szText, iLen + 1);

   hb_retclen(szText, iLen);
#else
   GetWindowText(hWnd, szText, iLen + 1);

   pStr = WideToAnsi(szText);
   hb_retc( pStr );
   hb_xfree(pStr);
#endif
   hb_xfree(szText);
}

HB_FUNC( SENDMESSAGE )
{
   HWND hwnd = hmg_par_HWND(1);

   if( IsWindow(hwnd) )
   {
      HB_RETNL( ( LONG_PTR ) SendMessage(hwnd, hmg_par_UINT(2), hb_parnl(3), hmg_par_LPARAM(4)) );
   }
   else
   {
      hb_errRT_BASE_SubstR(EG_ARG, 5001, "MiniGUI Error", HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS);
   }
}

HB_FUNC( SENDMESSAGESTRING )
{
   HB_RETNL( ( LONG_PTR ) SendMessage(hmg_par_HWND(1), hmg_par_UINT(2), hb_parnl(3), reinterpret_cast<LPARAM>(hb_parc(4))) );
}

HB_FUNC( GETNOTIFYCODE )
{
   LPARAM  lParam = ( LPARAM ) HB_PARNL(1);
   NMHDR * nmhdr  = ( NMHDR * ) lParam;

   hb_retni( nmhdr->code );
}

HB_FUNC( GETNOTIFYLINK )
{
   LPARAM   lParam  = ( LPARAM ) HB_PARNL(1);
   ENLINK * pENLink = ( ENLINK * ) lParam;

   hb_retnl( pENLink->msg );
   HB_STORNL( ( LONG_PTR ) pENLink->wParam, 2 );
   HB_STORNL( ( LONG_PTR ) pENLink->lParam, 3 );
   hb_stornl( pENLink->chrg.cpMin, 4 );
   hb_stornl( pENLink->chrg.cpMax, 5 );
}

//JP 107a
HB_FUNC( GETNOTIFYID )
{
   LPARAM  lParam = ( LPARAM ) HB_PARNL(1);
   NMHDR * nmhdr  = ( NMHDR * ) lParam;

   HB_RETNL( ( LONG_PTR ) nmhdr->idFrom ); // TODO: hmg_ret_HANDLE ?
}

HB_FUNC( GETHWNDFROM )
{
   LPARAM  lParam = ( LPARAM ) HB_PARNL(1);
   NMHDR * nmhdr  = ( NMHDR * ) lParam;

   hmg_ret_HWND(nmhdr->hwndFrom);
}

HB_FUNC( GETDRAWITEMHANDLE )
{
   hmg_ret_HWND(( ( DRAWITEMSTRUCT FAR * ) HB_PARNL(1) )->hwndItem);
}

HB_FUNC( GETFOCUS )
{
   hmg_ret_HWND(GetFocus());
}

HB_FUNC( GETGRIDCOLUMN )
{
   hb_retnl( ( LPARAM ) ( ( ( NM_LISTVIEW * ) HB_PARNL(1) )->iSubItem ) );
}

HB_FUNC( GETGRIDVKEY )
{
   hb_retnl( ( LPARAM ) ( ( ( LV_KEYDOWN * ) HB_PARNL(1) )->wVKey ) );
}

HB_FUNC( MOVEWINDOW )
{
   hb_retl(MoveWindow(hmg_par_HWND(1), hb_parni(2), hb_parni(3), hb_parni(4), hb_parni(5), ( HB_ISNIL(6) ? TRUE : hb_parl(6) )));
}

HB_FUNC( GETSYSTEMMETRICS )
{
   hb_retni( GetSystemMetrics(hb_parni(1)) );
}

HB_FUNC( GETWINDOWRECT )
{
   RECT rect;

   GetWindowRect(hmg_par_HWND(1), &rect);

   if( HB_ISNUM(2) )
   {
      switch( hb_parni(2) )
      {
         case 1: hb_retni( rect.top ); break;
         case 2: hb_retni( rect.left ); break;
         case 3: hb_retni( rect.right - rect.left ); break;
         case 4: hb_retni( rect.bottom - rect.top );
      }
   }
   else if( HB_ISARRAY(2) )
   {
      HB_STORVNL( rect.left, 2, 1 );
      HB_STORVNL( rect.top, 2, 2 );
      HB_STORVNL( rect.right, 2, 3 );
      HB_STORVNL( rect.bottom, 2, 4 );
   }
}

HB_FUNC( GETCLIENTRECT )
{
   RECT rect;

   hb_retl(GetClientRect(hmg_par_HWND(1), &rect));
   HB_STORVNL( rect.left, 2, 1 );
   HB_STORVNL( rect.top, 2, 2 );
   HB_STORVNL( rect.right, 2, 3 );
   HB_STORVNL( rect.bottom, 2, 4 );
}

HB_FUNC( GETDESKTOPAREA )
{
   RECT rect;

   SystemParametersInfo(SPI_GETWORKAREA, 1, &rect, 0);

   hb_reta(4);
   HB_STORNI( rect.left, -1, 1 );
   HB_STORNI( rect.top, -1, 2 );
   HB_STORNI( rect.right, -1, 3 );
   HB_STORNI( rect.bottom, -1, 4 );
}

HB_FUNC( GETTASKBARHEIGHT )
{
   RECT rect;

   GetWindowRect(FindWindow(TEXT("Shell_TrayWnd"), nullptr), &rect);
   hb_retni( rect.bottom - rect.top );
}

static BOOL ShowNotifyIcon(HWND hWnd, BOOL bAdd, HICON hIcon, TCHAR * szText)
{
   NOTIFYICONDATA nid;

   ZeroMemory(&nid, sizeof(nid));

   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.hIcon  = hIcon;
   nid.hWnd   = hWnd;
   nid.uID    = 0;
   nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
   nid.uCallbackMessage = WM_TASKBAR;
   lstrcpy(nid.szTip, szText);

   return Shell_NotifyIcon(bAdd ? NIM_ADD : NIM_DELETE, &nid);
}

HB_FUNC( SHOWNOTIFYICON )
{
#ifndef UNICODE
   char * szText = ( char * ) hb_parc(4);
#else
   TCHAR * szText = ( TCHAR * ) AnsiToWide(( char * ) hb_parc(4));
#endif
   hb_retl(( BOOL ) ShowNotifyIcon(hmg_par_HWND(1), hmg_par_BOOL(2), hmg_par_HICON(3), ( TCHAR * ) szText));

#ifdef UNICODE
   hb_xfree(szText);
#endif
}

HB_FUNC( GETCURSORPOS )
{
   POINT pt;

   GetCursorPos(&pt);
   if( hb_pcount() == 1 )
   {
      ScreenToClient(hmg_par_HWND(1), &pt);
   }

   hb_reta(2);
   if( hb_pcount() == 0 )
   {
      HB_STORNI( pt.y, -1, 1 );
      HB_STORNI( pt.x, -1, 2 );
   }
   else
   {
      HB_STORNI( pt.x, -1, 1 );
      HB_STORNI( pt.y, -1, 2 );
   }
}

HB_FUNC( SCREENTOCLIENT )
{
   LONG  x = hmg_par_LONG(2);
   LONG  y = hmg_par_LONG(3);
   POINT pt;

   pt.x = x;
   pt.y = y;

   ScreenToClient(hmg_par_HWND(1), &pt);

   hb_reta(2);
   HB_STORNI( pt.x, -1, 1 );
   HB_STORNI( pt.y, -1, 2 );
}

HB_FUNC( CLIENTTOSCREEN )
{
   LONG  x = hmg_par_LONG(2);
   LONG  y = hmg_par_LONG(3);
   POINT pt;

   pt.x = x;
   pt.y = y;

   hb_retl(ClientToScreen(hmg_par_HWND(1), &pt));

   if( HB_ISBYREF(2) )
   {
      hb_storni( pt.x, 2 );
   }
   if( HB_ISBYREF(3) )
   {
      hb_storni( pt.y, 3 );
   }
}

HB_FUNC( LOADTRAYICON )
{
   HICON     hIcon;
   HINSTANCE hInstance = hmg_par_HINSTANCE(1);                                       // handle to application instance

#ifndef UNICODE
   LPCTSTR lpIconName = HB_ISCHAR(2) ? hb_parc(2) : MAKEINTRESOURCE(hb_parni(2));   // name string or resource identifier
#else
   LPCWSTR lpIconName = HB_ISCHAR(2) ? AnsiToWide(( char * ) hb_parc(2)) : ( LPCWSTR ) MAKEINTRESOURCE(hb_parni(2));
#endif
   int cxDesired = HB_ISNUM(3) ? hb_parni(3) : GetSystemMetrics(SM_CXSMICON);
   int cyDesired = HB_ISNUM(4) ? hb_parni(4) : GetSystemMetrics(SM_CYSMICON);

   hIcon = static_cast<HICON>(LoadImage(hInstance, lpIconName, IMAGE_ICON, cxDesired, cyDesired, LR_DEFAULTCOLOR));

   if( hIcon == nullptr )
   {
      hIcon = static_cast<HICON>(LoadImage(hInstance, lpIconName, IMAGE_ICON, cxDesired, cyDesired, LR_LOADFROMFILE | LR_DEFAULTCOLOR));
   }

   RegisterResource(hIcon, "ICON");
   hmg_ret_HICON(hIcon);

#ifdef UNICODE
   if( HB_ISCHAR(2) )
   {
      hb_xfree(( TCHAR * ) lpIconName);
   }
#endif
}

static BOOL ChangeNotifyIcon(HWND hWnd, HICON hIcon, TCHAR * szText)
{
   NOTIFYICONDATA nid;

   ZeroMemory(&nid, sizeof(nid));

   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.hIcon  = hIcon;
   nid.hWnd   = hWnd;
   nid.uID    = 0;
   nid.uFlags = NIF_ICON | NIF_TIP;
   lstrcpy(nid.szTip, szText);

   return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

HB_FUNC( CHANGENOTIFYICON )
{
#ifndef UNICODE
   char * szText = ( char * ) hb_parc(3);
#else
   TCHAR * szText = ( TCHAR * ) AnsiToWide(( char * ) hb_parc(3));
#endif
   hb_retl(( BOOL ) ChangeNotifyIcon(hmg_par_HWND(1), hmg_par_HICON(2), ( TCHAR * ) szText));

#ifdef UNICODE
   hb_xfree(szText);
#endif
}

HB_FUNC( GETITEMPOS )
{
   HB_RETNL( ( LONG_PTR ) ( ( ( NMMOUSE FAR * ) HB_PARNL(1) )->dwItemSpec ) ); // TODO: hmg_ret_HANDLE ?
}

HB_FUNC( SETSCROLLRANGE )
{
   hb_retl(SetScrollRange(hmg_par_HWND(1), hb_parni(2), hb_parni(3), hb_parni(4), hb_parl(5)));
}

HB_FUNC( GETSCROLLPOS )
{
   hb_retni( GetScrollPos(hmg_par_HWND(1), hb_parni(2)) );
}

HB_FUNC( GETWINDOWSTATE )
{
   WINDOWPLACEMENT wp;

   wp.length = sizeof(WINDOWPLACEMENT);

   GetWindowPlacement(hmg_par_HWND(1), &wp);

   hb_retni( wp.showCmd );
}

HB_FUNC( GETPARENT )
{
   hmg_ret_HWND(GetParent(hmg_par_HWND(1)));
}

HB_FUNC( GETDESKTOPWINDOW )
{
   hmg_ret_HWND(GetDesktopWindow());
}

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM pArray)
{
   PHB_ITEM pHWnd = hb_itemPutNInt(nullptr, ( LONG_PTR ) hWnd);

   hb_arrayAddForward(( PHB_ITEM ) pArray, pHWnd);
   hb_itemRelease(pHWnd);

   return TRUE;
}

HB_FUNC( ENUMWINDOWS )
{
   PHB_ITEM pArray = hb_itemArrayNew(0);

   EnumWindows(( WNDENUMPROC ) EnumWindowsProc, reinterpret_cast<LPARAM>(pArray));

   hb_itemReturnRelease(pArray);
}

static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
   PHB_ITEM pCodeBlock = ( PHB_ITEM ) lParam;
   PHB_ITEM pHWnd      = hb_itemPutNInt(nullptr, ( LONG_PTR ) hWnd);

   if( pCodeBlock )
   {
      hb_evalBlock1(pCodeBlock, pHWnd);
   }

   hb_itemRelease(pHWnd);

   return hmg_par_BOOL(-1);
}

HB_FUNC( C_ENUMCHILDWINDOWS )
{
   HWND     hWnd       = hmg_par_HWND(1);
   PHB_ITEM pCodeBlock = hb_param(2, Harbour::Item::BLOCK);

   if( IsWindow(hWnd) && pCodeBlock )
   {
      hb_retl(EnumChildWindows(hWnd, EnumChildProc, reinterpret_cast<LPARAM>(pCodeBlock)) ? HB_TRUE : HB_FALSE);
   }
}

HB_FUNC( REDRAWWINDOWCONTROLRECT )
{
   RECT r;

   r.top    = hb_parni(2);
   r.left   = hb_parni(3);
   r.bottom = hb_parni(4);
   r.right  = hb_parni(5);

   RedrawWindow(hmg_par_HWND(1), &r, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASENOW | RDW_UPDATENOW);
}

HB_FUNC( ADDSPLITBOXITEM )
{
   REBARBANDINFO rbBand;
   RECT          rc;
   int style = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS | RBBS_USECHEVRON;

#ifndef UNICODE
   LPSTR lpText = ( LPSTR ) hb_parc(5);
#else
   LPWSTR lpText = AnsiToWide(( char * ) hb_parc(5));
#endif

   if( hb_parl(4) )
   {
      style |= RBBS_BREAK;
   }

   GetWindowRect(hmg_par_HWND(1), &rc);

   rbBand.cbSize  = sizeof(REBARBANDINFO);
   rbBand.fMask   = RBBIM_TEXT | RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
   rbBand.fStyle  = style;
   rbBand.hbmBack = 0;

   rbBand.lpText    = lpText;
   rbBand.hwndChild = hmg_par_HWND(1);

   if( hb_parni(9) )
   {
      rbBand.fMask = rbBand.fMask | RBBIM_IDEALSIZE;
   }

   if( !hb_parl(8) )
   {
      // Not Horizontal
      rbBand.cxMinChild = hb_parni(6) ? hb_parni(6) : 0;
      rbBand.cyMinChild = hb_parni(7) ? hb_parni(7) : rc.bottom - rc.top;
      rbBand.cx         = hb_parni(3);
      if( hb_parni(9) )
      {
         rbBand.cxIdeal    = hb_parni(6) ? hb_parni(6) : 0;
         rbBand.cxMinChild = hb_parni(9);
      }
      else
      {
         rbBand.cxMinChild = hb_parni(6) ? hb_parni(6) : 0;
      }
   }
   else
   {
      // Horizontal
      if( hb_parni(6) == 0 && hb_parni(7) == 0 )
      {
         // Not ToolBar
         rbBand.cxMinChild = 0;
         rbBand.cyMinChild = rc.right - rc.left;
         rbBand.cx         = rc.bottom - rc.top;
      }
      else
      {
         // ToolBar
         rbBand.cyMinChild = hb_parni(6) ? hb_parni(6) : 0;
         rbBand.cx         = hb_parni(7) ? hb_parni(7) : rc.bottom - rc.top;
         if( hb_parni(9) )
         {
            rbBand.cxIdeal    = hb_parni(7) ? hb_parni(7) : rc.bottom - rc.top;
            rbBand.cxMinChild = hb_parni(9);
         }
         else
         {
            rbBand.cxMinChild = hb_parni(7) ? hb_parni(7) : rc.bottom - rc.top;
         }
      }
   }

   SendMessage(hmg_par_HWND(2), RB_INSERTBAND, -1, reinterpret_cast<LPARAM>(&rbBand));

#ifdef UNICODE
   hb_xfree(lpText);
#endif
}

HB_FUNC( C_SETWINDOWRGN )
{
   HRGN    hRgn = nullptr;
   HBITMAP hbmp;

   if( hb_parni(6) == 0 )
   {
      SetWindowRgn(GetActiveWindow(), nullptr, TRUE);
   }
   else
   {
      switch( hb_parni(6) )
      {
         case 1:
            hRgn = CreateRectRgn(hb_parni(2), hb_parni(3), hb_parni(4), hb_parni(5));
            break;

         case 2:
            hRgn = CreateEllipticRgn(hb_parni(2), hb_parni(3), hb_parni(4), hb_parni(5));
            break;

         case 3:
            hRgn = CreateRoundRectRgn(0, 0, hb_parni(4), hb_parni(5), hb_parni(2), hb_parni(3));
            break;

         case 4:
            hbmp = static_cast<HBITMAP>(LoadImage(GetResources(), ( TCHAR * ) hb_parc(2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
            if( hbmp == nullptr )
            {
               hbmp = static_cast<HBITMAP>(LoadImage(nullptr, ( TCHAR * ) hb_parc(2), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
            }

            hRgn = BitmapToRegion(hbmp, ( COLORREF ) RGB(HB_PARNI(3, 1), HB_PARNI(3, 2), HB_PARNI(3, 3)), 0x101010);
            DeleteObject(hbmp);
            break;

         default:
            break;
      }

      SetWindowRgn(hmg_par_HWND(1), hRgn, TRUE);

      RegisterResource(hRgn, "REGION");
      hmg_ret_HRGN(hRgn);
   }
}

HB_FUNC( C_SETPOLYWINDOWRGN )
{
   HRGN  hRgn;
   POINT lppt[512];
   int   fnPolyFillMode;
   int   cPoints = ( int ) hb_parinfa(2, 0);

   if( hb_parni(4) == 1 )
   {
      fnPolyFillMode = WINDING;
   }
   else
   {
      fnPolyFillMode = ALTERNATE;
   }

   for( int i = 0; i <= cPoints - 1; i++ )
   {
      lppt[i].x = HB_PARNI(2, i + 1);
      lppt[i].y = HB_PARNI(3, i + 1);
   }

   hRgn = CreatePolygonRgn(lppt, cPoints, fnPolyFillMode);

   SetWindowRgn(GetActiveWindow(), hRgn, TRUE);

   RegisterResource(hRgn, "REGION");
   hmg_ret_HRGN(hRgn);
}

HB_FUNC( GETHELPDATA )
{
   hmg_ret_HANDLE(( ( HELPINFO FAR * ) HB_PARNL(1) )->hItemHandle);
}

HB_FUNC( GETMSKTEXTMESSAGE )
{
   HB_RETNL( ( LONG_PTR ) ( ( ( MSGFILTER FAR * ) HB_PARNL(1) )->msg ) ); // TODO: hmg_ret_HANDLE ?
}

HB_FUNC( GETMSKTEXTWPARAM )
{
   HB_RETNL( ( LONG_PTR ) ( ( ( MSGFILTER FAR * ) HB_PARNL(1) )->wParam ) ); // TODO: hmg_ret_HANDLE ?
}

HB_FUNC( GETMSKTEXTLPARAM )
{
   HB_RETNL( ( LONG_PTR ) ( ( ( MSGFILTER FAR * ) HB_PARNL(1) )->lParam ) ); // TODO: hmg_ret_HANDLE ?
}

HB_FUNC( GETWINDOW )
{
   hmg_ret_HWND(GetWindow(hmg_par_HWND(1), hb_parni(2)));
}

HB_FUNC( GETGRIDOLDSTATE )
{
   LPARAM        lParam = ( LPARAM ) HB_PARNL(1);
   NM_LISTVIEW * NMLV   = ( NM_LISTVIEW * ) lParam;

   hb_retni( NMLV->uOldState );
}

HB_FUNC( GETGRIDNEWSTATE )
{
   LPARAM        lParam = ( LPARAM ) HB_PARNL(1);
   NM_LISTVIEW * NMLV   = ( NM_LISTVIEW * ) lParam;

   hb_retni( NMLV->uNewState );
}

HB_FUNC( GETGRIDDISPINFOINDEX )
{
   LPARAM        lParam    = ( LPARAM ) HB_PARNL(1);
   LV_DISPINFO * pDispInfo = ( LV_DISPINFO * ) lParam;

   int iItem    = pDispInfo->item.iItem;
   int iSubItem = pDispInfo->item.iSubItem;

   hb_reta(2);
   HB_STORNI( iItem + 1, -1, 1 );
   HB_STORNI( iSubItem + 1, -1, 2 );
}

HB_FUNC( SETGRIDQUERYDATA )
{
   LPARAM        lParam    = ( LPARAM ) HB_PARNL(1);
   LV_DISPINFO * pDispInfo = ( LV_DISPINFO * ) lParam;

   // Copy the text to the LV_ITEM structure
   // Maximum number of characters is in pDispInfo->Item.cchTextMax
#ifdef UNICODE
   LPWSTR lpText = AnsiToWide(( char * ) hb_parc(2));
   lstrcpyn(pDispInfo->item.pszText, lpText, pDispInfo->item.cchTextMax);
   hb_xfree(lpText);
#else
   lstrcpyn(pDispInfo->item.pszText, ( char * ) hb_parc(2), pDispInfo->item.cchTextMax);
#endif
}

HB_FUNC( SETGRIDQUERYIMAGE )
{
   LPARAM        lParam    = ( LPARAM ) HB_PARNL(1);
   LV_DISPINFO * pDispInfo = ( LV_DISPINFO * ) lParam;

   pDispInfo->item.iImage = hb_parni(2);
}

HB_FUNC( FINDWINDOWEX )
{
#ifndef UNICODE
   LPCSTR lpszClass  = ( char * ) hb_parc(3);
   LPCSTR lpszWindow = ( char * ) hb_parc(4);
#else
   LPWSTR lpszClass  = ( hb_parc(3) != nullptr ) ? hb_osStrU16Encode(hb_parc(3)) : nullptr;
   LPWSTR lpszWindow = ( hb_parc(4) != nullptr ) ? hb_osStrU16Encode(hb_parc(4)) : nullptr;
#endif
   hmg_ret_HWND(FindWindowEx(hmg_par_HWND(1), hmg_par_HWND(2), lpszClass, lpszWindow));

#ifdef UNICODE
   if( lpszClass != nullptr )
   {
      hb_xfree(lpszClass);
   }
   if( lpszWindow != nullptr )
   {
      hb_xfree(lpszWindow);
   }
#endif

}

HB_FUNC( GETDS )
{
   LPARAM lParam = ( LPARAM ) HB_PARNL(1);
   LPNMLVCUSTOMDRAW lplvcd = ( LPNMLVCUSTOMDRAW ) lParam;

   if( lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT )
   {
      hb_retni( CDRF_NOTIFYITEMDRAW );
   }
   else if( lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
   {
      if( hb_pcount() > 1 )
      {
         if( ListView_GetNextItem(hmg_par_HWND(2), -1, LVNI_ALL | LVNI_SELECTED) == hb_parni(3) )
            ListView_SetItemState(hmg_par_HWND(2), hb_parni(3), 0, LVIS_SELECTED);
      }
      hb_retni( CDRF_NOTIFYSUBITEMDRAW );
   }
   else if( lplvcd->nmcd.dwDrawStage == ( CDDS_SUBITEM | CDDS_ITEMPREPAINT ) )
   {
      hb_retni( -1 );
   }
   else
   {
      hb_retni( CDRF_DODEFAULT );
   }
}

HB_FUNC( GETRC )     // Get ListView CustomDraw Row and Column
{
   LPARAM lParam = ( LPARAM ) HB_PARNL(1);
   LPNMLVCUSTOMDRAW lplvcd = ( LPNMLVCUSTOMDRAW ) lParam;

   hb_reta(2);
   HB_STORVNL( lplvcd->nmcd.dwItemSpec + 1, -1, 1 );
   HB_STORNI( lplvcd->iSubItem + 1, -1, 2 );
}

HB_FUNC( SETBCFC )   // Set Dynamic BackColor and ForeColor
{
   LPARAM lParam = ( LPARAM ) HB_PARNL(1);
   LPNMLVCUSTOMDRAW lplvcd = ( LPNMLVCUSTOMDRAW ) lParam;

   lplvcd->clrTextBk = hb_parni(2);
   lplvcd->clrText   = hb_parni(3);

   hb_retni( CDRF_NEWFONT );
}

HB_FUNC( SETBRCCD )  // Set Default BackColor and ForeColor
{
   LPARAM lParam = ( LPARAM ) HB_PARNL(1);
   LPNMLVCUSTOMDRAW lplvcd = ( LPNMLVCUSTOMDRAW ) lParam;

   lplvcd->clrText   = RGB(0, 0, 0);
   lplvcd->clrTextBk = RGB(255, 255, 255);

   hb_retni( CDRF_NEWFONT );
}

HB_FUNC( GETTABBEDCONTROLBRUSH )
{
   RECT   rc;
   HBRUSH hBrush;
   HDC    hDC = hmg_par_HDC(1);

   SetBkMode(hDC, TRANSPARENT);
   GetWindowRect(hmg_par_HWND(2), &rc);
   MapWindowPoints(nullptr, hmg_par_HWND(3), ( LPPOINT ) (&rc), 2);
   SetBrushOrgEx(hDC, -rc.left, -rc.top, nullptr);
   hBrush = hmg_par_HBRUSH(4);

   hmg_ret_HBRUSH(hBrush);
}

HB_FUNC( GETTABBRUSH )
{
   HBRUSH  hBrush;
   RECT    rc;
   HDC     hDC;
   HDC     hDCMem;
   HBITMAP hBmp;
   HBITMAP hOldBmp;
   HWND    hWnd = hmg_par_HWND(1);

   GetWindowRect(hWnd, &rc);
   hDC    = GetDC(hWnd);
   hDCMem = CreateCompatibleDC(hDC);

   hBmp = CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);

   hOldBmp = static_cast<HBITMAP>(SelectObject(hDCMem, hBmp));

   SendMessage(hWnd, WM_PRINTCLIENT, ( WPARAM ) hDCMem, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

   hBrush = CreatePatternBrush(hBmp);

   hmg_ret_HBRUSH(hBrush);

   SelectObject(hDCMem, hOldBmp);

   DeleteObject(hBmp);
   DeleteDC(hDCMem);
   ReleaseDC(hWnd, hDC);
}

HB_FUNC( INITMINMAXINFO )  // ( hWnd ) --> aMinMaxInfo
{
   long x, y, mx, my;

   if( GetWindowLong(hmg_par_HWND(1), GWL_STYLE) & WS_SIZEBOX )
   {
      x = -GetSystemMetrics(SM_CXFRAME);
      y = -GetSystemMetrics(SM_CYFRAME);
   }
   else
   {
      x = -GetSystemMetrics(SM_CXBORDER);
      y = -GetSystemMetrics(SM_CYBORDER);
   }

   mx = GetSystemMetrics(SM_CXSCREEN) - 2 * x;
   my = GetSystemMetrics(SM_CYSCREEN) - 2 * y;

   hb_reta(8);
   HB_STORVNL( mx, -1, 1 );
   HB_STORVNL( my, -1, 2 );
   HB_STORVNL( x, -1, 3 );
   HB_STORVNL( y, -1, 4 );
   HB_STORVNL( 0, -1, 5 );
   HB_STORVNL( 0, -1, 6 );
   HB_STORVNL( mx, -1, 7 );
   HB_STORVNL( my, -1, 8 );
}

HB_FUNC( SETMINMAXINFO )   // ( pMinMaxInfo, aMinMaxInfo ) --> 0
{
   MINMAXINFO * pMinMaxInfo = ( MINMAXINFO * ) HB_PARNL(1);

   pMinMaxInfo->ptMaxSize.x      = HB_PARNI(2, 1);
   pMinMaxInfo->ptMaxSize.y      = HB_PARNI(2, 2);
   pMinMaxInfo->ptMaxPosition.x  = HB_PARNI(2, 3);
   pMinMaxInfo->ptMaxPosition.y  = HB_PARNI(2, 4);
   pMinMaxInfo->ptMinTrackSize.x = HB_PARNI(2, 5);
   pMinMaxInfo->ptMinTrackSize.y = HB_PARNI(2, 6);
   pMinMaxInfo->ptMaxTrackSize.x = HB_PARNI(2, 7);
   pMinMaxInfo->ptMaxTrackSize.y = HB_PARNI(2, 8);

   hb_retni(0);
}

HB_FUNC( LOCKWINDOWUPDATE )
{
   hb_retl(LockWindowUpdate(hmg_par_HWND(1)) ? HB_TRUE : HB_FALSE);
}

HB_FUNC( ISWINDOWHANDLE )
{
   hb_retl(IsWindow(hmg_par_HWND(1)) ? HB_TRUE : HB_FALSE);
}

HB_FUNC( ISICONIC )
{
   hb_retl(IsIconic( hmg_par_HWND(1) ));
}

HB_FUNC( ISZOOMED )
{
   hb_retl(IsZoomed(hmg_par_HWND(1)));
}

HB_FUNC( GETWINDOWBRUSH )
{
   HB_RETNL( ( LONG_PTR ) GetClassLongPtr(hmg_par_HWND(1), GCLP_HBRBACKGROUND) );
}

HB_FUNC( SETWINDOWBRUSH )
{
   HB_RETNL( ( LONG_PTR ) SetClassLongPtr(hmg_par_HWND(1), GCLP_HBRBACKGROUND, ( LONG_PTR ) HB_PARNL(2)) );
}

HB_FUNC( CREATEHATCHBRUSH )
{
   hmg_ret_HBRUSH(CreateHatchBrush(hb_parni(1), hmg_par_COLORREF(2)));
}

/* Modified by P.Ch. 16.10. */
HB_FUNC( CREATEPATTERNBRUSH )
{
   HBITMAP hImage;

#ifndef UNICODE
   LPCTSTR lpImageName = HB_ISCHAR(1) ? hb_parc(1) : ( HB_ISNUM(1) ? MAKEINTRESOURCE(hb_parni(1)) : nullptr );
#else
   LPCWSTR lpImageName = HB_ISCHAR(1) ? AnsiToWide(( char * ) hb_parc(1)) : ( HB_ISNUM(1) ? ( LPCWSTR ) MAKEINTRESOURCE(hb_parni(1)) : nullptr );
#endif

   hImage = static_cast<HBITMAP>(LoadImage(GetResources(), lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));

   if( hImage == nullptr && HB_ISCHAR(1) )
   {
      hImage = static_cast<HBITMAP>(LoadImage(nullptr, lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
   }
   if( hImage == nullptr )
   {
      hImage = HMG_LoadImage(hb_parc(1), nullptr);
   }

   hmg_ret_HBRUSH(( hImage != nullptr ) ? CreatePatternBrush(hImage) : nullptr);

#ifdef UNICODE
   if( HB_ISCHAR(1) )
   {
      hb_xfree(( TCHAR * ) lpImageName);
   }
#endif
}

/*
   BitmapToRegion: Create a region from the "non-transparent" pixels of a bitmap
   Author        : Jean-Edouard Lachand-Robert
   (http://www.geocities.com/Paris/LeftBank/1160/resume.htm), June 1998.

   hBmp :              Source bitmap
   cTransparentColor : Color base for the "transparent" pixels
                       (default is black)
   cTolerance :        Color tolerance for the "transparent" pixels.

   A pixel is assumed to be transparent if the value of each of its 3
   components (blue, green and red) is
   greater or equal to the corresponding value in cTransparentColor and is
   lower or equal to the corresponding value in cTransparentColor + cTolerance.
 */
#define ALLOC_UNIT  100

HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
   HRGN   hRgn = nullptr;
   VOID * pbits32;
   DWORD  maxRects = ALLOC_UNIT;

   if( hBmp )
   {
      // Create a memory DC inside which we will scan the bitmap content
      HDC hMemDC = CreateCompatibleDC(nullptr);
      if( hMemDC )
      {
         BITMAP bm;
         BITMAPINFOHEADER RGB32BITSBITMAPINFO;
         HBITMAP          hbm32;

         // Get bitmap size
         GetObject(hBmp, sizeof(bm), &bm);

         // Create a 32 bits depth bitmap and select it into the memory DC
         RGB32BITSBITMAPINFO.biSize          = sizeof(BITMAPINFOHEADER);
         RGB32BITSBITMAPINFO.biWidth         = bm.bmWidth;
         RGB32BITSBITMAPINFO.biHeight        = bm.bmHeight;
         RGB32BITSBITMAPINFO.biPlanes        = 1;
         RGB32BITSBITMAPINFO.biBitCount      = 32;
         RGB32BITSBITMAPINFO.biCompression   = BI_RGB;
         RGB32BITSBITMAPINFO.biSizeImage     = 0;
         RGB32BITSBITMAPINFO.biXPelsPerMeter = 0;
         RGB32BITSBITMAPINFO.biYPelsPerMeter = 0;
         RGB32BITSBITMAPINFO.biClrUsed       = 0;
         RGB32BITSBITMAPINFO.biClrImportant  = 0;

         hbm32 = CreateDIBSection(hMemDC, ( BITMAPINFO * ) &RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, nullptr, 0);
         if( hbm32 )
         {
            HBITMAP holdBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hbm32));

            // Create a DC just to copy the bitmap into the memory DC
            HDC hDC = CreateCompatibleDC(hMemDC);
            if( hDC )
            {
               // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
               BITMAP    bm32;
               HANDLE    hData;
               RGNDATA * pData;
               BYTE *    p32;
               BYTE      lr, lg, lb, hr, hg, hb;
               HRGN      h;

               GetObject(hbm32, sizeof(bm32), &bm32);
               while( bm32.bmWidthBytes % 4 )
                  bm32.bmWidthBytes++;

               // Copy the bitmap into the memory DC
               holdBmp = static_cast<HBITMAP>(SelectObject(hDC, hBmp));
               BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

               // For better performances, we will use the  ExtCreateRegion() function to create the  region.
               // This function take a RGNDATA structure on  entry.
               // We will add rectangles by amount of ALLOC_UNIT number in this structure.
               hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + ( sizeof(RECT) * maxRects ));

               pData = ( RGNDATA * ) GlobalLock(hData);
               pData->rdh.dwSize = sizeof(RGNDATAHEADER);
               pData->rdh.iType  = RDH_RECTANGLES;
               pData->rdh.nCount = pData->rdh.nRgnSize = 0;
               SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

               // Keep on hand highest and lowest values for the  "transparent" pixels
               lr = GetRValue(cTransparentColor);
               lg = GetGValue(cTransparentColor);
               lb = GetBValue(cTransparentColor);
               hr = ( BYTE ) HB_MIN(0xff, lr + GetRValue(cTolerance));
               hg = ( BYTE ) HB_MIN(0xff, lg + GetGValue(cTolerance));
               hb = ( BYTE ) HB_MIN(0xff, lb + GetBValue(cTolerance));

               // Scan each bitmap row from bottom to top (the bitmap is  inverted vertically)
               p32 = ( BYTE * ) bm32.bmBits + ( bm32.bmHeight - 1 ) * bm32.bmWidthBytes;
               for( INT y = 0; y < bm.bmHeight; y++ )     // Scan each bitmap pixel from left to right
               {
                  for( INT x = 0; x < bm.bmWidth; x++ )   // Search for a continuous range of "non transparent pixels"
                  {
                     int    x0 = x;
                     LONG * p  = ( LONG * ) p32 + x;
                     while( x < bm.bmWidth )
                     {
                        BYTE b = GetRValue(*p);
                        if( b >= lr && b <= hr )
                        {
                           b = GetGValue(*p);
                           if( b >= lg && b <= hg )
                           {
                              b = GetBValue(*p);
                              if( b >= lb && b <= hb )
                              {
                                 break;   // This pixel is "transparent"
                              }
                           }
                        }

                        p++;
                        x++;
                     }

                     if( x > x0 )         // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                     {
                        RECT * pr;
                        if( pData->rdh.nCount >= maxRects )
                        {
                           GlobalUnlock(hData);
                           maxRects += ALLOC_UNIT;
                           hData     = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
                           pData     = ( RGNDATA * ) GlobalLock(hData);
                        }

                        pr = ( RECT * ) &pData->Buffer;
                        SetRect(&pr[pData->rdh.nCount], x0, y, x, y + 1);
                        if( x0 < pData->rdh.rcBound.left )
                        {
                           pData->rdh.rcBound.left = x0;
                        }

                        if( y < pData->rdh.rcBound.top )
                        {
                           pData->rdh.rcBound.top = y;
                        }

                        if( x > pData->rdh.rcBound.right )
                        {
                           pData->rdh.rcBound.right = x;
                        }

                        if( y + 1 > pData->rdh.rcBound.bottom )
                        {
                           pData->rdh.rcBound.bottom = y + 1;
                        }

                        pData->rdh.nCount++;

                        // On Windows98, ExtCreateRegion() may fail if  the number of rectangles is too
                        // large (ie: > 4000).
                        // Therefore, we have to create the region by multiple steps.
                        if( pData->rdh.nCount == 2000 )
                        {
                           h = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + ( sizeof(RECT) * maxRects ), pData);
                           if( hRgn )
                           {
                              CombineRgn(hRgn, hRgn, h, RGN_OR);
                              DeleteObject(h);
                           }
                           else
                           {
                              hRgn = h;
                           }

                           pData->rdh.nCount = 0;
                           SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
                        }
                     }
                  }

                  // Go to next row (remember, the bitmap is inverted vertically)
                  p32 -= bm32.bmWidthBytes;
               }

               // Create or extend the region with the remaining  rectangles
               h = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + ( sizeof(RECT) * maxRects ), pData);
               if( hRgn )
               {
                  CombineRgn(hRgn, hRgn, h, RGN_OR);
                  DeleteObject(h);
               }
               else
               {
                  hRgn = h;
               }

               // Clean up
               GlobalFree(hData);
               SelectObject(hDC, holdBmp);
               DeleteDC(hDC);
            }

            DeleteObject(SelectObject(hMemDC, holdBmp));
         }

         DeleteDC(hMemDC);
      }
   }

   return hRgn;
}
