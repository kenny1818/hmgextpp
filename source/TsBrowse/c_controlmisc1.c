#define _WIN32_IE     0x0500
#define _WIN32_WINNT  0x0400

#include <mgdefs.h>
#include <commctrl.h>

#ifdef UNICODE
   LPWSTR AnsiToWide( LPCSTR );
   LPSTR  WideToAnsi( LPWSTR );
#endif
BOOL Array2Rect(PHB_ITEM aRect, RECT * rc);
PHB_ITEM             Rect2Array( RECT * rc );

// Minigui Resources control system
void RegisterResource(HANDLE hResource, LPSTR szType);
void pascal DelResource(HANDLE hResource);

static far BYTE HandXor[] = {
   0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
   0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x60, 0x00, 0x00, 0x03, 0x6C, 0x00, 0x00,
   0x03, 0x6D, 0x00, 0x00, 0x03, 0x6D, 0x80, 0x00, 0x33, 0xFD, 0x80, 0x00, 0x3B, 0xFF, 0x80, 0x00,
   0x1B, 0xFF, 0x80, 0x00, 0x0B, 0xFF, 0x80, 0x00, 0x0F, 0xFF, 0x80, 0x00, 0x07, 0xFF, 0x80, 0x00,
   0x07, 0xFF, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x00,
   0x01, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static far BYTE HandAnd[] = {
   0xFC, 0xFF, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF,
   0xF8, 0x7F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x03, 0xFF, 0xFF, 0xF8, 0x00, 0xFF, 0xFF,
   0xF8, 0x00, 0x7F, 0xFF, 0x88, 0x00, 0x3F, 0xFF, 0x80, 0x00, 0x3F, 0xFF, 0x80, 0x00, 0x3F, 0xFF,
   0xC0, 0x00, 0x3F, 0xFF, 0xE0, 0x00, 0x3F, 0xFF, 0xE0, 0x00, 0x3F, 0xFF, 0xF0, 0x00, 0x3F, 0xFF,
   0xF0, 0x00, 0x7F, 0xFF, 0xF8, 0x00, 0x7F, 0xFF, 0xF8, 0x00, 0x7F, 0xFF, 0xFC, 0x00, 0xFF, 0xFF,
   0xFC, 0x00, 0xFF, 0xFF, 0xFC, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static far BYTE StopXor[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x01, 0xF8, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x11, 0xF8, 0x80,
   0x00, 0x26, 0x08, 0x40, 0x00, 0x24, 0x12, 0x40, 0x00, 0x48, 0x25, 0x20, 0x00, 0x48, 0x49, 0x20,
   0x00, 0x48, 0x91, 0x20, 0x00, 0x49, 0x21, 0x20, 0x00, 0x4A, 0x41, 0x20, 0x00, 0x24, 0x82, 0x40,
   0x00, 0x21, 0x06, 0x40, 0x00, 0x11, 0xF8, 0x80, 0x00, 0x08, 0x01, 0x00, 0x00, 0x06, 0x06, 0x00,
   0x00, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static far BYTE StopAnd[] = {
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFE, 0x07, 0xFF, 0xFF, 0xF8, 0x01, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xE0, 0x00, 0x7F,
   0xFF, 0xC1, 0xF0, 0x3F, 0xFF, 0xC3, 0xE0, 0x3F, 0xFF, 0x87, 0xC2, 0x1F, 0xFF, 0x87, 0x86, 0x1F,
   0xFF, 0x87, 0x0E, 0x1F, 0xFF, 0x86, 0x1E, 0x1F, 0xFF, 0x84, 0x3E, 0x1F, 0xFF, 0xC0, 0x7C, 0x3F,
   0xFF, 0xC0, 0xF8, 0x3F, 0xFF, 0xE0, 0x00, 0x7F, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF8, 0x01, 0xFF,
   0xFF, 0xFE, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static far BYTE CatchXor[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x1B, 0xB0, 0x00, 0x00, 0x1D,
   0xD8, 0x00, 0x00, 0x0E, 0xDA, 0x00, 0x00, 0x06, 0xEA, 0x00, 0x00, 0x07, 0x6D, 0x00, 0x00, 0x03,
   0x6D, 0x00, 0x00, 0x03, 0x6D, 0x00, 0x01, 0x83, 0x6D, 0x00, 0x01, 0x83, 0x6D, 0x00, 0x01, 0x83,
   0xFF, 0x00, 0x01, 0xC3, 0xFF, 0x00, 0x00, 0xE7, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x7F,
   0xFE, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x13, 0xFC, 0x00, 0x00, 0x13, 0xFC, 0x00, 0x00, 0x1F,
   0xFC, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static far BYTE CatchAnd[] = {
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xE0, 0x4F, 0xFF, 0xFF, 0xC0, 0x07, 0xFF, 0xFF, 0xC0,
   0x01, 0xFF, 0xFF, 0xE0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x7F, 0xFF, 0xF8,
   0x00, 0x7F, 0xFE, 0x78, 0x00, 0x7F, 0xFC, 0x38, 0x00, 0x7F, 0xFC, 0x38, 0x00, 0x7F, 0xFC, 0x38,
   0x00, 0x7F, 0xFC, 0x18, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x7F, 0xFF, 0x00,
   0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xE0,
   0x03, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xC0,
   0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xE0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static far BYTE DragXor[] = {
   0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00,
   0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x7E, 0x00,
   0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x7E, 0x00,
   0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x03, 0x00,
   0x00, 0x00, 0x09, 0x8A, 0x00, 0x00, 0x11, 0x85, 0x00, 0x00, 0x08, 0xC2, 0x00, 0x00, 0x10, 0xC1,
   0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x15, 0x55,
   0x00, 0x00, 0x0A, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static far BYTE DragAnd[] = {
   0x3F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF,
   0xFF, 0xFF, 0x07, 0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
   0xFF, 0xFF, 0x00, 0x7F, 0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0xFF, 0x00, 0x1F, 0xFF, 0xFF, 0x00, 0x0F,
   0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x18, 0x7F, 0xFF, 0xFF, 0x18, 0x7F,
   0xFF, 0xFF, 0x64, 0x20, 0xFF, 0xFF, 0xE4, 0x38, 0xFF, 0xFF, 0xE6, 0x1C, 0xFF, 0xFF, 0xE6, 0x1C,
   0xFF, 0xFF, 0xE7, 0x3C, 0xFF, 0xFF, 0xE7, 0xFC, 0xFF, 0xFF, 0xE7, 0xFC, 0xFF, 0xFF, 0xE0, 0x00,
   0xFF, 0xFF, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static far HCURSOR hHand  = NULL;
static far HCURSOR hStop  = NULL;
static far HCURSOR hCatch = NULL;
static far HCURSOR hDrag  = NULL;

HB_FUNC( NOR )
{
   int p = hb_pcount();
   int n, ret = 0;

   for( n = 1; n <= p; n++ )
   {
      ret = ret | hb_parni( n );
   }

   hb_retni( ret );
}

HB_FUNC( CREATEPEN )
{
   HPEN     hpen;
   int      fnPenStyle = hb_parni(1);   // pen style
   int      nWidth     = hb_parni(2);   // pen width
   COLORREF crColor    = hb_parni(3);   // pen color

   hpen = CreatePen(fnPenStyle, nWidth, crColor);

   HB_RETNL( ( LONG_PTR ) hpen );
}

HB_FUNC( MOVETO )
{
   POINT pt;

   MoveToEx( hmg_par_HDC(1), ( INT ) hb_parni(2), ( INT ) hb_parni(3), &pt );
}

HB_FUNC( LINETO )
{
   LineTo( hmg_par_HDC(1), ( INT ) hb_parni(2), ( INT ) hb_parni(3) );
}

HB_FUNC( DRAWICON )
{
   hb_retl( DrawIcon(hmg_par_HDC(1), hb_parni(2), hb_parni(3), ( HICON ) HB_PARNL(4)) );
}

HB_FUNC( CURSORWE )
{
   HB_RETNL( ( LONG_PTR ) SetCursor(LoadCursor(0, IDC_SIZEWE)) );
}

HB_FUNC( CURSORSIZE )
{
   HB_RETNL( ( LONG_PTR ) SetCursor(LoadCursor(0, IDC_SIZEALL)) );
}

HB_FUNC( RELEASECAPTURE )
{
   hb_retl( ReleaseCapture() );
}

HB_FUNC( INVERTRECT )
{
   RECT rc;

   if( HB_ISARRAY(2) )
   {
      Array2Rect(hb_param( 2, Harbour::Item::ARRAY ), &rc);
      InvertRect(hmg_par_HDC(1), &rc);
   }
}

HB_FUNC( GETCLASSINFO )
{
#ifndef UNICODE
   LPCSTR lpString = ( LPCSTR ) hb_parc(2);
#else
   LPWSTR lpString = AnsiToWide( ( char * ) hb_parc(2) );
   LPSTR pStr;
#endif
   WNDCLASS WndClass;

   if( GetClassInfo(HB_ISNIL(1) ? NULL : ( HINSTANCE ) HB_PARNL(1), lpString, &WndClass) )
   {
   #ifdef UNICODE
      hb_reta(1);
      pStr = WideToAnsi( ( LPWSTR ) WndClass.lpszClassName );
      HB_STORC( pStr, -1, 1 );
      hb_xfree(pStr);
   #else
      hb_retclen( ( char * ) &WndClass, sizeof(WNDCLASS) );
   #endif
   }

#ifdef UNICODE
   hb_xfree(lpString);
#endif
}

HB_FUNC( SETCAPTURE )
{
   HB_RETNL( ( LONG_PTR ) SetCapture( hmg_par_HWND(1) ) );
}

HB_FUNC( GETTEXTCOLOR )
{
   hb_retnl( ( ULONG ) GetTextColor(hmg_par_HDC(1)) );
}

HB_FUNC( GETBKCOLOR )
{
   hb_retnl( ( ULONG ) GetBkColor(hmg_par_HDC(1)) );
}

HB_FUNC( MOVEFILE )
{
#ifndef UNICODE
   LPCSTR lpExistingFileName = hb_parc(1);
   LPCSTR lpNewFileName = hb_parc(2);
#else
   LPWSTR lpExistingFileName = AnsiToWide( ( char * ) hb_parc(1) );
   LPWSTR lpNewFileName = AnsiToWide( ( char * ) hb_parc(2) );
#endif

   hb_retl( ( BOOL ) MoveFile(lpExistingFileName, lpNewFileName) );

#ifdef UNICODE
   hb_xfree(lpExistingFileName);
   hb_xfree(lpNewFileName);
#endif
}

HB_FUNC( GETACP )
{
   hb_retni( ( UINT ) GetACP() );
}

HB_FUNC( GETCURSORHAND )
{
   if( ! hHand )
   {
      hHand = CreateCursor(GetModuleHandle(NULL), 6, 0, 32, 32, HandAnd, HandXor );
      RegisterResource(hHand, "CUR");
   }

   HB_RETNL( ( LONG_PTR ) hHand );
}

HB_FUNC( GETCURSORDRAG )
{
   if( ! hDrag )
   {
      hDrag = CreateCursor(GetModuleHandle(NULL), 6, 0, 32, 32, DragAnd, DragXor);
      RegisterResource(hDrag, "CUR");
   }

   HB_RETNL( ( LONG_PTR ) hDrag );
}

HB_FUNC( GETCURSORCATCH )
{
   if( ! hCatch )
   {
      hCatch = CreateCursor(GetModuleHandle(NULL), 16, 16, 32, 32, CatchAnd, CatchXor);
      RegisterResource(hCatch, "CUR");
   }

   HB_RETNL( ( LONG_PTR ) hCatch );
}

HB_FUNC( GETCURSORSTOP )
{
   if( ! hStop )
   {
      hStop = CreateCursor(GetModuleHandle(NULL), 6, 0, 32, 32, StopAnd, StopXor);
      RegisterResource(hStop, "CUR");
   }

   HB_RETNL( ( LONG_PTR ) hStop );
}

HB_FUNC( CURSORSTOP )
{
   if( ! hStop )
   {
      hStop = CreateCursor(GetModuleHandle(NULL), 6, 0, 32, 32, StopAnd, StopXor);
   }

   SetCursor(hStop);
}

HB_FUNC( DESTROYCURSOR )
{
   HCURSOR hCur = ( HCURSOR ) HB_PARNL(1);

   if( hCur == hDrag )
      hDrag  = NULL;
   else if( hCur == hCatch )
      hCatch = NULL;
   else if( hCur == hStop )
      hStop  = NULL;
   else if( hCur == hHand )
      hHand  = NULL;

   DelResource(hCur);
   hb_retl( ( BOOL ) DestroyCursor(hCur) );
}

HB_FUNC( GETWHEELSCROLLLINES )
{
   UINT pulScrollLines;

   SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &pulScrollLines, 0);

   hb_retni( pulScrollLines );
}
