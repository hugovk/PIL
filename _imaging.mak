# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=jpeg - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to jpeg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "_imaging - Win32 Release" && "$(CFG)" !=\
 "_imaging - Win32 Debug" && "$(CFG)" != "zlib - Win32 Release" && "$(CFG)" !=\
 "zlib - Win32 Debug" && "$(CFG)" != "jpeg - Win32 Release" && "$(CFG)" !=\
 "jpeg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "_imaging.mak" CFG="jpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "_imaging - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "_imaging - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "_imaging - Win32 Debug"

!IF  "$(CFG)" == "_imaging - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "zlib - Win32 Release" "jpeg - Win32 Release" ".\_imaging.dll"

CLEAN : 
	-@erase "$(INTDIR)\_imagingmodule.obj"
	-@erase "$(INTDIR)\Bands.obj"
	-@erase "$(INTDIR)\Blend.obj"
	-@erase "$(INTDIR)\Chops.obj"
	-@erase "$(INTDIR)\Convert.obj"
	-@erase "$(INTDIR)\Copy.obj"
	-@erase "$(INTDIR)\Crc32.obj"
	-@erase "$(INTDIR)\Crop.obj"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\Draw.obj"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\EpsEncode.obj"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\Fill.obj"
	-@erase "$(INTDIR)\Filter.obj"
	-@erase "$(INTDIR)\FliDecode.obj"
	-@erase "$(INTDIR)\Geometry.obj"
	-@erase "$(INTDIR)\GetBBox.obj"
	-@erase "$(INTDIR)\GifDecode.obj"
	-@erase "$(INTDIR)\GifEncode.obj"
	-@erase "$(INTDIR)\HexDecode.obj"
	-@erase "$(INTDIR)\Histo.obj"
	-@erase "$(INTDIR)\JpegDecode.obj"
	-@erase "$(INTDIR)\JpegEncode.obj"
	-@erase "$(INTDIR)\LzwDecode.obj"
	-@erase "$(INTDIR)\Matrix.obj"
	-@erase "$(INTDIR)\MspDecode.obj"
	-@erase "$(INTDIR)\Negative.obj"
	-@erase "$(INTDIR)\Offset.obj"
	-@erase "$(INTDIR)\Pack.obj"
	-@erase "$(INTDIR)\PackDecode.obj"
	-@erase "$(INTDIR)\Palette.obj"
	-@erase "$(INTDIR)\Paste.obj"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\PcdDecode.obj"
	-@erase "$(INTDIR)\PcxDecode.obj"
	-@erase "$(INTDIR)\Point.obj"
	-@erase "$(INTDIR)\Quantize.obj"
	-@erase "$(INTDIR)\RawDecode.obj"
	-@erase "$(INTDIR)\RawEncode.obj"
	-@erase "$(INTDIR)\Storage.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\SunRleDecode.obj"
	-@erase "$(INTDIR)\TgaRleDecode.obj"
	-@erase "$(INTDIR)\Unpack.obj"
	-@erase "$(INTDIR)\XbmDecode.obj"
	-@erase "$(INTDIR)\XbmEncode.obj"
	-@erase "$(INTDIR)\ZipDecode.obj"
	-@erase "$(INTDIR)\ZipEncode.obj"
	-@erase "$(OUTDIR)\_imaging.exp"
	-@erase "$(OUTDIR)\_imaging.lib"
	-@erase ".\_imaging.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "../Include" /I "../PC" /I "./libImaging" /I "./jpeg-6" /I "./zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "../Include" /I "../PC" /I\
 "./libImaging" /I "./jpeg-6" /I "./zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/_imaging.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/_imaging.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"_imaging.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/_imaging.pdb" /machine:I386 /def:".\_imaging.def"\
 /out:"_imaging.dll" /implib:"$(OUTDIR)/_imaging.lib" 
DEF_FILE= \
	".\_imaging.def"
LINK32_OBJS= \
	"$(INTDIR)\_imagingmodule.obj" \
	"$(INTDIR)\Bands.obj" \
	"$(INTDIR)\Blend.obj" \
	"$(INTDIR)\Chops.obj" \
	"$(INTDIR)\Convert.obj" \
	"$(INTDIR)\Copy.obj" \
	"$(INTDIR)\Crc32.obj" \
	"$(INTDIR)\Crop.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\Draw.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\EpsEncode.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\Fill.obj" \
	"$(INTDIR)\Filter.obj" \
	"$(INTDIR)\FliDecode.obj" \
	"$(INTDIR)\Geometry.obj" \
	"$(INTDIR)\GetBBox.obj" \
	"$(INTDIR)\GifDecode.obj" \
	"$(INTDIR)\GifEncode.obj" \
	"$(INTDIR)\HexDecode.obj" \
	"$(INTDIR)\Histo.obj" \
	"$(INTDIR)\JpegDecode.obj" \
	"$(INTDIR)\JpegEncode.obj" \
	"$(INTDIR)\LzwDecode.obj" \
	"$(INTDIR)\Matrix.obj" \
	"$(INTDIR)\MspDecode.obj" \
	"$(INTDIR)\Negative.obj" \
	"$(INTDIR)\Offset.obj" \
	"$(INTDIR)\Pack.obj" \
	"$(INTDIR)\PackDecode.obj" \
	"$(INTDIR)\Palette.obj" \
	"$(INTDIR)\Paste.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\PcdDecode.obj" \
	"$(INTDIR)\PcxDecode.obj" \
	"$(INTDIR)\Point.obj" \
	"$(INTDIR)\Quantize.obj" \
	"$(INTDIR)\RawDecode.obj" \
	"$(INTDIR)\RawEncode.obj" \
	"$(INTDIR)\Storage.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\SunRleDecode.obj" \
	"$(INTDIR)\TgaRleDecode.obj" \
	"$(INTDIR)\Unpack.obj" \
	"$(INTDIR)\XbmDecode.obj" \
	"$(INTDIR)\XbmEncode.obj" \
	"$(INTDIR)\ZipDecode.obj" \
	"$(INTDIR)\ZipEncode.obj" \
	"..\vc40\python14.lib" \
	".\jpeg-6\jpeg.lib" \
	".\zlib\zlib.lib"

".\_imaging.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "zlib - Win32 Debug" "jpeg - Win32 Debug" ".\_imaging.dll"

