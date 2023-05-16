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

#include "minigui.ch"

SET PROCEDURE TO h_cdomail.prg

*-----------------------------------------------------------------------------*
FUNCTION GetData()
*-----------------------------------------------------------------------------*

   LOCAL PacketNames [ aDir ( _HMG_CommPath + _HMG_StationName + ".*" ) ]
   LOCAL i
   LOCAL Rows
   LOCAL Cols
   LOCAL RetVal := Nil
   LOCAL aItem
   LOCAL aTemp := {}
   LOCAL r
   LOCAL c
   LOCAL DataValue
   LOCAL DataType
   LOCAL DataLength
   LOCAL Packet
   LOCAL bd := Set ( _SET_DATEFORMAT )

   SET DATE TO ANSI

   ADir ( _HMG_CommPath + _HMG_StationName + ".*" , PacketNames )

   IF Len(PacketNames) > 0

      Packet := MemoRead(_HMG_CommPath + PacketNames[1])

      Rows := Val(SubStr(MemoLine( Packet , , 1 ) , 11 , 99))
      Cols := Val(SubStr(MemoLine( Packet , , 2 ) , 11 , 99))

      DO CASE

      // Single Data
      CASE Rows == 0 .AND. Cols == 0

         DataType := SubStr(MemoLine( Packet ,  , 3 ) , 12 , 1)
         DataLength := Val(SubStr(MemoLine( Packet , , 3 ) , 14 , 99))

         DataValue := MemoLine( Packet , 254 , 4 )

         DO CASE
         CASE DataType == "C"
            RetVal := Left(DataValue, DataLength)
         CASE DataType == "N"
            RetVal := Val(DataValue)
         CASE DataType == "D"
            RetVal := CToD(DataValue)
         CASE DataType == "L"
            RetVal := ( AllTrim(DataValue) == "T" )
         END CASE

      // One Dimension Array Data
      CASE Rows != 0 .AND. Cols == 0

         i := 3

         DO WHILE i < MLCount ( Packet )

            DataType   := SubStr(MemoLine(Packet, , i), 12, 1)
            DataLength := Val(SubStr(MemoLine(Packet, , i), 14, 99))

            i++

            DataValue  := MemoLine( Packet , 254 , i )

            DO CASE
            CASE DataType == "C"
               aItem := Left(DataValue, DataLength)
            CASE DataType == "N"
               aItem := Val(DataValue)
            CASE DataType == "D"
               aItem := CToD(DataValue)
            CASE DataType == "L"
               aItem := ( AllTrim(DataValue) == "T" )
            END CASE

            AAdd(aTemp, aItem)

            i++

         ENDDO

         RetVal := aTemp

      // Two Dimension Array Data
      CASE Rows != 0 .AND. Cols != 0

         i := 3

         aTemp := Array(Rows , Cols)

         r := 1
         c := 1

         DO WHILE i < MLCount ( Packet )

            DataType   := SubStr(MemoLine(Packet, , i) , 12, 1)
            DataLength := Val(SubStr(MemoLine(Packet, , i), 14, 99))

            i++

            DataValue  := MemoLine( Packet , 254 , i )

            DO CASE
            CASE DataType == "C"
               aItem := Left(DataValue, DataLength)
            CASE DataType == "N"
               aItem := Val(DataValue)
            CASE DataType == "D"
               aItem := CToD(DataValue)
            CASE DataType == "L"
               aItem := ( AllTrim(DataValue) == "T" )
            END CASE

            aTemp [r] [c] := aItem

            c++
            IF c > Cols
               r++
               c := 1
            ENDIF

            i++

         ENDDO

         RetVal := aTemp

      END CASE

      DELETE File ( _HMG_CommPath + PacketNames [1] )

   ENDIF

   SET ( _SET_DATEFORMAT , bd )

RETURN ( RetVal )

