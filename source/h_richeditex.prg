//
// HMG - Harbour Windows GUI library source code
//
// Copyright 2002-2017 Roberto Lopez <mail.box.hmg@gmail.com>
// http://sites.google.com/site/hmgweb/
//
// Head of HMG project:
//
// 2002-2012 Roberto Lopez <mail.box.hmg@gmail.com>
// http://sites.google.com/site/hmgweb/
//
// 2012-2017 Dr. Claudio Soto <srvet@adinet.com.uy>
// http://srvet.blogspot.com
//

// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this software; see the file COPYING. If not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA (or
// visit the web site http://www.gnu.org/).
//
// As a special exception, you have permission for additional uses of the text
// contained in this release of HMG.
//
// The exception is that, if you link the HMG library with other
// files to produce an executable, this does not by itself cause the resulting
// executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of linking the
// HMG library code into it.

// Parts of this project are based upon:
//
// "Harbour GUI framework for Win32"
// Copyright 2001 Alexander S.Kresin <alex@kresin.ru>
// Copyright 2001 Antonio Linares <alinares@fivetech.com>
// www - https://harbour.github.io/
//
// "Harbour Project"
// Copyright 1999-2022, https://harbour.github.io/
//
// "WHAT32"
// Copyright 2002 AJ Wos <andrwos@aust1.net>
//
// "HWGUI"
// Copyright 2001-2008 Alexander S.Kresin <alex@kresin.ru>

#include "hmg.ch"

#ifdef _HMG_COMPAT_

#include "i_winuser.ch"
#include "fileio.ch"

//---------------------------------------------------------------------------//
FUNCTION _DefineRichEditBoxEx(ControlName, ParentForm, x, y, w, h, value, fontname, fontsize, tooltip, maxlength, ;
   gotfocus, change, lostfocus, readonly, break, HelpId, invisible, notabstop, bold, italic, underline, strikeout, ;
   field, backcolor, noHscroll, noVscroll, selectionchange, OnLink, OnVScroll)