CLEAN : 
	-@erase "$(INTDIR)\_imagingmodule.obj"
	-@erase "$(INTDIR)\Bands.obj"
	-@erase "$(INTDIR)\Blend.obj"
	-@erase "$(INTDIR)\Chops.obj"
	-@erase "$(INTDIR)\Convert.obj"
	-@erase "$(INTDIR)\Copy.obj"
	-@erase "$(INTDIR)\Crc32.obj"
	-@erase "$(INTDIR)\Crop.obj"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\Dib.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\Draw.obj"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\EpsEncode.obj"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\Fill.obj"
	-@erase "$(INTDIR)\Filter.obj"
	-@erase "$(INTDIR)\FliDecode.obj"
	-@erase "$(INTDIR)\Geometry.obj"
	-@erase "$(INTDIR)\GetBBox.obj"
	-@erase "$(INTDIR)\GifDecode.obj"
	-@erase "$(INTDIR)\GifEncode.obj"
	-@erase "$(INTDIR)\HexDecode.obj"
	-@erase "$(INTDIR)\Histo.obj"
	-@erase "$(INTDIR)\JpegDecode.obj"
	-@erase "$(INTDIR)\JpegEncode.obj"
	-@erase "$(INTDIR)\LzwDecode.obj"
	-@erase "$(INTDIR)\Matrix.obj"
	-@erase "$(INTDIR)\MspDecode.obj"
	-@erase "$(INTDIR)\Negative.obj"
	-@erase "$(INTDIR)\Offset.obj"
	-@erase "$(INTDIR)\Pack.obj"
	-@erase "$(INTDIR)\PackDecode.obj"
	-@erase "$(INTDIR)\Palette.obj"
	-@erase "$(INTDIR)\Paste.obj"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\PcdDecode.obj"
	-@erase "$(INTDIR)\PcxDecode.obj"
	-@erase "$(INTDIR)\Point.obj"
	-@erase "$(INTDIR)\Quantize.obj"
	-@erase "$(INTDIR)\RawDecode.obj"
	-@erase "$(INTDIR)\RawEncode.obj"
	-@erase "$(INTDIR)\Storage.obj"
	-@erase "$(INTDIR)\String.obj"
	-@erase "$(INTDIR)\SunRleDecode.obj"
	-@erase "$(INTDIR)\TgaRleDecode.obj"
	-@erase "$(INTDIR)\Unpack.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\XbmDecode.obj"
	-@erase "$(INTDIR)\XbmEncode.obj"
	-@erase "$(INTDIR)\ZipDecode.obj"
	-@erase "$(INTDIR)\ZipEncode.obj"
	-@erase "$(OUTDIR)\_imaging.exp"
	-@erase "$(OUTDIR)\_imaging.lib"
	-@erase "$(OUTDIR)\_imaging.pdb"
	-@erase ".\_imaging.dll"
	-@erase ".\_imaging.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /O2 /Ob2 /I "../Include" /I "../PC" /I "./libImaging" /I "./jpeg-6" /I "./zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /O2 /Ob2 /I "../Include" /I "../PC" /I\
 "./libImaging" /I "./jpeg-6" /I "./zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/_imaging.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/_imaging.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"_imaging.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/_imaging.pdb" /debug /machine:I386 /def:".\_imaging.def"\
 /out:"_imaging.dll" /implib:"$(OUTDIR)/_imaging.lib" 
DEF_FILE= \
	".\_imaging.def"
LINK32_OBJS= \
	"$(INTDIR)\_imagingmodule.obj" \
	"$(INTDIR)\Bands.obj" \
	"$(INTDIR)\Blend.obj" \
	"$(INTDIR)\Chops.obj" \
	"$(INTDIR)\Convert.obj" \
	"$(INTDIR)\Copy.obj" \
	"$(INTDIR)\Crc32.obj" \
	"$(INTDIR)\Crop.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\Dib.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\Draw.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\EpsEncode.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\Fill.obj" \
	"$(INTDIR)\Filter.obj" \
	"$(INTDIR)\FliDecode.obj" \
	"$(INTDIR)\Geometry.obj" \
	"$(INTDIR)\GetBBox.obj" \
	"$(INTDIR)\GifDecode.obj" \
	"$(INTDIR)\GifEncode.obj" \
	"$(INTDIR)\HexDecode.obj" \
	"$(INTDIR)\Histo.obj" \
	"$(INTDIR)\JpegDecode.obj" \
	"$(INTDIR)\JpegEncode.obj" \
	"$(INTDIR)\LzwDecode.obj" \
	"$(INTDIR)\Matrix.obj" \
	"$(INTDIR)\MspDecode.obj" \
	"$(INTDIR)\Negative.obj" \
	"$(INTDIR)\Offset.obj" \
	"$(INTDIR)\Pack.obj" \
	"$(INTDIR)\PackDecode.obj" \
	"$(INTDIR)\Palette.obj" \
	"$(INTDIR)\Paste.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\PcdDecode.obj" \
	"$(INTDIR)\PcxDecode.obj" \
	"$(INTDIR)\Point.obj" \
	"$(INTDIR)\Quantize.obj" \
	"$(INTDIR)\RawDecode.obj" \
	"$(INTDIR)\RawEncode.obj" \
	"$(INTDIR)\Storage.obj" \
	"$(INTDIR)\String.obj" \
	"$(INTDIR)\SunRleDecode.obj" \
	"$(INTDIR)\TgaRleDecode.obj" \
	"$(INTDIR)\Unpack.obj" \
	"$(INTDIR)\XbmDecode.obj" \
	"$(INTDIR)\XbmEncode.obj" \
	"$(INTDIR)\ZipDecode.obj" \
	"$(INTDIR)\ZipEncode.obj" \
	"..\vc40\python14.lib" \
	".\jpeg-6\jpeg.lib" \
	".\zlib\zlib.lib"

".\_imaging.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib\Release"
# PROP BASE Intermediate_Dir "zlib\Release"
# PROP BASE Target_Dir "zlib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "zlib"
# PROP Intermediate_Dir "zlib/Release"
# PROP Target_Dir "zlib"
OUTDIR=.\zlib
INTDIR=.\zlib/Release

ALL : "$(OUTDIR)\zlib.lib"

