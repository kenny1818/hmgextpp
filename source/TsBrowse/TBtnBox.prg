#include "minigui.ch"
#include "hbclass.ch"
#include "TSBrowse.ch"

#define EN_CHANGE      768    // 0x0300
#define EN_UPDATE      1024   // 0x0400
#define ES_NUMBER      8192
#define NM_KILLFOCUS   (-8)

* ============================================================================
* CLASS TBtnBox  Driver for BtnBox  TSBrowse 7.0
* ============================================================================

CLASS TBtnBox FROM TControl

   CLASSDATA lRegistered AS LOGICAL

   DATA Atx 
   DATA lAppend
   DATA bAction
   DATA nCell
   DATA lChanged
   DATA hWndChild

   METHOD New( nRow, nCol, bSetGet, oWnd, nWidth, nHeight, cPict, ;
               nClrFore, nClrBack, hFont, cControl, cWnd, cMsg, bChanged, bValid,;
               cResName, bAction, lSpinner, bUp, bDown, bMin, bMax, nBmpWidth, nCell )
   METHOD Default()
   METHOD HandleEvent( nMsg, nWParam, nLParam )
   METHOD GetDlgCode(nLastKey, nFlags)
   Method KeyChar( nKey, nFlags )
   Method KeyDown( nKey, nFlags )
   Method LostFocus( hCtlFocus )
   Method lValid()
   METHOD LButtonDown( nRow, nCol )
   METHOD GetVal()
   METHOD Command(nWParam, nLParam)

ENDCLASS

* ============================================================================
* METHOD TBtnBox:New() Version 7.0
* ============================================================================


METHOD New( nRow, nCol, bSetGet, oWnd, nWidth, nHeight, cPict, ;
            nClrFore, nClrBack, hFont, cControl, cWnd, cMsg, bChanged, bValid,;
            cResName, bAction, lSpinner, bUp, bDown, bMin, bMax, nBmpWidth, nCell ) CLASS TBtnBox

   LOCAL invisible    := .F.
   LOCAL notabstop    := .F.
   LOCAL ParentHandle
   LOCAL nMaxLenght   := 255
   LOCAL nMin 
   LOCAL nMax

   HB_SYMBOL_UNUSED(cPict)
   HB_SYMBOL_UNUSED(bChanged)
   HB_SYMBOL_UNUSED(bDown)

   DEFAULT nClrFore  := GetSysColor( COLOR_WINDOWTEXT ), ;
           nClrBack  := GetSysColor( COLOR_WINDOW ), ;
           nHeight   := 12, ;
           bMin      := {|| 0 }, ;
           bMax      := {|| 32000 }

   ::nTop         := nRow
   ::nLeft        := nCol
   ::nBottom      := ::nTop  + nHeight - 2
   ::nRight       := ::nLeft + nWidth  - 2
   ::oWnd         := oWnd
   ParentHandle   := oWnd:hWnd

   IF _HMG_BeginWindowMDIActive
      ParentHandle := GetActiveMdiHandle()
      cWnd         := _GetWindowProperty( ParentHandle, "PROP_FORMNAME" )
   endif

   ::nId          := ::GetNewId()
   ::nStyle       := nOR( ES_NUMBER , WS_CHILD )
   ::cControlName := cControl
   ::cParentWnd   := cWnd
   ::hWndParent   := oWnd:hWnd
   ::bSetGet      := bSetGet
   ::lCaptured    := .F.
   ::hFont        := hFont
   ::lFocused     := .F.
   ::lAppend      := .F.
   ::nLastKey     := 0
   ::lChanged     := .F.

   ::cMsg         := cMsg
   ::bChange      := .T.
   ::bValid       := bValid
   ::bAction      := bAction
   ::nCell        := nCell
   ::Atx          := 0

   ::SetColor( nClrFore, nClrBack )

   if !Empty(ParentHandle)
      if lSpinner
         ::Create("EDIT")
         nMin := IIf(hb_IsBlock(bMin), Eval(bMin), bMin)
         nMax := IIf(hb_IsBlock(bMax), Eval(bMax), bMax)
         ::hWndChild := InitedSpinner( ::hWndParent, ::hWnd , nCol, nRow, 0, nHeight, nMin, nMax, Eval(::bSetGet) )
         SetIncrementSpinner( ::hWndChild, bUp )
      else
         ::hWnd := InitBtnTextBox( ParentHandle, 0, nCol, nRow, nWidth, nHeight, "", 0, nMaxLenght, ;
           .F., .F., .F., .F.,.F., invisible, notabstop, cResName, nBmpWidth, "", .F. )[1]
      endif

      ::AddVars( ::hWnd )
      ::Default()

      if GetObjectType(hFont) == OBJ_FONT
         _SetFontHandle(::hWnd, hFont)
         ::hFont := hFont
      endif
      oWnd:AddControl( ::hWnd )
   endif

Return Self

* ============================================================================
* METHOD TBtnBox:Default() Version 7.0
* ============================================================================

METHOD Default() CLASS TBtnBox

   LOCAL cValue
 
   cValue := Eval(::bSetGet)
   If Valtype(cValue) != "C"
      cValue := cValToChar( cValue )
   EndIf

   if Len(cValue) > 0
      SetWindowText(::hWnd, cValue)
   endif

Return NIL

* ============================================================================
* METHOD TBtnBox:HandleEvent() Version 7.0
* ============================================================================

