/*
 * MINIGUI - Harbour Win32 GUI library source code
 *
 * Copyright 2002-2010 Roberto Lopez <harbourminigui@gmail.com>
 * http://harbourminigui.googlepages.com/
 *
 * IMAGELIST control source code
 * (C)2005 Janusz Pora <januszpora@onet.eu>
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

#include "mgdefs.hpp"

#include <commctrl.h>

#if defined( __BORLANDC__ )
WINCOMMCTRLAPI void WINAPI ImageList_EndDrag(void);
#endif

extern HBITMAP HMG_LoadImage(const char * FileName, const char * pszTypeOfRes);
extern HBITMAP HMG_LoadPicture(const char * FileName, int New_Width, int New_Height, HWND hWnd, int ScaleStretch, int Transparent, long BackgroundColor, int AdjustImage,
                               HB_BOOL bAlphaFormat, int iAlpfaConstant);

#ifdef UNICODE
LPWSTR AnsiToWide(LPCSTR);
#endif

HB_FUNC( INITIMAGELIST )   //InitImageList ( cx , cy, mask , nCount )
{
   HIMAGELIST himlIcons;
   UINT       Styl = ILC_COLOR32;

   if( hb_parl(3) )
   {
      Styl = Styl | ILC_MASK;
   }

   InitCommonControls();

   himlIcons = ImageList_Create(hmg_par_INT(1), hmg_par_INT(2), Styl, hmg_par_INT(4), 0);

   RegisterResource(himlIcons, "IMAGELIST");
   hmg_ret_HANDLE(himlIcons);
}

HB_FUNC( IL_ADD )          //IL_Add( himl , image , maskimage , ix , iy , imagecount )
{
#ifndef UNICODE
   LPCSTR lpImageName  = hb_parc(2);
   LPCSTR lpImageName2 = hb_parc(3);
#else
   LPWSTR lpImageName  = AnsiToWide(( char * ) hb_parc(2));
   LPWSTR lpImageName2 = AnsiToWide(( char * ) hb_parc(3));
#endif
   BITMAP  bm;
   HBITMAP himage1;  // handle to image
   HBITMAP himage2;  // handle to maskimage
   int     lResult = -1;
   int     ic      = 1;

   if( hb_parni(6) )
   {
      ic = hb_parni(6);
   }

   himage1 = static_cast<HBITMAP>(LoadImage(GetResources(), lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
   if( himage1 == nullptr )
   {
      himage1 = static_cast<HBITMAP>(LoadImage(nullptr, lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
   }

   if( himage1 == nullptr )
   {
      himage1 = static_cast<HBITMAP>(HMG_LoadImage(hb_parc(2), nullptr));
   }

   himage2 = 0;
   if( hb_parclen(3) )
   {
      himage2 = static_cast<HBITMAP>(LoadImage(GetResources(), lpImageName2, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
      if( himage2 == nullptr )
      {
         himage2 = static_cast<HBITMAP>(LoadImage(nullptr, lpImageName2, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
      }

      if( himage2 == nullptr )
      {
         himage2 = static_cast<HBITMAP>(HMG_LoadImage(hb_parc(3), nullptr));
      }
   }

#ifdef UNICODE
   hb_xfree(lpImageName);
   hb_xfree(lpImageName2);
#endif
   if( GetObject(himage1, sizeof(BITMAP), &bm) != 0 )
   {
      if( ( hb_parni(4) * ic == bm.bmWidth ) && ( hb_parni(5) == bm.bmHeight ) )
      {
         lResult = ImageList_Add( hmg_par_HIMAGELIST(1), himage1, himage2 );
      }

      DeleteObject(himage1);
      if( himage2 )
      {
         DeleteObject(himage2);
      }
   }

   hb_retni( lResult );
}

HB_FUNC( IL_ADDMASKED )    //IL_AddMasked(himl , image , color , ix , iy , imagecount)
{
#ifndef UNICODE
   LPCSTR lpImageName = hb_parc(2);
#else
   LPWSTR lpImageName = AnsiToWide(( char * ) hb_parc(2));
#endif
   BITMAP   bm;
   HBITMAP  himage1; // handle to image
   COLORREF clrBk   = CLR_NONE;
   int      lResult = -1;
   int      ic      = 1;

   if( hb_parnl(3) )
   {
      clrBk = hmg_par_COLORREF(3);
   }

   if( hb_parni(6) )
   {
      ic = hb_parni(6);
   }

   himage1 = static_cast<HBITMAP>(LoadImage(GetResources(), lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
   if( himage1 == nullptr )
   {
      himage1 = static_cast<HBITMAP>(LoadImage(nullptr, lpImageName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT));
   }

   if( himage1 == nullptr )
   {
      himage1 = static_cast<HBITMAP>(HMG_LoadPicture(hb_parc(2), -1, -1, nullptr, 0, 1, -1, 0, HB_FALSE, 255));
   }

#ifdef UNICODE
   hb_xfree(lpImageName);
#endif
   if( GetObject(himage1, sizeof(BITMAP), &bm) != 0 )
   {
      if( ( hb_parni(4) * ic == bm.bmWidth ) && ( hb_parni(5) == bm.bmHeight ) )
      {
         lResult = ImageList_AddMasked(hmg_par_HIMAGELIST(1), himage1, clrBk);
      }

      DeleteObject(himage1);
   }

   hb_retni( lResult );
}

HB_FUNC( IL_DRAW )         //BOOL IL_Draw(HWND hwnd, HIMAGELIST himl, int imageindex, cx , cy)
{
   HDC  hdc;
   HWND hwnd = hmg_par_HWND(1);

   if( ( hdc = GetDC(hwnd) ) == nullptr )
   {
      hb_retl(FALSE);
   }

   if( !ImageList_Draw(hmg_par_HIMAGELIST(2), hmg_par_INT(3), hdc, hb_parni(4), hb_parni(5), ILD_TRANSPARENT) )
   {
      hb_retl(FALSE);
   }

   ReleaseDC(hwnd, hdc);

   hb_retl(TRUE);
}

HB_FUNC( IL_REMOVE )       //IL_Remove(hwnd , imageindex)
{
   HIMAGELIST himlIcons;

   himlIcons = hmg_par_HIMAGELIST(1);

   hb_retl(ImageList_Remove(himlIcons, hmg_par_INT(2)));
}

HB_FUNC( IL_SETBKCOLOR )   //IL_SetBkColor(hwnd, color)
{
   COLORREF clrBk = CLR_NONE;

   if( hb_parnl(2) )
   {
      clrBk = hb_parnl(2);
   }

   hb_retnl( ( COLORREF ) ImageList_SetBkColor(hmg_par_HIMAGELIST(1), clrBk) );
}

HB_FUNC( IL_ERASEIMAGE )   //IL_EraseImage(hwnd, ix, iy, dx, dy)
{
   RECT rcImage;

   SetRect(&rcImage, hb_parni(2), hb_parni(3), hb_parni(4) + hb_parni(2), hb_parni(5) + hb_parni(3));

   InvalidateRect(hmg_par_HWND(1), &rcImage, TRUE);
   UpdateWindow(hmg_par_HWND(1));
}

HB_FUNC( IL_BEGINDRAG )    //IL_BeginDrag(hwnd, himl, ImageInx, ix, iy)
{
   INT  cx;
   INT  cy;
   RECT rcImage;

   if( ImageList_GetIconSize(hmg_par_HIMAGELIST(2), &cx, &cy) )
   {
      SetRect(&rcImage, hb_parni(4) - 2, hb_parni(5) - 2, hb_parni(4) + cx + 2, hb_parni(5) + cy + 2);
      InvalidateRect(hmg_par_HWND(1), &rcImage, TRUE);
      UpdateWindow(hmg_par_HWND(1));
   }

   hb_retl(ImageList_BeginDrag(hmg_par_HIMAGELIST(2), hmg_par_INT(3), ( INT ) 0, ( INT ) 0));
}

HB_FUNC( IL_DRAGMOVE )     //IL_DragMove(ix, iy)
{
   hb_retl(ImageList_DragMove(hmg_par_INT(1), hmg_par_INT(2)));
}

HB_FUNC( IL_DRAGENTER )    //IL_DragEnter( hwnd, ix, iy )
{
   hb_retl(ImageList_DragEnter( hmg_par_HWND(1), hmg_par_INT(2), hmg_par_INT(3) ));
}

HB_FUNC( IL_ENDDRAG )      //IL_EndDrag(hwnd)
{
   ImageList_EndDrag();
   ImageList_DragLeave(hmg_par_HWND(1));
}

HB_FUNC( IL_GETIMAGECOUNT ) //IL_GetImageCount(himl)
{
   hb_retni( ImageList_GetImageCount(hmg_par_HIMAGELIST(1)) );
}