CLEAN : 
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/zlib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\zlib/Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib\Debug"
# PROP BASE Intermediate_Dir "zlib\Debug"
# PROP BASE Target_Dir "zlib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "zlib"
# PROP Intermediate_Dir "zlib/Debug"
# PROP Target_Dir "zlib"
OUTDIR=.\zlib
INTDIR=.\zlib/Debug

ALL : "$(OUTDIR)\zlib.lib"

CLEAN : 
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/zlib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\zlib/Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj"

"$(OUTDIR)\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpeg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "jpeg\Release"
# PROP BASE Intermediate_Dir "jpeg\Release"
# PROP BASE Target_Dir "jpeg"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "jpeg-6"
# PROP Intermediate_Dir "jpeg-6\Release"
# PROP Target_Dir "jpeg"
OUTDIR=.\jpeg-6
INTDIR=.\jpeg-6\Release

ALL : "$(OUTDIR)\jpeg.lib"

CLEAN : 
	-@erase "$(INTDIR)\Jcapimin.obj"
	-@erase "$(INTDIR)\Jcapistd.obj"
	-@erase "$(INTDIR)\Jccoefct.obj"
	-@erase "$(INTDIR)\Jccolor.obj"
	-@erase "$(INTDIR)\Jcdctmgr.obj"
	-@erase "$(INTDIR)\Jchuff.obj"
	-@erase "$(INTDIR)\Jcinit.obj"
	-@erase "$(INTDIR)\Jcmainct.obj"
	-@erase "$(INTDIR)\Jcmarker.obj"
	-@erase "$(INTDIR)\Jcmaster.obj"
	-@erase "$(INTDIR)\Jcomapi.obj"
	-@erase "$(INTDIR)\Jcparam.obj"
	-@erase "$(INTDIR)\Jcphuff.obj"
	-@erase "$(INTDIR)\Jcprepct.obj"
	-@erase "$(INTDIR)\Jcsample.obj"
	-@erase "$(INTDIR)\Jctrans.obj"
	-@erase "$(INTDIR)\Jdapimin.obj"
	-@erase "$(INTDIR)\Jdapistd.obj"
	-@erase "$(INTDIR)\Jdatadst.obj"
	-@erase "$(INTDIR)\Jdatasrc.obj"
	-@erase "$(INTDIR)\Jdcoefct.obj"
	-@erase "$(INTDIR)\Jdcolor.obj"
	-@erase "$(INTDIR)\Jddctmgr.obj"
	-@erase "$(INTDIR)\Jdhuff.obj"
	-@erase "$(INTDIR)\Jdinput.obj"
	-@erase "$(INTDIR)\Jdmainct.obj"
	-@erase "$(INTDIR)\Jdmarker.obj"
	-@erase "$(INTDIR)\Jdmaster.obj"
	-@erase "$(INTDIR)\Jdmerge.obj"
	-@erase "$(INTDIR)\Jdphuff.obj"
	-@erase "$(INTDIR)\Jdpostct.obj"
	-@erase "$(INTDIR)\Jdsample.obj"
	-@erase "$(INTDIR)\Jdtrans.obj"
	-@erase "$(INTDIR)\Jerror.obj"
	-@erase "$(INTDIR)\Jfdctflt.obj"
	-@erase "$(INTDIR)\Jfdctfst.obj"
	-@erase "$(INTDIR)\Jfdctint.obj"
	-@erase "$(INTDIR)\Jidctflt.obj"
	-@erase "$(INTDIR)\Jidctfst.obj"
	-@erase "$(INTDIR)\Jidctint.obj"
	-@erase "$(INTDIR)\Jidctred.obj"
	-@erase "$(INTDIR)\Jmemmgr.obj"
	-@erase "$(INTDIR)\Jmemnobs.obj"
	-@erase "$(INTDIR)\Jpegtran.obj"
	-@erase "$(INTDIR)\Jquant1.obj"
	-@erase "$(INTDIR)\Jquant2.obj"
	-@erase "$(INTDIR)\Jutils.obj"
	-@erase "$(OUTDIR)\jpeg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/jpeg.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=".\jpeg-6\Release/"
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/jpeg.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/jpeg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Jcapimin.obj" \
	"$(INTDIR)\Jcapistd.obj" \
	"$(INTDIR)\Jccoefct.obj" \
	"$(INTDIR)\Jccolor.obj" \
	"$(INTDIR)\Jcdctmgr.obj" \
	"$(INTDIR)\Jchuff.obj" \
	"$(INTDIR)\Jcinit.obj" \
	"$(INTDIR)\Jcmainct.obj" \
	"$(INTDIR)\Jcmarker.obj" \
	"$(INTDIR)\Jcmaster.obj" \
	"$(INTDIR)\Jcomapi.obj" \
	"$(INTDIR)\Jcparam.obj" \
	"$(INTDIR)\Jcphuff.obj" \
	"$(INTDIR)\Jcprepct.obj" \
	"$(INTDIR)\Jcsample.obj" \
	"$(INTDIR)\Jctrans.obj" \
	"$(INTDIR)\Jdapimin.obj" \
	"$(INTDIR)\Jdapistd.obj" \
	"$(INTDIR)\Jdatadst.obj" \
	"$(INTDIR)\Jdatasrc.obj" \
	"$(INTDIR)\Jdcoefct.obj" \
	"$(INTDIR)\Jdcolor.obj" \
	"$(INTDIR)\Jddctmgr.obj" \
	"$(INTDIR)\Jdhuff.obj" \
	"$(INTDIR)\Jdinput.obj" \
	"$(INTDIR)\Jdmainct.obj" \
	"$(INTDIR)\Jdmarker.obj" \
	"$(INTDIR)\Jdmaster.obj" \
	"$(INTDIR)\Jdmerge.obj" \
	"$(INTDIR)\Jdphuff.obj" \
	"$(INTDIR)\Jdpostct.obj" \
	"$(INTDIR)\Jdsample.obj" \
	"$(INTDIR)\Jdtrans.obj" \
	"$(INTDIR)\Jerror.obj" \
	"$(INTDIR)\Jfdctflt.obj" \
	"$(INTDIR)\Jfdctfst.obj" \
	"$(INTDIR)\Jfdctint.obj" \
	"$(INTDIR)\Jidctflt.obj" \
	"$(INTDIR)\Jidctfst.obj" \
	"$(INTDIR)\Jidctint.obj" \
	"$(INTDIR)\Jidctred.obj" \
	"$(INTDIR)\Jmemmgr.obj" \
	"$(INTDIR)\Jmemnobs.obj" \
	"$(INTDIR)\Jpegtran.obj" \
	"$(INTDIR)\Jquant1.obj" \
	"$(INTDIR)\Jquant2.obj" \
	"$(INTDIR)\Jutils.obj"

