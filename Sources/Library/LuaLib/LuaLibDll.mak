# Microsoft Developer Studio Generated NMAKE File, Based on LuaLibDll.dsp
!IF $(CFG)" == "
CFG=LuaLibDll - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LuaLibDll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LuaLibDll - Win32 Release" && "$(CFG)" != "LuaLibDll - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LuaLibDll.mak" CFG="LuaLibDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LuaLibDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LuaLibDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF $(OS)" == "Windows_NT
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

ALL : "$(OUTDIR)\LuaLibDll.dll" "$(OUTDIR)\LuaLibDll.bsc"


CLEAN :
	-@erase "$(INTDIR)\dump.obj"
	-@erase "$(INTDIR)\dump.sbr"
	-@erase "$(INTDIR)\lapi.obj"
	-@erase "$(INTDIR)\lapi.sbr"
	-@erase "$(INTDIR)\lauxlib.obj"
	-@erase "$(INTDIR)\lauxlib.sbr"
	-@erase "$(INTDIR)\lbaselib.obj"
	-@erase "$(INTDIR)\lbaselib.sbr"
	-@erase "$(INTDIR)\lcode.obj"
	-@erase "$(INTDIR)\lcode.sbr"
	-@erase "$(INTDIR)\ldblib.obj"
	-@erase "$(INTDIR)\ldblib.sbr"
	-@erase "$(INTDIR)\ldebug.obj"
	-@erase "$(INTDIR)\ldebug.sbr"
	-@erase "$(INTDIR)\ldo.obj"
	-@erase "$(INTDIR)\ldo.sbr"
	-@erase "$(INTDIR)\lfunc.obj"
	-@erase "$(INTDIR)\lfunc.sbr"
	-@erase "$(INTDIR)\lgc.obj"
	-@erase "$(INTDIR)\lgc.sbr"
	-@erase "$(INTDIR)\liolib.obj"
	-@erase "$(INTDIR)\liolib.sbr"
	-@erase "$(INTDIR)\llex.obj"
	-@erase "$(INTDIR)\llex.sbr"
	-@erase "$(INTDIR)\lmathlib.obj"
	-@erase "$(INTDIR)\lmathlib.sbr"
	-@erase "$(INTDIR)\lmem.obj"
	-@erase "$(INTDIR)\lmem.sbr"
	-@erase "$(INTDIR)\lobject.obj"
	-@erase "$(INTDIR)\lobject.sbr"
	-@erase "$(INTDIR)\lparser.obj"
	-@erase "$(INTDIR)\lparser.sbr"
	-@erase "$(INTDIR)\lstate.obj"
	-@erase "$(INTDIR)\lstate.sbr"
	-@erase "$(INTDIR)\lstring.obj"
	-@erase "$(INTDIR)\lstring.sbr"
	-@erase "$(INTDIR)\lstrlib.obj"
	-@erase "$(INTDIR)\lstrlib.sbr"
	-@erase "$(INTDIR)\ltable.obj"
	-@erase "$(INTDIR)\ltable.sbr"
	-@erase "$(INTDIR)\ltests.obj"
	-@erase "$(INTDIR)\ltests.sbr"
	-@erase "$(INTDIR)\ltm.obj"
	-@erase "$(INTDIR)\ltm.sbr"
	-@erase "$(INTDIR)\lua.obj"
	-@erase "$(INTDIR)\lua.sbr"
	-@erase "$(INTDIR)\luac.obj"
	-@erase "$(INTDIR)\luac.sbr"
	-@erase "$(INTDIR)\LuaExtend.obj"
	-@erase "$(INTDIR)\LuaExtend.sbr"
	-@erase "$(INTDIR)\lundump.obj"
	-@erase "$(INTDIR)\lundump.sbr"
	-@erase "$(INTDIR)\lvm.obj"
	-@erase "$(INTDIR)\lvm.sbr"
	-@erase "$(INTDIR)\lzio.obj"
	-@erase "$(INTDIR)\lzio.sbr"
	-@erase "$(INTDIR)\opt.obj"
	-@erase "$(INTDIR)\opt.sbr"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\print.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LuaLibDll.bsc"
	-@erase "$(OUTDIR)\LuaLibDll.dll"
	-@erase "$(OUTDIR)\LuaLibDll.exp"
	-@erase "$(OUTDIR)\LuaLibDll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".\LuaLibDll\src" /I ".\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUALIBDLL_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\LuaLibDll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LuaLibDll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dump.sbr" \
	"$(INTDIR)\lapi.sbr" \
	"$(INTDIR)\lcode.sbr" \
	"$(INTDIR)\ldebug.sbr" \
	"$(INTDIR)\ldo.sbr" \
	"$(INTDIR)\lfunc.sbr" \
	"$(INTDIR)\lgc.sbr" \
	"$(INTDIR)\llex.sbr" \
	"$(INTDIR)\lmem.sbr" \
	"$(INTDIR)\lobject.sbr" \
	"$(INTDIR)\lparser.sbr" \
	"$(INTDIR)\lstate.sbr" \
	"$(INTDIR)\lstring.sbr" \
	"$(INTDIR)\ltable.sbr" \
	"$(INTDIR)\ltests.sbr" \
	"$(INTDIR)\ltm.sbr" \
	"$(INTDIR)\lua.sbr" \
	"$(INTDIR)\luac.sbr" \
	"$(INTDIR)\LuaExtend.sbr" \
	"$(INTDIR)\lundump.sbr" \
	"$(INTDIR)\lvm.sbr" \
	"$(INTDIR)\lzio.sbr" \
	"$(INTDIR)\opt.sbr" \
	"$(INTDIR)\print.sbr" \
	"$(INTDIR)\lauxlib.sbr" \
	"$(INTDIR)\lbaselib.sbr" \
	"$(INTDIR)\ldblib.sbr" \
	"$(INTDIR)\liolib.sbr" \
	"$(INTDIR)\lmathlib.sbr" \
	"$(INTDIR)\lstrlib.sbr"