*-----------------------------------------------------------------------------*
FUNCTION SendData ( cDest , Data )
*-----------------------------------------------------------------------------*
   
   LOCAL cData
   LOCAL i
   LOCAL j
   LOCAL pData
   LOCAL cLen
   LOCAL cType
   LOCAL FileName
   LOCAL Rows
   LOCAL Cols

   FileName := _HMG_CommPath + cDest + "." + _HMG_StationName + "." + hb_ntos ( ++_HMG_SendDataCount )

   IF hb_IsArray(Data)

      IF ValType(Data [1]) != "A"

         cData := "#DataRows=" + hb_ntos( Len(Data ) ) + Chr( 13 ) + Chr( 10 )
         cData += "#DataCols=0" + Chr( 13 ) + Chr( 10 )

         FOR i := 1 TO Len(Data)

            cType := ValType(Data[i])

            IF cType == "D"
               pData := hb_ntos( Year( data[i] ) ) + "." + hb_ntos( Month( data[i] ) ) + "." + hb_ntos( Day( data[i] ) )
               cLen := hb_ntos( Len(pData) )
            ELSEIF cType == "L"
               pData := iif( Data[i] == .T. , "T", "F" )
               cLen := hb_ntos( Len(pData) )
            ELSEIF cType == "N"
               pData := Str(Data[i])
               cLen := hb_ntos( Len(pData) )
            ELSEIF cType == "C"
               pData := Data[i]
               cLen := hb_ntos( Len(pData) )
            ELSE
               MsgMiniGuiError("SendData: Type Not Supported.")
            ENDIF

            cData += "#DataBlock=" + cType + "," + cLen + Chr( 13 ) + Chr( 10 )
            cData += pData + Chr( 13 ) + Chr( 10 )

         NEXT i

         MemoWrit ( FileName , cData )

      ELSE

         Rows := Len(Data)
         Cols := Len(Data[1])

         cData := "#DataRows=" + hb_ntos( Rows ) + Chr( 13 ) + Chr( 10 )
         cData += "#DataCols=" + hb_ntos( Cols ) + Chr( 13 ) + Chr( 10 )

         FOR i := 1 TO Rows

            FOR j := 1 TO Cols

               cType := ValType(Data[i] [j])

               IF cType == "D"
                  pData := hb_ntos( Year( data[i][j] ) ) + "." + hb_ntos( Month( data[i][j] ) ) + "." + hb_ntos( Day( data[i][j] ) )
                  cLen := hb_ntos( Len(pData) )
               ELSEIF cType == "L"
                  pData := iif( Data[i] [j] == .T. , "T", "F" )
                  cLen := hb_ntos( Len(pData) )
               ELSEIF cType == "N"
                  pData := Str(Data[i][j])
                  cLen := hb_ntos( Len(pData) )
               ELSEIF cType == "C"
                  pData := Data[i] [j]
                  cLen := hb_ntos( Len(pData) )
               ELSE
                  MsgMiniGuiError("SendData: Type Not Supported.")
               ENDIF

               cData += "#DataBlock=" + cType + "," + cLen + Chr( 13 ) + Chr( 10 )
               cData += pData + Chr( 13 ) + Chr( 10 )

            NEXT j
         NEXT i

         MemoWrit ( FileName , cData )

      ENDIF

   ELSE

      cType := ValType(Data)

      IF cType == "D"
         pData := hb_ntos( Year( data ) ) + "." + hb_ntos( Month( data ) ) + "." + hb_ntos( Day( data ) )
         cLen := hb_ntos( Len(pData) )
      ELSEIF cType == "L"
         pData := iif( Data == .T. , "T", "F" )
         cLen := hb_ntos( Len(pData) )
      ELSEIF cType == "N"
         pData := Str(Data)
         cLen := hb_ntos( Len(pData) )
      ELSEIF cType == "C"
         pData := Data
         cLen := hb_ntos( Len(pData) )
      ELSE
         MsgMiniGuiError("SendData: Type Not Supported.")
      ENDIF

      cData := "#DataRows=0" + Chr( 13 ) + Chr( 10 )
      cData += "#DataCols=0" + Chr( 13 ) + Chr( 10 )

      cData += "#DataBlock=" + cType + "," + cLen + Chr( 13 ) + Chr( 10 )
      cData += pData + Chr( 13 ) + Chr( 10 )

      MemoWrit ( FileName , cData )

   ENDIF

RETURN Nil

*-----------------------------------------------------------------------------*
FUNCTION HMG_ClrToHTML( nClr )
*-----------------------------------------------------------------------------*
   
   LOCAL cHex := Lower( hb_NumToHex( nClr, 6 ) )

RETURN "#" + Right(cHex, 2) + SubStr(cHex, 3, 2) + Left(cHex, 2)

#include "fileio.ch"

#define F_BLOCK   8192

