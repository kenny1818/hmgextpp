//
// MINIGUI - Harbour Win32 GUI library source code
//
// Copyright 2016 Grigory Filatov <gfilatov@gmail.com>
//

#include "minigui.ch"

#ifdef _USERINIT_

//----------------------------------------------------------------------------//
INIT PROCEDURE _InitCLButton
//----------------------------------------------------------------------------//

   InstallEventHandler("CLButtonEventHandler")
   InstallMethodHandler("Release", "ReleaseCLButtonImageList")
   InstallMethodHandler("SetShield", "CLButton_SetShield")
   InstallMethodHandler("SetFocus", "CLButtonSetFocus")
   InstallMethodHandler("Enable", "CLButtonEnable")
   InstallMethodHandler("Disable", "CLButtonDisable")
   InstallPropertyHandler("Handle", "SetCLButtonHandle", "GetCLButtonHandle")
   InstallPropertyHandler("Caption", "SetCLButtonCaption", "GetCLButtonCaption")
   InstallPropertyHandler("NoteText", "SetCLButtonNoteText", "GetCLButtonNoteText")
   InstallPropertyHandler("Picture", "SetCLButtonPicture", "GetCLButtonPicture")

RETURN

//----------------------------------------------------------------------------//
PROCEDURE _DefineCLButton(cName, nRow, nCol, cCaption, cNotes, bAction, cParent, lDefault, w, h, cBitmap)
//----------------------------------------------------------------------------//
   
   LOCAL hControlHandle
   LOCAL hParentFormHandle
   LOCAL mVar
   LOCAL k
   LOCAL nId

   IF _HMG_BeginWindowActive
      cParent := _HMG_ActiveFormName
   ENDIF

   // If defined inside a Tab structure, adjust position and determine cParent
   IF _HMG_FrameLevel > 0
      nCol += _HMG_ActiveFrameCol[_HMG_FrameLevel]
      nRow += _HMG_ActiveFrameRow[_HMG_FrameLevel]
      cParent := _HMG_ActiveFrameParentFormName[_HMG_FrameLevel]
   ENDIF

   IF !_IsWindowDefined(cParent)
      MsgMiniGuiError("Window: " + cParent + " is not defined.")
   ENDIF

   IF _IsControlDefined(cName, cParent)
      MsgMiniGuiError("Control: " + cName + " Of " + cParent + " Already defined.")
   ENDIF

   hb_default(@w, 180)
   hb_default(@h, 60)

   mVar := "_" + cParent + "_" + cName

   k := _GetControlFree()
   nId := _GetId()

   hParentFormHandle := GetFormHandle(cParent)

   hControlHandle := hmg_InitCLButton(hParentFormHandle, nRow, nCol, cCaption, lDefault, w, h, nId)

   hmg_CLButton_SetNote(hControlHandle, cNotes)

   IF _HMG_BeginTabActive
      AAdd(_HMG_ActiveTabCurrentPageMap, hControlHandle)
   ENDIF

#ifdef _NAMES_LIST_
   _SetNameList(mVar, k)
#else
   Public &mVar. := k
