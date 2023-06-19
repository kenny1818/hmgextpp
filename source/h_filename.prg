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

*-----------------------------------------------------------------------------*
FUNCTION cFilePath(cPathMask)
*-----------------------------------------------------------------------------*

   LOCAL cPath

   hb_FNameSplit(cPathMask, @cPath)

RETURN hb_StrShrink( cPath )

*-----------------------------------------------------------------------------*
FUNCTION cFileNoPath(cPathMask)
*-----------------------------------------------------------------------------*

   LOCAL cName
   LOCAL cExt

   hb_FNameSplit(cPathMask, , @cName, @cExt)

RETURN ( cName + cExt )

*-----------------------------------------------------------------------------*
FUNCTION cFileNoExt(cPathMask)
*-----------------------------------------------------------------------------*

   LOCAL cName

   hb_FNameSplit(cPathMask, , @cName)

RETURN cName

// Jacek Kubica <kubica@wssk.wroc.pl> HMG 1.1 Experimental Build 11a
// cFile - string to be compacted (may be for example fullpath, path or file name)
// nMax  - required string size (characters count)
// _GetCompactPath("C:\Program Files\Adobe", 20) -> "C:\Program...\Adobe"
*-----------------------------------------------------------------------------*
FUNCTION _GetCompactPath(cFile, nMax)
*-----------------------------------------------------------------------------*

   LOCAL cShort := Space(iif(hb_IsNumeric(nMax), nMax + 1, 64))

RETURN iif(GetCompactPath(@cShort, cFile, iif(hb_IsNumeric(nMax), nMax, 63), NIL) > 0, cShort, cFile)

// Jacek Kubica <kubica@wssk.wroc.pl> HMG 1.1 Experimental Build 11a
// _GetShortPathName("C:\Program Files\Adobe") -> "C:\Program~1\Adobe"
*-----------------------------------------------------------------------------*
FUNCTION _GetShortPathName(cPath)
*-----------------------------------------------------------------------------*
   
   LOCAL cShortPathName

RETURN iif(GetShortPathName(cPath, @cShortPathName) > 0, cShortPathName, cPath)