"$(OUTDIR)\LuaLibDll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\LuaLibDll.pdb" /machine:I386 /out:"$(OUTDIR)\LuaLibDll.dll" /implib:"$(OUTDIR)\LuaLibDll.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dump.obj" \
	"$(INTDIR)\lapi.obj" \
	"$(INTDIR)\lcode.obj" \
	"$(INTDIR)\ldebug.obj" \
	"$(INTDIR)\ldo.obj" \
	"$(INTDIR)\lfunc.obj" \
	"$(INTDIR)\lgc.obj" \
	"$(INTDIR)\llex.obj" \
	"$(INTDIR)\lmem.obj" \
	"$(INTDIR)\lobject.obj" \
	"$(INTDIR)\lparser.obj" \
	"$(INTDIR)\lstate.obj" \
	"$(INTDIR)\lstring.obj" \
	"$(INTDIR)\ltable.obj" \
	"$(INTDIR)\ltests.obj" \
	"$(INTDIR)\ltm.obj" \
	"$(INTDIR)\lua.obj" \
	"$(INTDIR)\luac.obj" \
	"$(INTDIR)\LuaExtend.obj" \
	"$(INTDIR)\lundump.obj" \
	"$(INTDIR)\lvm.obj" \
	"$(INTDIR)\lzio.obj" \
	"$(INTDIR)\opt.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\lauxlib.obj" \
	"$(INTDIR)\lbaselib.obj" \
	"$(INTDIR)\ldblib.obj" \
	"$(INTDIR)\liolib.obj" \
	"$(INTDIR)\lmathlib.obj" \
	"$(INTDIR)\lstrlib.obj"