"$(OUTDIR)\jpeg.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "jpeg\Debug"
# PROP BASE Intermediate_Dir "jpeg\Debug"
# PROP BASE Target_Dir "jpeg"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "jpeg-6"
# PROP Intermediate_Dir "jpeg-6\Debug"
# PROP Target_Dir "jpeg"
OUTDIR=.\jpeg-6
INTDIR=.\jpeg-6\Debug

ALL : "$(OUTDIR)\jpeg.lib"

CLEAN : 
	-@erase "$(INTDIR)\Jcapimin.obj"
	-@erase "$(INTDIR)\Jcapistd.obj"
	-@erase "$(INTDIR)\Jccoefct.obj"
	-@erase "$(INTDIR)\Jccolor.obj"
	-@erase "$(INTDIR)\Jcdctmgr.obj"
	-@erase "$(INTDIR)\Jchuff.obj"
	-@erase "$(INTDIR)\Jcinit.obj"
	-@erase "$(INTDIR)\Jcmainct.obj"
	-@erase "$(INTDIR)\Jcmarker.obj"
	-@erase "$(INTDIR)\Jcmaster.obj"
	-@erase "$(INTDIR)\Jcomapi.obj"
	-@erase "$(INTDIR)\Jcparam.obj"
	-@erase "$(INTDIR)\Jcphuff.obj"
	-@erase "$(INTDIR)\Jcprepct.obj"
	-@erase "$(INTDIR)\Jcsample.obj"
	-@erase "$(INTDIR)\Jctrans.obj"
	-@erase "$(INTDIR)\Jdapimin.obj"
	-@erase "$(INTDIR)\Jdapistd.obj"
	-@erase "$(INTDIR)\Jdatadst.obj"
	-@erase "$(INTDIR)\Jdatasrc.obj"
	-@erase "$(INTDIR)\Jdcoefct.obj"
	-@erase "$(INTDIR)\Jdcolor.obj"
	-@erase "$(INTDIR)\Jddctmgr.obj"
	-@erase "$(INTDIR)\Jdhuff.obj"
	-@erase "$(INTDIR)\Jdinput.obj"
	-@erase "$(INTDIR)\Jdmainct.obj"
	-@erase "$(INTDIR)\Jdmarker.obj"
	-@erase "$(INTDIR)\Jdmaster.obj"
	-@erase "$(INTDIR)\Jdmerge.obj"
	-@erase "$(INTDIR)\Jdphuff.obj"
	-@erase "$(INTDIR)\Jdpostct.obj"
	-@erase "$(INTDIR)\Jdsample.obj"
	-@erase "$(INTDIR)\Jdtrans.obj"
	-@erase "$(INTDIR)\Jerror.obj"
	-@erase "$(INTDIR)\Jfdctflt.obj"
	-@erase "$(INTDIR)\Jfdctfst.obj"
	-@erase "$(INTDIR)\Jfdctint.obj"
	-@erase "$(INTDIR)\Jidctflt.obj"
	-@erase "$(INTDIR)\Jidctfst.obj"
	-@erase "$(INTDIR)\Jidctint.obj"
	-@erase "$(INTDIR)\Jidctred.obj"
	-@erase "$(INTDIR)\Jmemmgr.obj"
	-@erase "$(INTDIR)\Jmemnobs.obj"
	-@erase "$(INTDIR)\Jpegtran.obj"
	-@erase "$(INTDIR)\Jquant1.obj"
	-@erase "$(INTDIR)\Jquant2.obj"
	-@erase "$(INTDIR)\Jutils.obj"
	-@erase "$(OUTDIR)\jpeg.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/jpeg.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=".\jpeg-6\Debug/"
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/jpeg.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/jpeg.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Jcapimin.obj" \
	"$(INTDIR)\Jcapistd.obj" \
	"$(INTDIR)\Jccoefct.obj" \
	"$(INTDIR)\Jccolor.obj" \
	"$(INTDIR)\Jcdctmgr.obj" \
	"$(INTDIR)\Jchuff.obj" \
	"$(INTDIR)\Jcinit.obj" \
	"$(INTDIR)\Jcmainct.obj" \
	"$(INTDIR)\Jcmarker.obj" \
	"$(INTDIR)\Jcmaster.obj" \
	"$(INTDIR)\Jcomapi.obj" \
	"$(INTDIR)\Jcparam.obj" \
	"$(INTDIR)\Jcphuff.obj" \
	"$(INTDIR)\Jcprepct.obj" \
	"$(INTDIR)\Jcsample.obj" \
	"$(INTDIR)\Jctrans.obj" \
	"$(INTDIR)\Jdapimin.obj" \
	"$(INTDIR)\Jdapistd.obj" \
	"$(INTDIR)\Jdatadst.obj" \
	"$(INTDIR)\Jdatasrc.obj" \
	"$(INTDIR)\Jdcoefct.obj" \
	"$(INTDIR)\Jdcolor.obj" \
	"$(INTDIR)\Jddctmgr.obj" \
	"$(INTDIR)\Jdhuff.obj" \
	"$(INTDIR)\Jdinput.obj" \
	"$(INTDIR)\Jdmainct.obj" \
	"$(INTDIR)\Jdmarker.obj" \
	"$(INTDIR)\Jdmaster.obj" \
	"$(INTDIR)\Jdmerge.obj" \
	"$(INTDIR)\Jdphuff.obj" \
	"$(INTDIR)\Jdpostct.obj" \
	"$(INTDIR)\Jdsample.obj" \
	"$(INTDIR)\Jdtrans.obj" \
	"$(INTDIR)\Jerror.obj" \
	"$(INTDIR)\Jfdctflt.obj" \
	"$(INTDIR)\Jfdctfst.obj" \
	"$(INTDIR)\Jfdctint.obj" \
	"$(INTDIR)\Jidctflt.obj" \
	"$(INTDIR)\Jidctfst.obj" \
	"$(INTDIR)\Jidctint.obj" \
	"$(INTDIR)\Jidctred.obj" \
	"$(INTDIR)\Jmemmgr.obj" \
	"$(INTDIR)\Jmemnobs.obj" \
	"$(INTDIR)\Jpegtran.obj" \
	"$(INTDIR)\Jquant1.obj" \
	"$(INTDIR)\Jquant2.obj" \
	"$(INTDIR)\Jutils.obj"