//---------------------------------------------------------------------------//

   LOCAL i
   LOCAL cParentForm
   LOCAL mVar
   LOCAL ContainerHandle := 0
   LOCAL k
   LOCAL ControlHandle
   LOCAL FontHandle
   LOCAL WorkArea
   LOCAL aRichEditMenu

   hb_default(@w, 120)
   hb_default(@h, 240)
   hb_default(@value, "")
   hb_default(@maxlength, -1)
   hb_default(@invisible, .F.)
   hb_default(@notabstop, .F.)
   hb_default(@noHscroll, .F.)
   hb_default(@noVscroll, .F.)

   IF maxlength == 0
      maxlength := -1 // for compatibility with TextBox and EditBox
   ENDIF

   IF Field != NIL
      IF hb_UAt(">", Field) == 0
         MsgHMGError("Control: " + ControlName + " Of " + ParentForm + " : You must specify a fully qualified field name.")
      ELSE
         WorkArea := hb_ULeft(Field, hb_UAt(">", Field) - 2)
         IF SELECT(WorkArea) != 0
            value := &(Field)
         ENDIF
      ENDIF
   ENDIF

   IF _HMG_BeginWindowActive = .T.
      ParentForm := _HMG_ActiveFormName
      IF !Empty(_HMG_DefaultFontName) .AND. FontName == NIL
         fontname := _HMG_DefaultFontName
      ENDIF
      IF !Empty(_HMG_DefaultFontSize) .AND. FontSize == NIL
         fontsize := _HMG_DefaultFontSize
      ENDIF
   ENDIF
   IF _HMG_FrameLevel > 0
      IF !_HMG_ParentWindowActive
         x += _HMG_ActiveFrameCol[_HMG_FrameLevel]
         y += _HMG_ActiveFrameRow[_HMG_FrameLevel]
         ParentForm := _HMG_ActiveFrameParentFormName[_HMG_FrameLevel]
      ENDIF
   ENDIF

   IF !_IsWindowDefined(ParentForm)
      MsgHMGError("Window: " + ParentForm + " is not defined.")
   ENDIF

   IF _IsControlDefined(ControlName, ParentForm)
      MsgHMGError("Control: " + ControlName + " Of " + ParentForm + " Already defined.")
   ENDIF

   mVar := "_" + ParentForm + "_" + ControlName

   cParentForm := ParentForm

   ParentForm = GetFormHandle(ParentForm)

   IF x == NIL .OR. y == NIL

      IF _HMG_SplitLastControl == "TOOLBAR"
         Break := .T.
      ENDIF

      _HMG_SplitLastControl := "RICHEDIT"

      i := GetFormIndex(cParentForm)

      IF i > 0

         ControlHandle := hmg_InitRichEditBoxEx(_HMG_aFormReBarHandle[i], 0, x, y, w, h, "", 0, maxlength, readonly, invisible, notabstop, noHscroll, noVscroll)
         IF fontname != NIL .AND. fontsize != NIL
            FontHandle := hmg__SetFont(ControlHandle, fontname, fontsize, bold, italic, underline, strikeout)
         ELSE
            FontHandle := hmg__SetFont(ControlHandle, _HMG_DefaultFontName, _HMG_DefaultFontSize, bold, italic, underline, strikeout)
         ENDIF

         hmg_AddSplitBoxItem(Controlhandle, _HMG_aFormReBarHandle[i], w, break, , , , _HMG_ActiveSplitBoxInverted)
         Containerhandle := _HMG_aFormReBarHandle[i]

         IF LEN(value) > 0
            hmg_SetWindowText(ControlHandle, value)
         ENDIF

      ENDIF

   ELSE

      ControlHandle := hmg_InitRichEditBoxEx(ParentForm, 0, x, y, w, h, "", 0, maxlength, readonly, invisible, notabstop, noHscroll, noVscroll)
      IF hmg_IsWindowHandle(ControlHandle)
         IF fontname != NIL .AND. fontsize != NIL
            FontHandle := hmg__SetFont(ControlHandle, fontname, fontsize, bold, italic, underline, strikeout)
         ELSE
            FontHandle := hmg__SetFont(ControlHandle, _HMG_DefaultFontName, _HMG_DefaultFontSize, bold, italic, underline, strikeout)
         ENDIF
      ENDIF

      IF LEN(value) > 0
         hmg_SetWindowText(ControlHandle, value)
      ENDIF

   ENDIF

   IF _HMG_BeginTabActive = .T.
      AAdd(_HMG_ActiveTabCurrentPageMap, Controlhandle)
   ENDIF

   IF tooltip != NIL
      hmg_SetToolTip(ControlHandle, TOOLTIP, GetFormToolTipHandle(cParentForm))
   ENDIF

   hmg_RichEditBox_SetRTFTextMode(ControlHandle, .T.)
   hmg_RichEditBox_SetAutoURLDetect(ControlHandle, .T.)

   k := _GetControlFree()

#ifdef _NAMES_LIST_
   _SetNameList(mVar, k)
#else
   Public &mVar. := k