"$(OUTDIR)\LuaLibDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Release
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\LuaLibDll.dll" "$(OUTDIR)\LuaLibDll.bsc"
   copy Release\lualibdll.dll ..\..\..\Lib\lualibdll.dll
	copy Release\lualibdll.lib ..\..\..\Lib\lualibdll.lib
	copy Release\lualibdll.dll ..\..\..\bin\server\lualibdll.dll
	copy Release\lualibdll.lib ..\..\..\bin\server\lualibdll.lib
	copy Release\lualibdll.dll ..\..\..\bin\client\lualibdll.dll
	copy Release\lualibdll.lib ..\..\..\bin\client\lualibdll.lib
	copy Release\lualibdll.lib ..\..\..\Lib\release\lualibdll.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

ALL : "$(OUTDIR)\LuaLibDll.dll"


CLEAN :
	-@erase "$(INTDIR)\dump.obj"
	-@erase "$(INTDIR)\lapi.obj"
	-@erase "$(INTDIR)\lauxlib.obj"
	-@erase "$(INTDIR)\lbaselib.obj"
	-@erase "$(INTDIR)\lcode.obj"
	-@erase "$(INTDIR)\ldblib.obj"
	-@erase "$(INTDIR)\ldebug.obj"
	-@erase "$(INTDIR)\ldo.obj"
	-@erase "$(INTDIR)\lfunc.obj"
	-@erase "$(INTDIR)\lgc.obj"
	-@erase "$(INTDIR)\liolib.obj"
	-@erase "$(INTDIR)\llex.obj"
	-@erase "$(INTDIR)\lmathlib.obj"
	-@erase "$(INTDIR)\lmem.obj"
	-@erase "$(INTDIR)\lobject.obj"
	-@erase "$(INTDIR)\lparser.obj"
	-@erase "$(INTDIR)\lstate.obj"
	-@erase "$(INTDIR)\lstring.obj"
	-@erase "$(INTDIR)\lstrlib.obj"
	-@erase "$(INTDIR)\ltable.obj"
	-@erase "$(INTDIR)\ltests.obj"
	-@erase "$(INTDIR)\ltm.obj"
	-@erase "$(INTDIR)\lua.obj"
	-@erase "$(INTDIR)\luac.obj"
	-@erase "$(INTDIR)\LuaExtend.obj"
	-@erase "$(INTDIR)\lundump.obj"
	-@erase "$(INTDIR)\lvm.obj"
	-@erase "$(INTDIR)\lzio.obj"
	-@erase "$(INTDIR)\opt.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LuaLibDll.dll"
	-@erase "$(OUTDIR)\LuaLibDll.exp"
	-@erase "$(OUTDIR)\LuaLibDll.ilk"
	-@erase "$(OUTDIR)\LuaLibDll.lib"
	-@erase "$(OUTDIR)\LuaLibDll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUALIBDLL_EXPORTS" /Fp"$(INTDIR)\LuaLibDll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LuaLibDll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\LuaLibDll.pdb" /debug /machine:I386 /out:"$(OUTDIR)\LuaLibDll.dll" /implib:"$(OUTDIR)\LuaLibDll.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dump.obj" \
	"$(INTDIR)\lapi.obj" \
	"$(INTDIR)\lcode.obj" \
	"$(INTDIR)\ldebug.obj" \
	"$(INTDIR)\ldo.obj" \
	"$(INTDIR)\lfunc.obj" \
	"$(INTDIR)\lgc.obj" \
	"$(INTDIR)\llex.obj" \
	"$(INTDIR)\lmem.obj" \
	"$(INTDIR)\lobject.obj" \
	"$(INTDIR)\lparser.obj" \
	"$(INTDIR)\lstate.obj" \
	"$(INTDIR)\lstring.obj" \
	"$(INTDIR)\ltable.obj" \
	"$(INTDIR)\ltests.obj" \
	"$(INTDIR)\ltm.obj" \
	"$(INTDIR)\lua.obj" \
	"$(INTDIR)\luac.obj" \
	"$(INTDIR)\LuaExtend.obj" \
	"$(INTDIR)\lundump.obj" \
	"$(INTDIR)\lvm.obj" \
	"$(INTDIR)\lzio.obj" \
	"$(INTDIR)\opt.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\lauxlib.obj" \
	"$(INTDIR)\lbaselib.obj" \
	"$(INTDIR)\ldblib.obj" \
	"$(INTDIR)\liolib.obj" \
	"$(INTDIR)\lmathlib.obj" \
	"$(INTDIR)\lstrlib.obj"