#endif

   _HMG_aControlType               [k] := CONTROL_TYPE_CLBUTTON
   _HMG_aControlNames              [k] := cName
   _HMG_aControlHandles            [k] := hControlHandle
   _HMG_aControlParenthandles      [k] := hParentFormHandle
   _HMG_aControlIds                [k] := nId
   _HMG_aControlProcedures         [k] := bAction
   _HMG_aControlPageMap            [k] := {}
   _HMG_aControlValue              [k] := NIL
   _HMG_aControlInputMask          [k] := ""
   _HMG_aControllostFocusProcedure [k] := ""
   _HMG_aControlGotFocusProcedure  [k] := ""
   _HMG_aControlChangeProcedure    [k] := ""
   _HMG_aControlDeleted            [k] := .F.
   _HMG_aControlBkColor            [k] := NIL
   _HMG_aControlFontColor          [k] := NIL
   _HMG_aControlDblClick           [k] := ""
   _HMG_aControlHeadClick          [k] := {}
   _HMG_aControlRow                [k] := nRow
   _HMG_aControlCol                [k] := nCol
   _HMG_aControlWidth              [k] := w
   _HMG_aControlHeight             [k] := h
   _HMG_aControlSpacing            [k] := 0
   _HMG_aControlContainerRow       [k] := iif(_HMG_FrameLevel > 0, _HMG_ActiveFrameRow[_HMG_FrameLevel], -1)
   _HMG_aControlContainerCol       [k] := iif(_HMG_FrameLevel > 0, _HMG_ActiveFrameCol[_HMG_FrameLevel], -1)
   _HMG_aControlPicture            [k] := "Arrow"
   _HMG_aControlContainerHandle    [k] := HMG_NULLHANDLE
   _HMG_aControlFontName           [k] := NIL
   _HMG_aControlFontSize           [k] := NIL
   _HMG_aControlFontAttributes     [k] := {}
   _HMG_aControlToolTip            [k] := ""
   _HMG_aControlRangeMin           [k] := 0
   _HMG_aControlRangeMax           [k] := 0
   _HMG_aControlCaption            [k] := ""
   _HMG_aControlVisible            [k] := .T.
   _HMG_aControlHelpId             [k] := 0
   _HMG_aControlFontHandle         [k] := NIL
   _HMG_aControlBrushHandle        [k] := HMG_NULLHANDLE
   _HMG_aControlEnabled            [k] := .T.
   _HMG_aControlMiscData1          [k] := 0
   _HMG_aControlMiscData2          [k] := ""

   IF _HMG_lOOPEnabled
      Eval(_HMG_bOnControlInit, k, mVar)
   ENDIF

   IF !Empty(cBitmap)
      _HMG_aControlPicture[k] := cBitmap
      _HMG_aControlBrushHandle[k] := hmg_CLButton_SetImage(hControlHandle, cBitmap)
   ENDIF

RETURN

//----------------------------------------------------------------------------//
PROCEDURE ReleaseCLButtonImageList(cWindow, cControl)
//----------------------------------------------------------------------------//

   LOCAL i

   IF _IsControlDefined(cControl, cWindow) .AND. GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      i := GetControlIndex(cControl, cWindow)

      IF !Empty(_HMG_aControlBrushHandle[i])
         hmg_IMAGELIST_DESTROY(_HMG_aControlBrushHandle[i])
      ENDIF

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

#define WM_COMMAND  0x0111
#define BN_CLICKED  0
//----------------------------------------------------------------------------//
FUNCTION CLButtonEventhandler(hWnd, nMsg, wParam, lParam)
//----------------------------------------------------------------------------//
   
   LOCAL RetVal // := NIL
   LOCAL i

   HB_SYMBOL_UNUSED(hWnd)

   IF nMsg == WM_COMMAND

      i := AScan(_HMG_aControlHandles, hmg_numbertohandle(lParam))

      IF i > 0 .AND. _HMG_aControlType[i] == CONTROL_TYPE_CLBUTTON

         IF hmg_HiWord(wParam) == BN_CLICKED
            RetVal := 0
            _DoControlEventProcedure(_HMG_aControlProcedures[i], i)
         ENDIF

      ENDIF

   ENDIF

RETURN RetVal

#define BCM_SETSHIELD      0x0000160C
//----------------------------------------------------------------------------//
PROCEDURE CLButton_SetShield(cWindow, cControl)
//----------------------------------------------------------------------------//
   
   LOCAL i

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      i := GetControlIndex(cControl, cWindow)

      _HMG_aControlPicture[i] := "Shield"

      hmg_SendMessage(GetControlHandle(cControl, cWindow), BCM_SETSHIELD, 0, 0xFFFFFFFF)

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

#define BS_COMMANDLINK     0x0000000E
#define BS_DEFCOMMANDLINK  0x0000000F
#define BM_SETSTYLE        244
//----------------------------------------------------------------------------//
PROCEDURE CLButtonSetFocus(cWindow, cControl)
//----------------------------------------------------------------------------//
   
   LOCAL hWnd
   LOCAL ParentFormHandle
   LOCAL ControlCount
   LOCAL x

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      _HMG_UserComponentProcess := .T.

      hWnd := GetControlHandle(cControl, cWindow)
      ControlCount := Len(_HMG_aControlNames)
      ParentFormHandle := _HMG_aControlParentHandles[GetControlIndex(cControl, cWindow)]
      FOR x := 1 TO ControlCount
         IF _HMG_aControlType[x] == CONTROL_TYPE_CLBUTTON
            IF _HMG_aControlParentHandles[x] == ParentFormHandle
               hmg_SendMessage(_HMG_aControlHandles[x], BM_SETSTYLE, hmg_LOWORD(BS_COMMANDLINK), 1)
            ENDIF
         ENDIF
      NEXT
      hmg_SetFocus(hWnd)
      hmg_SendMessage(hWnd, BM_SETSTYLE, hmg_LOWORD(BS_DEFCOMMANDLINK), 1)

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