#endif

   _HMG_aControlType               [k] := CONTROL_TYPE_RICHEDIT
   _HMG_aControlNames              [k] := ControlName
   _HMG_aControlHandles            [k] := ControlHandle
   _HMG_aControlParenthandles      [k] := ParentForm
   _HMG_aControlIds                [k] := 0
   _HMG_aControlProcedures         [k] := ""
   _HMG_aControlPageMap            [k] := field
   _HMG_aControlValue              [k] := NIL
   _HMG_aControlInputMask          [k] := ""
   _HMG_aControllostFocusProcedure [k] := lostfocus
   _HMG_aControlGotFocusProcedure  [k] := gotfocus
   _HMG_aControlChangeProcedure    [k] := change
   _HMG_aControlDeleted            [k] := .F.
   _HMG_aControlBkColor            [k] := backcolor
   _HMG_aControlFontColor          [k] := NIL
   _HMG_aControlDblClick           [k] := ""
   _HMG_aControlHeadClick          [k] := {}
   _HMG_aControlRow                [k] := y
   _HMG_aControlCol                [k] := x
   _HMG_aControlWidth              [k] := w
   _HMG_aControlHeight             [k] := h
   _HMG_aControlSpacing            [k] := selectionchange
   _HMG_aControlContainerRow       [k] := iif(_HMG_FrameLevel > 0, _HMG_ActiveFrameRow[_HMG_FrameLevel], -1)
   _HMG_aControlContainerCol       [k] := iif(_HMG_FrameLevel > 0, _HMG_ActiveFrameCol[_HMG_FrameLevel], -1)
   _HMG_aControlPicture            [k] := ""
   _HMG_aControlContainerHandle    [k] := ContainerHandle
   _HMG_aControlFontName           [k] := fontname
   _HMG_aControlFontSize           [k] := fontsize
   _HMG_aControlFontAttributes     [k] := {bold, italic, underline, strikeout}
   _HMG_aControlToolTip            [k] := tooltip
   _HMG_aControlRangeMin           [k] := OnLink
   _HMG_aControlRangeMax           [k] := OnVScroll
   _HMG_aControlCaption            [k] := ""
   _HMG_aControlVisible            [k] := iif(invisible, .F., .T.)
   _HMG_aControlHelpId             [k] := HelpId
   _HMG_aControlFontHandle         [k] := FontHandle
   _HMG_aControlBrushHandle        [k] := HMG_NULLHANDLE
   _HMG_aControlEnabled            [k] := .T.
   _HMG_aControlMiscData1          [k] := 1
   _HMG_aControlMiscData2          [k] := ""

   IF Field != NIL
      AAdd(_HMG_aFormBrowseList[GetFormIndex(cParentForm)], k)
   ENDIF

   IF IsArrayRGB(backcolor)
      hmg_SendMessage(_HMG_aControlHandles[k], EM_SETBKGNDCOLOR, 0, RGB(backcolor[1], backcolor[2], backcolor[3]))
   ENDIF

   IF _SetGetGlobal("_HMG_aRichEditMenu") == NIL
      STATIC _HMG_aRichEditMenu AS GLOBAL VALUE {"&Undo", "Cu&t", "&Copy", "&Paste", "&Delete", "Select &All"}
   ENDIF

   aRichEditMenu := _SetGetGlobal("_HMG_aRichEditMenu")

   _DefineControlContextMenu(ControlName, cParentForm)
      _DefineMenuItem(aRichEditMenu[1] , {||RichEditBox_mnuEdit_Click("UNDO")} , "mnuEditUndo" , , .F. , .F. ,,,, .F., .F. ,, .F.)
      _DefineSeparator()
      _DefineMenuItem(aRichEditMenu[2] , {||RichEditBox_mnuEdit_Click("CUT")} , "mnuEditCut" , , .F. , .F. ,,,, .F., .F. ,, .F.)
      _DefineMenuItem(aRichEditMenu[3] , {||RichEditBox_mnuEdit_Click("COPY")} , "mnuEditCopy" , , .F. , .F. ,,,, .F., .F. ,, .F.)
      _DefineMenuItem(aRichEditMenu[4] , {||RichEditBox_mnuEdit_Click("PASTE")} , "mnuEditPaste" , , .F. , .F. ,,,, .F., .F. ,, .F.)
      _DefineMenuItem(aRichEditMenu[5] , {||RichEditBox_mnuEdit_Click("DEL")} , "mnuEditDelete" , , .F. , .F. ,,,, .F., .F. ,, .F.)
      _DefineSeparator()
      _DefineMenuItem(aRichEditMenu[6] , {||RichEditBox_mnuEdit_Click("SELALL")} , "mnuEditSelAll" , , .F. , .F. ,,,, .F., .F. ,, .F.)
   _EndMenu()