"$(OUTDIR)\LuaLibDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# 开始自定义宏
OutDir=.\Debug
# 结束自定义宏

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\LuaLibDll.dll"
   copy debug\lualibdll.lib ..\..\..\Lib\lualibdll.lib
	copy debug\lualibdll.dll ..\..\..\Lib\lualibdll.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LuaLibDll.dep")
!INCLUDE "LuaLibDll.dep"
!ELSE 
!MESSAGE Warning: cannot find "LuaLibDll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LuaLibDll - Win32 Release" || "$(CFG)" == "LuaLibDll - Win32 Debug"
SOURCE=.\src\dump.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\dump.obj"	"$(INTDIR)\dump.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\dump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lapi.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lapi.obj"	"$(INTDIR)\lapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lapi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lcode.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lcode.obj"	"$(INTDIR)\lcode.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lcode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\ldebug.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ldebug.obj"	"$(INTDIR)\ldebug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ldebug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\ldo.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ldo.obj"	"$(INTDIR)\ldo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ldo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lfunc.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lfunc.obj"	"$(INTDIR)\lfunc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lfunc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lgc.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lgc.obj"	"$(INTDIR)\lgc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lgc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\llex.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\llex.obj"	"$(INTDIR)\llex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\llex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lmem.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lmem.obj"	"$(INTDIR)\lmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lobject.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lobject.obj"	"$(INTDIR)\lobject.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lobject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lparser.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lparser.obj"	"$(INTDIR)\lparser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lstate.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lstate.obj"	"$(INTDIR)\lstate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lstate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lstring.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lstring.obj"	"$(INTDIR)\lstring.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lstring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\ltable.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ltable.obj"	"$(INTDIR)\ltable.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ltable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\ltests.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ltests.obj"	"$(INTDIR)\ltests.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ltests.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\ltm.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ltm.obj"	"$(INTDIR)\ltm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ltm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lua.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lua.obj"	"$(INTDIR)\lua.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lua.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\luac.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\luac.obj"	"$(INTDIR)\luac.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\luac.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\LuaExtend.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\LuaExtend.obj"	"$(INTDIR)\LuaExtend.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\LuaExtend.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lundump.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lundump.obj"	"$(INTDIR)\lundump.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lundump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lvm.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lvm.obj"	"$(INTDIR)\lvm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lvm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\lzio.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lzio.obj"	"$(INTDIR)\lzio.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lzio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\opt.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\opt.obj"	"$(INTDIR)\opt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\opt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\print.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\print.obj"	"$(INTDIR)\print.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\print.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\lauxlib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lauxlib.obj"	"$(INTDIR)\lauxlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lauxlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\lbaselib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lbaselib.obj"	"$(INTDIR)\lbaselib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lbaselib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\ldblib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\ldblib.obj"	"$(INTDIR)\ldblib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\ldblib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\liolib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\liolib.obj"	"$(INTDIR)\liolib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\liolib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\lmathlib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lmathlib.obj"	"$(INTDIR)\lmathlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lmathlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\baselib\lstrlib.c

!IF  "$(CFG)" == "LuaLibDll - Win32 Release"


"$(INTDIR)\lstrlib.obj"	"$(INTDIR)\lstrlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "LuaLibDll - Win32 Debug"


"$(INTDIR)\lstrlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