//----------------------------------------------------------------------------//
PROCEDURE CLButtonEnable(cWindow, cControl)
//----------------------------------------------------------------------------//

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      hmg_EnableWindow(GetControlHandle(cControl, cWindow))

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

//----------------------------------------------------------------------------//
PROCEDURE CLButtonDisable(cWindow, cControl)
//----------------------------------------------------------------------------//

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      hmg_DisableWindow(GetControlHandle(cControl, cWindow))

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

//----------------------------------------------------------------------------//
FUNCTION SetCLButtonHandle(cWindow, cControl)
//----------------------------------------------------------------------------//

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      MsgExclamation("This Property is Read Only!", "Warning")

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN NIL

//----------------------------------------------------------------------------//
FUNCTION GetCLButtonHandle(cWindow, cControl)
//----------------------------------------------------------------------------//

   LOCAL RetVal // := NIL

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      _HMG_UserComponentProcess := .T.

      RetVal := GetControlHandle(cControl, cWindow)

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN RetVal

//----------------------------------------------------------------------------//
FUNCTION SetCLButtonCaption(cWindow, cControl, cProperty, cValue)
//----------------------------------------------------------------------------//
   cProperty := NIL // Unused variable

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      _HMG_UserComponentProcess := .T.

      hmg_SetWindowText(GetControlHandle(cControl, cWindow), cValue)

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN NIL

//----------------------------------------------------------------------------//
FUNCTION GetCLButtonCaption(cWindow, cControl)
//----------------------------------------------------------------------------//
   
   LOCAL RetVal // := NIL

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      _HMG_UserComponentProcess := .T.

      RetVal := hmg_GetWindowText(GetControlHandle(cControl, cWindow))

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN RetVal

//----------------------------------------------------------------------------//
PROCEDURE SetCLButtonNoteText(cWindow, cControl, cProperty, cValue)
//----------------------------------------------------------------------------//
   cProperty := NIL // Unused variable

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      hmg_CLButton_SetNote(GetControlHandle(cControl, cWindow), cValue)

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

//----------------------------------------------------------------------------//
FUNCTION GetCLButtonNoteText(cWindow, cControl)
//----------------------------------------------------------------------------//

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      MsgExclamation("This Property is Write Only!", "Warning")

   ENDIF

   _HMG_UserComponentProcess := .F.

RETURN NIL

//----------------------------------------------------------------------------//
PROCEDURE SetCLButtonPicture(cWindow, cControl, cProperty, cBitmap)
//----------------------------------------------------------------------------//
   
   LOCAL i
   
   cProperty := NIL // Unused variable

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      i := GetControlIndex(cControl, cWindow)

      _HMG_aControlPicture[i] := cBitmap

      IF !Empty(_HMG_aControlBrushHandle[i])
         hmg_IMAGELIST_DESTROY(_HMG_aControlBrushHandle[i])
      ENDIF

      _HMG_aControlBrushHandle[i] := hmg_CLButton_SetImage(GetControlHandle(cControl, cWindow), cBitmap)

      _HMG_UserComponentProcess := .T.

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN

//----------------------------------------------------------------------------//
FUNCTION GetCLButtonPicture(cWindow, cControl)
//----------------------------------------------------------------------------//
   
   LOCAL RetVal

   IF GetControlType(cControl, cWindow) == CONTROL_TYPE_CLBUTTON

      _HMG_UserComponentProcess := .T.

      RetVal := _GetPicture(cControl, cWindow)

   ELSE

      _HMG_UserComponentProcess := .F.

   ENDIF

RETURN RetVal