"$(OUTDIR)\jpeg.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "_imaging - Win32 Release"
# Name "_imaging - Win32 Debug"

!IF  "$(CFG)" == "_imaging - Win32 Release"

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=..\vc40\python14.lib

!IF  "$(CFG)" == "_imaging - Win32 Release"

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\encode.c
DEP_CPP_ENCOD=\
	"..\Include\abstract.h"\
	"..\Include\accessobject.h"\
	"..\Include\allobjects.h"\
	"..\Include\bltinmodule.h"\
	"..\Include\ceval.h"\
	"..\Include\classobject.h"\
	"..\Include\cobject.h"\
	"..\Include\complexobject.h"\
	"..\Include\fileobject.h"\
	"..\Include\floatobject.h"\
	"..\Include\funcobject.h"\
	"..\Include\import.h"\
	"..\Include\intobject.h"\
	"..\Include\intrcheck.h"\
	"..\Include\listobject.h"\
	"..\Include\longobject.h"\
	"..\Include\mappingobject.h"\
	"..\Include\methodobject.h"\
	"..\Include\modsupport.h"\
	"..\Include\moduleobject.h"\
	"..\Include\mymalloc.h"\
	"..\Include\myproto.h"\
	"..\Include\object.h"\
	"..\Include\objimpl.h"\
	"..\Include\pydebug.h"\
	"..\Include\pyerrors.h"\
	"..\Include\pythonrun.h"\
	"..\Include\rangeobject.h"\
	"..\Include\rename2.h"\
	"..\Include\sliceobject.h"\
	"..\Include\stringobject.h"\
	"..\Include\sysmodule.h"\
	"..\Include\thread.h"\
	"..\Include\traceback.h"\
	"..\Include\tupleobject.h"\
	".\../Include\Python.h"\
	".\../PC\config.h"\
	".\./jpeg-6\jpeglib.h"\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Jpeg.h"\
	".\./libImaging\Zip.h"\
	".\./zlib\zlib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\encode.obj" : $(SOURCE) $(DEP_CPP_ENCOD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\decode.c
DEP_CPP_DECOD=\
	"..\Include\abstract.h"\
	"..\Include\accessobject.h"\
	"..\Include\allobjects.h"\
	"..\Include\bltinmodule.h"\
	"..\Include\ceval.h"\
	"..\Include\classobject.h"\
	"..\Include\cobject.h"\
	"..\Include\complexobject.h"\
	"..\Include\fileobject.h"\
	"..\Include\floatobject.h"\
	"..\Include\funcobject.h"\
	"..\Include\import.h"\
	"..\Include\intobject.h"\
	"..\Include\intrcheck.h"\
	"..\Include\listobject.h"\
	"..\Include\longobject.h"\
	"..\Include\mappingobject.h"\
	"..\Include\methodobject.h"\
	"..\Include\modsupport.h"\
	"..\Include\moduleobject.h"\
	"..\Include\mymalloc.h"\
	"..\Include\myproto.h"\
	"..\Include\object.h"\
	"..\Include\objimpl.h"\
	"..\Include\pydebug.h"\
	"..\Include\pyerrors.h"\
	"..\Include\pythonrun.h"\
	"..\Include\rangeobject.h"\
	"..\Include\rename2.h"\
	"..\Include\sliceobject.h"\
	"..\Include\stringobject.h"\
	"..\Include\sysmodule.h"\
	"..\Include\thread.h"\
	"..\Include\traceback.h"\
	"..\Include\tupleobject.h"\
	".\../Include\Python.h"\
	".\../PC\config.h"\
	".\./jpeg-6\jpeglib.h"\
	".\./libImaging\Gif.h"\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Jpeg.h"\
	".\./libImaging\Lzw.h"\
	".\./libImaging\Raw.h"\
	".\./libImaging\Zip.h"\
	".\./zlib\zlib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\decode.obj" : $(SOURCE) $(DEP_CPP_DECOD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\display.c
DEP_CPP_DISPL=\
	"..\Include\abstract.h"\
	"..\Include\accessobject.h"\
	"..\Include\allobjects.h"\
	"..\Include\bltinmodule.h"\
	"..\Include\ceval.h"\
	"..\Include\classobject.h"\
	"..\Include\cobject.h"\
	"..\Include\complexobject.h"\
	"..\Include\fileobject.h"\
	"..\Include\floatobject.h"\
	"..\Include\funcobject.h"\
	"..\Include\import.h"\
	"..\Include\intobject.h"\
	"..\Include\intrcheck.h"\
	"..\Include\listobject.h"\
	"..\Include\longobject.h"\
	"..\Include\mappingobject.h"\
	"..\Include\methodobject.h"\
	"..\Include\modsupport.h"\
	"..\Include\moduleobject.h"\
	"..\Include\mymalloc.h"\
	"..\Include\myproto.h"\
	"..\Include\object.h"\
	"..\Include\objimpl.h"\
	"..\Include\pydebug.h"\
	"..\Include\pyerrors.h"\
	"..\Include\pythonrun.h"\
	"..\Include\rangeobject.h"\
	"..\Include\rename2.h"\
	"..\Include\sliceobject.h"\
	"..\Include\stringobject.h"\
	"..\Include\sysmodule.h"\
	"..\Include\thread.h"\
	"..\Include\traceback.h"\
	"..\Include\tupleobject.h"\
	".\../Include\Python.h"\
	".\../PC\config.h"\
	".\./libImaging\Imaging.h"\
	".\./libImaging\ImDib.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\display.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\_imagingmodule.c