*-----------------------------------------------------------------------------*
FUNCTION HMG_FILECOPY( cSourceFile, cTargetFile, nBuffer, bEval )
*-----------------------------------------------------------------------------*
   
   LOCAL hSourceFile
   LOCAL hTargetFile
   LOCAL cBuffer
   LOCAL nTotalBytes
   LOCAL nCurrentlBytes
   LOCAL nReadBytes
   LOCAL lShowProgress := ( HB_ISBLOCK(bEval) )
   LOCAL lSuccess := .F.

   DEFAULT nBuffer TO F_BLOCK

   IF ( hSourceFile := FOpen(cSourceFile, FO_READ) ) != F_ERROR

      IF ( hTargetFile := FCreate( cTargetFile, FC_NORMAL ) ) != F_ERROR

         nTotalBytes := FSeek(hSourceFile, 0, FS_END)
         nCurrentlBytes := 0

         FSeek(hSourceFile, 0, FS_SET)

         DO WHILE nCurrentlBytes < nTotalBytes

            cBuffer := Space( nBuffer )
            nCurrentlBytes += ( nReadBytes := FRead(hSourceFile, @cBuffer, nBuffer) )
            FWrite(hTargetFile, cBuffer, nReadBytes)

            IF lShowProgress

               Eval(bEval, nCurrentlBytes / nTotalBytes)

            ENDIF

         ENDDO

         lSuccess := FClose(hTargetFile)

      ENDIF

      FClose(hSourceFile)

   ENDIF

RETURN lSuccess

*-----------------------------------------------------------------------------*
FUNCTION uCharToVal(cText, cType)
*-----------------------------------------------------------------------------*
   
   LOCAL uVal
   LOCAL cTrue := "|.T.|T|TRUE|YES|SI|"
   LOCAL cFalse := "|.F.|F|FALSE|NO|"

   IF HB_ISCHAR(cType) .AND. Len(cType) == 1 .AND. ( cType := Upper(cType) ) $ "CDLMN"

      IF cType == "M"
         cType := "C"
      ENDIF

   ELSE

      cType := ValType(cType)

   ENDIF

   IF cType == "T"
      cType := "D"
   ENDIF

   IF HB_ISCHAR(cText)

      cText := AllTrim(cText)

      DO CASE

      CASE cType == "C"

         uVal := cText

      CASE cType == "N"

         uVal := IfNil( nStrToNum( cText, , .T. ), Val(cText) )

      CASE cType == "L"

         uVal := ( "|" + Upper(cText) + "|" $ cTrue )

      CASE cType == "D"

         uVal := dCharToDate( cText )

      OTHERWISE

         IF ( uVal := nStrToNum( cText ) ) != NIL

            cType := "N"

         ELSEIF "|" + Upper(cText) + "|" $ cTrue

            uVal := .T.
            cType := "L"

         ELSEIF "|" + Upper(cText) + "|" $ cFalse

            uVal := .F.
            cType := "L"

         ELSE

            uVal := dCharToDate( cText )

            IF Empty(uVal)

               uVal := cText
               cType := "C"

            ELSE

               cType := "D"

            ENDIF

         ENDIF

      ENDCASE

   ELSE

      uVal := cText
      cType := ValType(uVal)

   ENDIF

RETURN uVal

*-----------------------------------------------------------------------------*
FUNCTION nStrToNum( cNum, lEuropean, lForceNumeric )
*-----------------------------------------------------------------------------*
   
   LOCAL nVal // := NIL
   LOCAL cMinus := ""
   LOCAL lPercent := .F.
   LOCAL nCommaAt
   LOCAL nDotAt

   cNum := AllTrim(cNum)
   IF Left(cNum, 1) == "+"
      cNum := LTrim(SubStr(cNum, 2))
   ENDIF

   IF Left(cNum, 1) == "-"
      cMinus := "-"
      cNum := LTrim(SubStr(cNum, 2))
   ENDIF

   IF Right(cNum, 1) == "%"
      lPercent := .T.
      cNum := Trim(Left(cNum, Len(cNum) - 1))
   ENDIF

   IF lForceNumeric == .T. .AND. " " $ cNum
      cNum := BeforAtNum( " ", cNum, 1 )
   ENDIF

   IF CharOnly( "0123456789,.", cNum ) == cNum

      // valid number string
      nCommaAt := RAt( ",", cNum )
      nDotAt := RAt( ".", cNum )
      IF nCommaAt == 0 .AND. nDotAt == 0
         RETURN Val(cMinus + cNum)
      ENDIF

      IF Occurs( ",", cNum ) > 1

         // American format
         lEuropean := .F.

      ELSEIF Occurs( ".", cNum ) > 1

         lEuropean := .T.

      ELSEIF nCommaAt > 0 .AND. nDotAt > nCommaAt

         // American format
         lEuropean := .F.

      ELSEIF nDotAt > 0 .AND. nCommaAt > nDotAt

         // European Format
         lEuropean := .T.

      ELSEIF nDotAt > 0 .AND. nDotAt != Len(cNum) - 3

         lEuropean := .F.

      ELSEIF nCommaAt > 0 .AND. nCommaAt != Len(cNum) - 3

         lEuropean := .T.

      ELSE

         DEFAULT lEuropean := .F.

      ENDIF

      IF lEuropean

         nVal := Val(cMinus + CharRepl( ",", CharRem( ".", cNum ), "." ))

      ELSE

         nVal := Val(cMinus + CharRem( ",", cNum ))

      ENDIF

      IF lPercent
         nVal *= 0.01
      ENDIF

   ENDIF