RETURN NIL

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_mnuEdit_Click(cAction)
//---------------------------------------------------------------------------//

   LOCAL hEdit

   hEdit := GetControlHandleByIndex(_HMG_xControlsContextMenuID)

   SWITCH cAction
   CASE "UNDO"   ; hmg_RichEditBox_ChangeUndo(hEdit) ; EXIT  // Ctrl+Z
   CASE "CUT"    ; hmg_RichEditBox_SelCut(hEdit)     ; EXIT  // Ctrl+X
   CASE "COPY"   ; hmg_RichEditBox_SelCopy(hEdit)    ; EXIT  // Ctrl+C
   CASE "PASTE"  ; hmg_RichEditBox_SelPaste(hEdit)   ; EXIT  // Ctrl+V
   CASE "DEL"    ; hmg_RichEditBox_SelClear(hEdit)   ; EXIT  // Del
   CASE "SELALL" ; hmg_SendMessage(hEdit, EM_SETSEL, 0, -1)  // Ctrl+A
   ENDSWITCH

RETURN NIL

*******************************************************************************
* by Dr. Claudio Soto, January 2014
*******************************************************************************

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_SetCaretPos(hWndControl, nPos)
//---------------------------------------------------------------------------//

   LOCAL aSelRange := {nPos, nPos}

   hmg_RichEditBox_SetSelRange(hWndControl, aSelRange)

RETURN NIL

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_GetCaretPos(hWndControl)
//---------------------------------------------------------------------------//
   
   LOCAL aSelRange := hmg_RichEditBox_GetSelRange(hWndControl)

RETURN aSelRange[2]

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_SelectAll(hWndControl)
//---------------------------------------------------------------------------//
   
   LOCAL aSelRange := {0, -1}

   hmg_RichEditBox_SetSelRange(hWndControl, aSelRange)

RETURN NIL

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_UnSelectAll(hWndControl)
//---------------------------------------------------------------------------//
   
   LOCAL nPos := RichEditBox_GetCaretPos(hWndControl)

   RichEditBox_SetCaretPos(hWndControl, nPos)

RETURN NIL

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_ReplaceText(hWndControl, cFind, cReplace, lMatchCase, lWholeWord, lSelectFindText)
//---------------------------------------------------------------------------//
   
   LOCAL lDown := .T.
   LOCAL aPos

   aPos := hmg_RichEditBox_GetSelRange(hWndControl)
   hmg_RichEditBox_SetSelRange(hWndControl, {aPos[1], aPos[1]})
   aPos := hmg_RichEditBox_FindText(hWndControl, cFind, lDown, lMatchCase, lWholeWord, lSelectFindText)
   IF aPos[1] != -1
      hmg_RichEditBox_SetSelRange(hWndControl, aPos)
      hmg_RichEditBox_SetText(hWndControl, .T., cReplace)
      aPos := hmg_RichEditBox_FindText(hWndControl, cFind, lDown, lMatchCase, lWholeWord, lSelectFindText)
   ENDIF

RETURN aPos

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_ReplaceAllText(hWndControl, cFind, cReplace, lMatchCase, lWholeWord, lSelectFindText)
//---------------------------------------------------------------------------//
   
   LOCAL aPos := {0, 0}

   DO WHILE aPos[1] != -1
      aPos := RichEditBox_ReplaceText(hWndControl, cFind, cReplace, lMatchCase, lWholeWord, lSelectFindText)
      DO EVENTS
   ENDDO