METHOD HandleEvent( nMsg, nWParam, nLParam ) CLASS TBtnBox

   // just used for some testings
   If nMsg == WM_NOTIFY
      IF HiWord(nWParam) == NM_KILLFOCUS
         ::LostFocus()
      Endif
   EndIf

Return ::Super:HandleEvent( nMsg, nWParam, nLParam )

* ============================================================================
* METHOD TBtnBox:GetDlgCode() Version 7.0
* ============================================================================

METHOD GetDlgCode(nLastKey, nFlags) CLASS TBtnBox

   HB_SYMBOL_UNUSED(nFlags)
   ::nLastKey := nLastKey

Return DLGC_WANTALLKEYS + DLGC_WANTCHARS


* ============================================================================
* METHOD TBtnBox:KeyChar() Version 7.0
* ============================================================================

METHOD KeyChar( nKey, nFlags ) CLASS TBtnBox

   If _GetKeyState(VK_CONTROL)
      nKey := IIf(Upper(Chr(nKey)) == "W" .OR. nKey == VK_RETURN, VK_TAB, nKey)
   EndIf

   If nKey == VK_TAB .OR. nKey == VK_ESCAPE
      Return 0
   EndIf

RETURN ::Super:KeyChar( nKey, nFlags )

* ============================================================================
* METHOD TBtnBox:KeyDown() Version 7.0
* ============================================================================

METHOD KeyDown( nKey, nFlags ) CLASS TBtnBox

   ::nLastKey := nKey
   If nKey == VK_TAB .OR. nKey == VK_RETURN .OR. nKey == VK_ESCAPE

      IF nKey != VK_ESCAPE
         If ::bSetGet != Nil
            Eval(::bSetGet, ::GetVal())
         EndIf
      ENDIF
      ::bLostFocus := Nil
      Eval(::bKeyDown, nKey, nFlags, .T.)
   EndIf

RETURN 0

* ============================================================================
* METHOD TBtnBox:lValid() Version 7.0
* ============================================================================

METHOD lValid() CLASS TBtnBox

   Local lRet := .T.

   If hb_IsBlock(::bValid)
      lRet := Eval(::bValid, ::GetVal())
   EndIf

Return lRet

* ============================================================================
* METHOD TBtnBox:LostFocus() Version 7.0
* ============================================================================

METHOD LostFocus( hCtlFocus ) CLASS TBtnBox

   DEFAULT ::lAppend := .F.

   If ::nLastKey == Nil .AND. ::lAppend
      ::SetFocus()
      ::nLastKey := 0
      Return 0
   EndIf
   ::lFocused := .F.
   If ::bLostFocus != Nil
      Eval(::bLostFocus, ::nLastKey, hCtlFocus)
   EndIf
   IF ::hWndChild != Nil
      ::SetFocus()
   endif

Return 0

* ============================================================================
* METHOD TBtnBox:LButtonDown() Version 7.0
* ============================================================================

METHOD LButtonDown( nRow, nCol ) CLASS TBtnBox

   HB_SYMBOL_UNUSED(nRow)
   HB_SYMBOL_UNUSED(nCol)

   If ::nLastKey != Nil .AND. ::nLastKey == 9999
      ::nLastKey := 0
   Else
      ::nLastKey := 9999
   EndIf

Return 0

* ============================================================================
* METHOD TBtnBox:VarGet() Version 7.0
* ============================================================================

METHOD GetVal() CLASS TBtnBox

   LOCAL retVal
   LOCAL cType

   cType := ValType(::VarGet())

   DO CASE
      CASE cType == "C" ; retVal := GetWindowText(::hWnd)
      CASE cType == "N" ; retVal := Int(Val(GetWindowText(::hWnd)))
   ENDCASE

RETURN retVal

* ============================================================================
* METHOD TBtnBox:Command() Version 7.0
* ============================================================================

METHOD Command(nWParam, nLParam) CLASS TBtnBox

   LOCAL nNotifyCode
   LOCAL nID
   LOCAL hWndCtl

   nNotifyCode := HiWord(nWParam)
   nID         := LoWord(nWParam)
   hWndCtl     := nLParam

   do case
   case hWndCtl == 0

      * Enter ........................................
      If HiWord(nWParam) == 0 .And. LoWord(nWParam) == 1
         ::KeyDown( VK_RETURN, 0 )
      EndIf

      * Escape .......................................
      If HiWord(nwParam) == 0 .And. LoWord(nwParam) == 2
         ::KeyDown( VK_ESCAPE, 0 )
      EndIf

   case hWndCtl != 0

      do case
         case nNotifyCode == 512 .And. nID == 0 .And. ::bAction != Nil
            ::oWnd:lPostEdit := .T.
            Eval(::bAction, Self, Eval(::bSetGet))
            ::bLostFocus := { | nKey | ::oWnd:EditExit( ::nCell, nKey, ::VarGet(), ::bValid, .F. ) }
            ::nLastKey := VK_RETURN
            ::LostFocus()
            ::oWnd:lPostEdit := .F.
         
         case nNotifyCode == EN_CHANGE
            ::lChanged :=.T.
         
         case nNotifyCode == EN_KILLFOCUS
            ::LostFocus()
         
         case nNotifyCode == EN_UPDATE
            If _GetKeyState(VK_ESCAPE)
               ::KeyDown( VK_ESCAPE, 0 )
            Endif
            If _GetKeyState(VK_CONTROL)
               If GetKeyState(VK_RETURN) == -127 .Or. _GetKeyState(VK_RETURN)
                  ::KeyDown( VK_RETURN, 0 )
               Endif
            Endif
      endcase

   endcase

Return nil