RETURN nVal

*-----------------------------------------------------------------------------*
STATIC FUNCTION IfNil( ... )
*-----------------------------------------------------------------------------*
   
   LOCAL aParams := hb_AParams()
   LOCAL u

   IF Len(aParams) == 1 .AND. hb_IsArray(aParams[1])
      aParams := aParams[1]
   ENDIF

   FOR EACH u IN aParams

      IF u != NIL
         RETURN u
      ENDIF

   NEXT

RETURN u

*-----------------------------------------------------------------------------*
STATIC FUNCTION dCharToDate( cDate )
*-----------------------------------------------------------------------------*
   
   LOCAL cFormat
   LOCAL cc
   LOCAL dDate

   IF ( cc := Upper(cDate) ) != Lower( cDate )
      RETURN dAlphaToDate( cc )
   ENDIF

   IF Len(cDate) >= 8 .AND. !Empty(dDate := SToD(Left(cDate, 8)))
      RETURN dDate
   ENDIF

   IF Len(cDate) >= 10 .AND. !Empty(dDate := SToD(Left(cDate, 4) + SubStr(cDate, 6, 2) + SubStr(cDate, 9, 2)))
      RETURN dDate
   ENDIF

   cFormat := Set( _SET_DATEFORMAT )

   dDate := CToD(cDate)

   IF Empty(dDate)

      cc := Lower( Left(cFormat, 2) )
      Set( _SET_DATEFORMAT, iif( cc == "dd", "mm/dd/yy", "dd/mm/yy" ) )
      dDate := CToD(cDate)
      IF cc == "yy" .AND. Empty(dDate)
         SET DATE AMERICAN
         dDate := CToD(cDate)
      ENDIF

   ENDIF

   Set( _SET_DATEFORMAT, cFormat )

RETURN dDate

*-----------------------------------------------------------------------------*
STATIC FUNCTION dAlphaToDate( cDate )
*-----------------------------------------------------------------------------*
   
   LOCAL dDate := BLANK_DATE
   LOCAL m
   LOCAL n
   LOCAL nEpoch
   LOCAL aMonths := Array(12)
   LOCAL aNum

   FOR n := 1 TO 12

      aMonths[n] := Upper(Left(cMonth(StoD(Str(Year(Date()), 4) + StrZero( n, 2 ) + "01" ) ), 3 ) )

      IF aMonths[n] $ cDate

         m := n
         EXIT

      ENDIF

   NEXT n

   IF !Empty(m)

      aNum := ParseNumsFromDateStr(cDate)

      IF Empty(aNum[2])

         aNum[2] := Year( Date() )

      ELSE

         IF aNum[2] < 100

            nEpoch := Set( _SET_EPOCH )
            aNum[2] += 1900

            IF aNum[2] < nEpoch
               aNum[2] += 100
            ENDIF

         ENDIF

      ENDIF

      dDate := SToD( StrZero( aNum[2], 4 ) + StrZero( m, 2 ) + StrZero( aNum[1], 2 ) )

   ENDIF

RETURN dDate

*-----------------------------------------------------------------------------*
STATIC FUNCTION ParseNumsFromDateStr(cStr)
*-----------------------------------------------------------------------------*
   
   LOCAL aNum := {}
   LOCAL cNum := ""
   LOCAL c

   FOR EACH c IN cStr

      IF IsDigit( c )

         cNum += c

      ELSE

         IF c == ":" .AND. Len(aNum) < 2

            ASize(aNum, 2)

         ENDIF

         IF !Empty(cNum)

            AAdd(aNum, cNum)
            cNum := ""

         ENDIF

      ENDIF

   NEXT c

   IF !Empty(cNum)

      AAdd(aNum, cNum)

   ENDIF

   IF Len(aNum) < 2

      ASize(aNum, 2)

   ENDIF

   AEval(aNum, {| c, i | aNum[i] := iif( c == NIL, 0, Val(c) ) })

   IF aNum[1] > 31

      c := aNum[1]
      aNum[1] := aNum[2]
      aNum[2] := c

   ENDIF

RETURN aNum