RETURN aPos

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_AddTextAndSelect(hWndControl, nPos, cText)
//---------------------------------------------------------------------------//
   
   LOCAL StartCaretPos
   LOCAL EndCaretPos
   LOCAL DeltaCaretPos

   RichEditBox_SetCaretPos(hWndControl, -1)
   StartCaretPos := RichEditBox_GetCaretPos(hWndControl)

   hmg_RichEditBox_SetText(hWndControl, .T., cText)
   RichEditBox_SetCaretPos(hWndControl, -1)
   EndCaretPos := RichEditBox_GetCaretPos(hWndControl)

   hmg_RichEditBox_SetSelRange(hWndControl, {StartCaretPos, EndCaretPos})

   IF nPos <= -1 .OR. nPos > EndCaretPos
      hmg_RichEditBox_SetSelRange(hWndControl, {StartCaretPos, -1})
   ELSE
      DeltaCaretPos := EndCaretPos - StartCaretPos

      hmg_RichEditBox_SelClear(hWndControl)

      RichEditBox_SetCaretPos(hWndControl, nPos)
      hmg_RichEditBox_SetText(hWndControl, .T., cText)
      hmg_RichEditBox_SetSelRange(hWndControl, {nPos, nPos + DeltaCaretPos})
   ENDIF

RETURN NIL

#define TWIPS     1440 / 25.4

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_RTFPrint(hWndControl, aSelRange, nLeft, nTop, nRight, nBottom, PrintPageCodeBlock)
//---------------------------------------------------------------------------//
   
   LOCAL nPageWidth
   LOCAL nPageHeight
   LOCAL nNextChar := 0
   LOCAL nTextLength := hmg_RichEditBox_GetTextLength(hWndControl)

   DEFAULT aSelRange TO {0, -1}   // select all text
   DEFAULT nLeft TO 20            // Left   page margin in millimeters
   DEFAULT nTop TO 20             // Top    page margin in millimeters
   DEFAULT nRight TO 20           // Right  page margin in millimeters
   DEFAULT nBottom TO 20          // Bottom page margin in millimeters
   DEFAULT PrintPageCodeBlock TO {||NIL}

   nPageWidth := OpenPrinterGetPageWidth() // in millimeters
   nPageHeight := OpenPrinterGetPageHeight() // in millimeters

   nRight := nPageWidth - nRight
   nBottom := nPageHeight - nBottom

   // Convert millimeters in twips ( 1 inch = 25.4 mm = 1440 twips )
   nLeft *= TWIPS
   nTop *= TWIPS
   nRight *= TWIPS
   nBottom *= TWIPS

   IF aSelRange[2] == -1 .OR. aSelRange[2] > nTextLength
      aSelRange[2] := nTextLength
   ENDIF

   START PRINTDOC

   DO WHILE nNextChar < nTextLength

      START PRINTPAGE

      Eval(PrintPageCodeBlock)
      nNextChar := hmg_RichEditBox_FormatRange(hWndControl, OpenPrinterGetPageDC(), nLeft, nTop, nRight, nBottom, aSelRange)
      aSelRange[1] := nNextChar
      DO EVENTS

      END PRINTPAGE

   ENDDO

   END PRINTDOC

RETURN NIL

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_LoadFile(hWndControl, cFile, lSelection, nType)
//---------------------------------------------------------------------------//
   
   LOCAL lSuccess

   hb_default(@lSelection, .F.)
   hb_default(@nType, RICHEDITFILE_RTF)

   lSuccess := hmg_RichEditBox_RTFLoadResourceFile(hWndControl, cFile, lSelection)

   IF !lSuccess
      lSuccess := hmg_RichEditBox_StreamIn(hWndControl, cFile, lSelection, nType)
   ENDIF

RETURN lSuccess

//---------------------------------------------------------------------------//
FUNCTION RichEditBox_SaveFile(hWndControl, cFile, lSelection, nType)
//---------------------------------------------------------------------------//

   hb_default(@lSelection, .F.)
   hb_default(@nType, RICHEDITFILE_RTF)

   hmg_RichEditBox_StreamOut(hWndControl, cFile, lSelection, nType)

RETURN NIL

#endif