DEP_CPP__IMAG=\
	"..\Include\abstract.h"\
	"..\Include\accessobject.h"\
	"..\Include\allobjects.h"\
	"..\Include\bltinmodule.h"\
	"..\Include\ceval.h"\
	"..\Include\classobject.h"\
	"..\Include\cobject.h"\
	"..\Include\complexobject.h"\
	"..\Include\fileobject.h"\
	"..\Include\floatobject.h"\
	"..\Include\funcobject.h"\
	"..\Include\import.h"\
	"..\Include\intobject.h"\
	"..\Include\intrcheck.h"\
	"..\Include\listobject.h"\
	"..\Include\longobject.h"\
	"..\Include\mappingobject.h"\
	"..\Include\methodobject.h"\
	"..\Include\modsupport.h"\
	"..\Include\moduleobject.h"\
	"..\Include\mymalloc.h"\
	"..\Include\myproto.h"\
	"..\Include\object.h"\
	"..\Include\objimpl.h"\
	"..\Include\pydebug.h"\
	"..\Include\pyerrors.h"\
	"..\Include\pythonrun.h"\
	"..\Include\rangeobject.h"\
	"..\Include\rename2.h"\
	"..\Include\sliceobject.h"\
	"..\Include\stringobject.h"\
	"..\Include\sysmodule.h"\
	"..\Include\thread.h"\
	"..\Include\traceback.h"\
	"..\Include\tupleobject.h"\
	".\../Include\Python.h"\
	".\../PC\config.h"\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\_imagingmodule.obj" : $(SOURCE) $(DEP_CPP__IMAG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\XbmEncode.c