//----------------------------------------------------------------------------//
// Low Level C Routines
//----------------------------------------------------------------------------//

#pragma BEGINDUMP

#define BS_COMMANDLINK     0x0000000E
#define BS_DEFCOMMANDLINK  0x0000000F

#include "mgdefs.hpp"
#include <commctrl.h>

#ifdef UNICODE
   LPWSTR AnsiToWide(LPCSTR);
#endif

HB_FUNC(HMG_INITCLBUTTON)
{
   int style;
#ifndef UNICODE
   LPCSTR lpWindowName = hb_parc(4);
#else
   LPWSTR lpWindowName = AnsiToWide(const_cast<char*>(hb_parc(4)));
#endif

   style = BS_COMMANDLINK;

   if (hb_parl( 5 ))
   {
      style = BS_DEFCOMMANDLINK;
   }

   auto hbutton = CreateWindowEx(0,
                                 "button",
                                 lpWindowName,
                                 style | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_PUSHBUTTON | WS_VISIBLE,
                                 hmg_par_int(3),
                                 hmg_par_int(2),
                                 hmg_par_int(6),
                                 hmg_par_int(7),
                                 hmg_par_HWND(1),
                                 hmg_par_HMENU(8),
                                 GetModuleHandle(nullptr),
                                 nullptr);

   hmg_ret_HWND(hbutton);
}

#ifndef BCM_SETNOTE
#define BCM_SETNOTE  0x00001609
#endif

HB_FUNC(HMG_CLBUTTON_SETNOTE)
{
   if (HB_ISCHAR(2))
   {
      LPSTR  szText        = const_cast<LPSTR>(hb_parc(2));
      int    nConvertedLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szText, -1, nullptr, 0);
      auto lpwText = static_cast<LPWSTR>(hb_xgrab(nConvertedLen * 2 + 1));

      MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szText, -1, lpwText, nConvertedLen);

      SendMessage(hmg_par_HWND(1), BCM_SETNOTE, 0, reinterpret_cast<LPARAM>(lpwText));
      hb_xfree(lpwText);
   }
}

#ifndef BCM_FIRST
#define BCM_FIRST         0x1600
#define BCM_SETIMAGELIST  ( BCM_FIRST + 0x0002 )
#endif

#if (defined(__BORLANDC__) && __BORLANDC__ < 1410) || (defined(__MINGW32__) && defined(__MINGW32_VERSION))
typedef struct
{
   HIMAGELIST himl;
   RECT       margin;
   UINT       uAlign;
} BUTTON_IMAGELIST, * PBUTTON_IMAGELIST;
#endif

HB_FUNC(HMG_CLBUTTON_SETIMAGE)
{
   HIMAGELIST       himl;
   BUTTON_IMAGELIST bi;
#ifndef UNICODE
   LPCTSTR lpImageName = hb_parc(2);
#else
   LPWSTR  lpImageName = AnsiToWide(const_cast<char*>(hb_parc(2)));
#endif

   himl = ImageList_LoadImage
          (
      GetModuleHandle(nullptr),
      lpImageName,
      0,
      6,
      CLR_DEFAULT,
      IMAGE_BITMAP,
      LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT
          );

   if (himl == nullptr)
   {
      himl = ImageList_LoadImage
             (
         GetModuleHandle(nullptr),
         lpImageName,
         0,
         6,
         CLR_DEFAULT,
         IMAGE_BITMAP,
         LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT
             );
   }

   bi.himl          = himl;
   bi.margin.left   = 10;
   bi.margin.top    = 10;
   bi.margin.bottom = 10;
   bi.margin.right  = 10;
   bi.uAlign        = 4;

   SendMessage(hmg_par_HWND(1), BCM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(&bi));

   hmg_ret_HIMAGELIST(himl);
}

#ifndef HMG_NO_DEPRECATED_FUNCTIONS
HB_FUNC_TRANSLATE( INITCLBUTTON, HMG_INITCLBUTTON )
HB_FUNC_TRANSLATE( CLBUTTON_SETNOTE, HMG_CLBUTTON_SETNOTE )
HB_FUNC_TRANSLATE( CLBUTTON_SETIMAGE, HMG_CLBUTTON_SETIMAGE )
#endif

#pragma ENDDUMP

#endif