DEP_CPP_XBMEN=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\XbmEncode.obj" : $(SOURCE) $(DEP_CPP_XBMEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Blend.c
DEP_CPP_BLEND=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Blend.obj" : $(SOURCE) $(DEP_CPP_BLEND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Chops.c
DEP_CPP_CHOPS=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Chops.obj" : $(SOURCE) $(DEP_CPP_CHOPS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Convert.c
DEP_CPP_CONVE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Copy.c
DEP_CPP_COPY_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Copy.obj" : $(SOURCE) $(DEP_CPP_COPY_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Crop.c
DEP_CPP_CROP_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Crop.obj" : $(SOURCE) $(DEP_CPP_CROP_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Dib.c
DEP_CPP_DIB_C=\
	".\./libImaging\Imaging.h"\
	".\./libImaging\ImDib.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Dib.obj" : $(SOURCE) $(DEP_CPP_DIB_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Draw.c
DEP_CPP_DRAW_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Draw.obj" : $(SOURCE) $(DEP_CPP_DRAW_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\EpsEncode.c
DEP_CPP_EPSEN=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\EpsEncode.obj" : $(SOURCE) $(DEP_CPP_EPSEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\File.c
DEP_CPP_FILE_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\File.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Fill.c
DEP_CPP_FILL_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Fill.obj" : $(SOURCE) $(DEP_CPP_FILL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Filter.c
DEP_CPP_FILTE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Geometry.c
DEP_CPP_GEOME=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Geometry.obj" : $(SOURCE) $(DEP_CPP_GEOME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\GetBBox.c
DEP_CPP_GETBB=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\GetBBox.obj" : $(SOURCE) $(DEP_CPP_GETBB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\GifDecode.c
DEP_CPP_GIFDE=\
	".\./libImaging\Gif.h"\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\GifDecode.obj" : $(SOURCE) $(DEP_CPP_GIFDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\HexDecode.c
DEP_CPP_HEXDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\HexDecode.obj" : $(SOURCE) $(DEP_CPP_HEXDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Histo.c
DEP_CPP_HISTO=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Histo.obj" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\JpegDecode.c
DEP_CPP_JPEGD=\
	".\./jpeg-6\jpeglib.h"\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Jpeg.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\JpegDecode.obj" : $(SOURCE) $(DEP_CPP_JPEGD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\JpegEncode.c
DEP_CPP_JPEGE=\
	".\./jpeg-6\jpeglib.h"\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Jpeg.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\JpegEncode.obj" : $(SOURCE) $(DEP_CPP_JPEGE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\LzwDecode.c
DEP_CPP_LZWDE=\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Lzw.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\LzwDecode.obj" : $(SOURCE) $(DEP_CPP_LZWDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Matrix.c
DEP_CPP_MATRI=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Matrix.obj" : $(SOURCE) $(DEP_CPP_MATRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Negative.c
DEP_CPP_NEGAT=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Negative.obj" : $(SOURCE) $(DEP_CPP_NEGAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Offset.c
DEP_CPP_OFFSE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Offset.obj" : $(SOURCE) $(DEP_CPP_OFFSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Pack.c
DEP_CPP_PACK_=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Pack.obj" : $(SOURCE) $(DEP_CPP_PACK_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\PackDecode.c
DEP_CPP_PACKD=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\PackDecode.obj" : $(SOURCE) $(DEP_CPP_PACKD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Palette.c
DEP_CPP_PALET=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Palette.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Paste.c
DEP_CPP_PASTE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Paste.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\PcdDecode.c
DEP_CPP_PCDDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\PcdDecode.obj" : $(SOURCE) $(DEP_CPP_PCDDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\PcxDecode.c
DEP_CPP_PCXDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\PcxDecode.obj" : $(SOURCE) $(DEP_CPP_PCXDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Point.c
DEP_CPP_POINT=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Point.obj" : $(SOURCE) $(DEP_CPP_POINT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Quantize.c
DEP_CPP_QUANT=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Quantize.obj" : $(SOURCE) $(DEP_CPP_QUANT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\RawDecode.c
DEP_CPP_RAWDE=\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Raw.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\RawDecode.obj" : $(SOURCE) $(DEP_CPP_RAWDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\RawEncode.c
DEP_CPP_RAWEN=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\RawEncode.obj" : $(SOURCE) $(DEP_CPP_RAWEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Storage.c
DEP_CPP_STORA=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Storage.obj" : $(SOURCE) $(DEP_CPP_STORA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\String.c
DEP_CPP_STRIN=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\String.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Unpack.c
DEP_CPP_UNPAC=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Unpack.obj" : $(SOURCE) $(DEP_CPP_UNPAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\XbmDecode.c
DEP_CPP_XBMDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\XbmDecode.obj" : $(SOURCE) $(DEP_CPP_XBMDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Bands.c
DEP_CPP_BANDS=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Bands.obj" : $(SOURCE) $(DEP_CPP_BANDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\_imaging.def

!IF  "$(CFG)" == "_imaging - Win32 Release"

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\path.c
NODEP_CPP_PATH_=\
	".\Python.h"\
	

"$(INTDIR)\path.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\Crc32.c
DEP_CPP_CRC32=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\Crc32.obj" : $(SOURCE) $(DEP_CPP_CRC32) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\ZipDecode.c
DEP_CPP_ZIPDE=\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Zip.h"\
	".\./zlib\zlib.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\ZipDecode.obj" : $(SOURCE) $(DEP_CPP_ZIPDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "jpeg"

!IF  "$(CFG)" == "_imaging - Win32 Release"

"jpeg - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\_imaging.mak" CFG="jpeg - Win32 Release" 

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

"jpeg - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\_imaging.mak" CFG="jpeg - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "zlib"

!IF  "$(CFG)" == "_imaging - Win32 Release"

"zlib - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\_imaging.mak" CFG="zlib - Win32 Release" 

!ELSEIF  "$(CFG)" == "_imaging - Win32 Debug"

"zlib - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\_imaging.mak" CFG="zlib - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\libimaging\ZipEncode.c
DEP_CPP_ZIPEN=\
	".\./libImaging\Imaging.h"\
	".\./libImaging\Zip.h"\
	".\./zlib\zlib.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\ZipEncode.obj" : $(SOURCE) $(DEP_CPP_ZIPEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\FliDecode.c
DEP_CPP_FLIDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\FliDecode.obj" : $(SOURCE) $(DEP_CPP_FLIDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\MspDecode.c
DEP_CPP_MSPDE=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\MspDecode.obj" : $(SOURCE) $(DEP_CPP_MSPDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\SunRleDecode.c
DEP_CPP_SUNRL=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\SunRleDecode.obj" : $(SOURCE) $(DEP_CPP_SUNRL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\TgaRleDecode.c
DEP_CPP_TGARL=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\TgaRleDecode.obj" : $(SOURCE) $(DEP_CPP_TGARL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\libimaging\GifEncode.c
DEP_CPP_GIFEN=\
	".\./libImaging\Imaging.h"\
	".\libimaging\ImConfig.h"\
	".\libimaging\ImPlatform.h"\
	

"$(INTDIR)\GifEncode.obj" : $(SOURCE) $(DEP_CPP_GIFEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "zlib - Win32 Release"
# Name "zlib - Win32 Debug"

!IF  "$(CFG)" == "zlib - Win32 Release"

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\zlib\adler32.c
DEP_CPP_ADLER=\
	".\./zlib\zlib.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\adler32.obj" : $(SOURCE) $(DEP_CPP_ADLER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\compress.c
DEP_CPP_COMPR=\
	".\./zlib\zlib.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\deflate.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_DEFLA=\
	".\zlib\deflate.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_DEFLA=\
	".\./zlib\zlib.h"\
	".\zlib\deflate.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\deflate.obj" : $(SOURCE) $(DEP_CPP_DEFLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\gzio.c
DEP_CPP_GZIO_=\
	".\./zlib\zlib.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\gzio.obj" : $(SOURCE) $(DEP_CPP_GZIO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\infblock.c
DEP_CPP_INFBL=\
	".\./zlib\zlib.h"\
	".\zlib\infblock.h"\
	".\zlib\infcodes.h"\
	".\zlib\inftrees.h"\
	".\zlib\infutil.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\infblock.obj" : $(SOURCE) $(DEP_CPP_INFBL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\infcodes.c
DEP_CPP_INFCO=\
	".\./zlib\zlib.h"\
	".\zlib\infblock.h"\
	".\zlib\infcodes.h"\
	".\zlib\inffast.h"\
	".\zlib\inftrees.h"\
	".\zlib\infutil.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\infcodes.obj" : $(SOURCE) $(DEP_CPP_INFCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\inffast.c
DEP_CPP_INFFA=\
	".\./zlib\zlib.h"\
	".\zlib\infblock.h"\
	".\zlib\infcodes.h"\
	".\zlib\inffast.h"\
	".\zlib\inftrees.h"\
	".\zlib\infutil.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\inffast.obj" : $(SOURCE) $(DEP_CPP_INFFA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\inflate.c
DEP_CPP_INFLA=\
	".\./zlib\zlib.h"\
	".\zlib\infblock.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\inflate.obj" : $(SOURCE) $(DEP_CPP_INFLA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\inftrees.c
DEP_CPP_INFTR=\
	".\./zlib\zlib.h"\
	".\zlib\inftrees.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\inftrees.obj" : $(SOURCE) $(DEP_CPP_INFTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\infutil.c
DEP_CPP_INFUT=\
	".\./zlib\zlib.h"\
	".\zlib\infblock.h"\
	".\zlib\infcodes.h"\
	".\zlib\inftrees.h"\
	".\zlib\infutil.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\infutil.obj" : $(SOURCE) $(DEP_CPP_INFUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\trees.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_TREES=\
	".\zlib\deflate.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_TREES=\
	".\./zlib\zlib.h"\
	".\zlib\deflate.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\trees.obj" : $(SOURCE) $(DEP_CPP_TREES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\uncompr.c
DEP_CPP_UNCOM=\
	".\./zlib\zlib.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\uncompr.obj" : $(SOURCE) $(DEP_CPP_UNCOM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zlib\zutil.c
DEP_CPP_ZUTIL=\
	".\./zlib\zlib.h"\
	".\zlib\zconf.h"\
	".\zlib\zutil.h"\
	

"$(INTDIR)\zutil.obj" : $(SOURCE) $(DEP_CPP_ZUTIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "jpeg - Win32 Release"
# Name "jpeg - Win32 Debug"

!IF  "$(CFG)" == "jpeg - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcapimin.c"
DEP_CPP_JCAPI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcapimin.obj" : $(SOURCE) $(DEP_CPP_JCAPI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcapistd.c"
DEP_CPP_JCAPIS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcapistd.obj" : $(SOURCE) $(DEP_CPP_JCAPIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jccoefct.c"
DEP_CPP_JCCOE=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jccoefct.obj" : $(SOURCE) $(DEP_CPP_JCCOE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jccolor.c"
DEP_CPP_JCCOL=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jccolor.obj" : $(SOURCE) $(DEP_CPP_JCCOL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcdctmgr.c"
DEP_CPP_JCDCT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcdctmgr.obj" : $(SOURCE) $(DEP_CPP_JCDCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jchuff.c"
DEP_CPP_JCHUF=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jchuff.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jchuff.obj" : $(SOURCE) $(DEP_CPP_JCHUF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcinit.c"
DEP_CPP_JCINI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcinit.obj" : $(SOURCE) $(DEP_CPP_JCINI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcmainct.c"
DEP_CPP_JCMAI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcmainct.obj" : $(SOURCE) $(DEP_CPP_JCMAI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcmarker.c"
DEP_CPP_JCMAR=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcmarker.obj" : $(SOURCE) $(DEP_CPP_JCMAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcmaster.c"
DEP_CPP_JCMAS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcmaster.obj" : $(SOURCE) $(DEP_CPP_JCMAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcomapi.c"
DEP_CPP_JCOMA=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcomapi.obj" : $(SOURCE) $(DEP_CPP_JCOMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcparam.c"
DEP_CPP_JCPAR=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcparam.obj" : $(SOURCE) $(DEP_CPP_JCPAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcphuff.c"
DEP_CPP_JCPHU=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jchuff.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcphuff.obj" : $(SOURCE) $(DEP_CPP_JCPHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcprepct.c"
DEP_CPP_JCPRE=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcprepct.obj" : $(SOURCE) $(DEP_CPP_JCPRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jcsample.c"
DEP_CPP_JCSAM=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jcsample.obj" : $(SOURCE) $(DEP_CPP_JCSAM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jctrans.c"
DEP_CPP_JCTRA=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jctrans.obj" : $(SOURCE) $(DEP_CPP_JCTRA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdapimin.c"
DEP_CPP_JDAPI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdapimin.obj" : $(SOURCE) $(DEP_CPP_JDAPI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdapistd.c"
DEP_CPP_JDAPIS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdapistd.obj" : $(SOURCE) $(DEP_CPP_JDAPIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdatadst.c"
DEP_CPP_JDATA=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdatadst.obj" : $(SOURCE) $(DEP_CPP_JDATA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdatasrc.c"
DEP_CPP_JDATAS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdatasrc.obj" : $(SOURCE) $(DEP_CPP_JDATAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdcoefct.c"
DEP_CPP_JDCOE=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdcoefct.obj" : $(SOURCE) $(DEP_CPP_JDCOE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdcolor.c"
DEP_CPP_JDCOL=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdcolor.obj" : $(SOURCE) $(DEP_CPP_JDCOL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jddctmgr.c"
DEP_CPP_JDDCT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jddctmgr.obj" : $(SOURCE) $(DEP_CPP_JDDCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdhuff.c"
DEP_CPP_JDHUF=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdhuff.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdhuff.obj" : $(SOURCE) $(DEP_CPP_JDHUF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdinput.c"
DEP_CPP_JDINP=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdinput.obj" : $(SOURCE) $(DEP_CPP_JDINP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdmainct.c"
DEP_CPP_JDMAI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdmainct.obj" : $(SOURCE) $(DEP_CPP_JDMAI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdmarker.c"
DEP_CPP_JDMAR=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdmarker.obj" : $(SOURCE) $(DEP_CPP_JDMAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdmaster.c"
DEP_CPP_JDMAS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdmaster.obj" : $(SOURCE) $(DEP_CPP_JDMAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdmerge.c"
DEP_CPP_JDMER=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdmerge.obj" : $(SOURCE) $(DEP_CPP_JDMER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdphuff.c"
DEP_CPP_JDPHU=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdhuff.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdphuff.obj" : $(SOURCE) $(DEP_CPP_JDPHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdpostct.c"
DEP_CPP_JDPOS=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdpostct.obj" : $(SOURCE) $(DEP_CPP_JDPOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdsample.c"
DEP_CPP_JDSAM=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdsample.obj" : $(SOURCE) $(DEP_CPP_JDSAM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jdtrans.c"
DEP_CPP_JDTRA=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jdtrans.obj" : $(SOURCE) $(DEP_CPP_JDTRA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jerror.c"
DEP_CPP_JERRO=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\Jpeg-6\jversion.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jerror.obj" : $(SOURCE) $(DEP_CPP_JERRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jfdctflt.c"
DEP_CPP_JFDCT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jfdctflt.obj" : $(SOURCE) $(DEP_CPP_JFDCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jfdctfst.c"
DEP_CPP_JFDCTF=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jfdctfst.obj" : $(SOURCE) $(DEP_CPP_JFDCTF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jfdctint.c"
DEP_CPP_JFDCTI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jfdctint.obj" : $(SOURCE) $(DEP_CPP_JFDCTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jidctflt.c"
DEP_CPP_JIDCT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jidctflt.obj" : $(SOURCE) $(DEP_CPP_JIDCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jidctfst.c"
DEP_CPP_JIDCTF=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jidctfst.obj" : $(SOURCE) $(DEP_CPP_JIDCTF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jidctint.c"
DEP_CPP_JIDCTI=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jidctint.obj" : $(SOURCE) $(DEP_CPP_JIDCTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jidctred.c"
DEP_CPP_JIDCTR=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jdct.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jidctred.obj" : $(SOURCE) $(DEP_CPP_JIDCTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jmemnobs.c"
DEP_CPP_JMEMN=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmemsys.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jmemnobs.obj" : $(SOURCE) $(DEP_CPP_JMEMN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jpegtran.c"
DEP_CPP_JPEGT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\cderror.h"\
	".\Jpeg-6\cdjpeg.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	".\Jpeg-6\jversion.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jpegtran.obj" : $(SOURCE) $(DEP_CPP_JPEGT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jquant1.c"
DEP_CPP_JQUAN=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jquant1.obj" : $(SOURCE) $(DEP_CPP_JQUAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jquant2.c"
DEP_CPP_JQUANT=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jquant2.obj" : $(SOURCE) $(DEP_CPP_JQUANT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jutils.c"
DEP_CPP_JUTIL=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jutils.obj" : $(SOURCE) $(DEP_CPP_JUTIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=".\Jpeg-6\Jmemmgr.c"
DEP_CPP_JMEMM=\
	".\./jpeg-6\jpeglib.h"\
	".\Jpeg-6\jconfig.h"\
	".\Jpeg-6\jerror.h"\
	".\Jpeg-6\jinclude.h"\
	".\Jpeg-6\jmemsys.h"\
	".\Jpeg-6\jmorecfg.h"\
	".\Jpeg-6\jpegint.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\Jmemmgr.obj" : $(SOURCE) $(DEP_CPP_JMEMM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
