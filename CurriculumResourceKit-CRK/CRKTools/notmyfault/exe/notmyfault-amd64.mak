# Microsoft Developer Studio Generated NMAKE File, Based on NotMyfault.dsp
!IF "$(CFG)" == ""
CFG=Release
!MESSAGE No configuration specified. Defaulting to Amd64 Release.
!ENDIF 

!IF "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NotMyfault.mak" CFG="Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NotMyfault - AMD64 Release"
!MESSAGE "NotMyfault - AMD64 Debug"
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Release"

OUTDIR=.\RelAmd
INTDIR=.\RelAmd
# Begin Custom Macros
OutDir=.\RelAmd
# End Custom Macros

ALL : "$(OUTDIR)\NotMyfault.exe"


CLEAN :
	-@erase "$(INTDIR)\driver.obj"
	-@erase "$(INTDIR)\notmyfault.obj"
	-@erase "$(INTDIR)\notmyfault.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\NotMyfault.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /W3 /WX /EHsc /O2 /D "NDEBUG" /D "Amd64" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /Amd64
RSC=rc.exe
RSC_PROJ=/l 0x409 /D "AMD64" /fo"$(INTDIR)\notmyfault.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NotMyfault.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\NotMyfault.pdb" /machine:AMD64 /out:"$(OUTDIR)\NotMyfault.exe" 
LINK32_OBJS= \
	"$(INTDIR)\driver.obj" \
	"$(INTDIR)\notmyfault.obj" \
	"$(INTDIR)\notmyfault.res"

"$(OUTDIR)\NotMyfault.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Debug"

OUTDIR=.\DebAmd
INTDIR=.\DebAmd
# Begin Custom Macros
OutDir=.\DebAmd
# End Custom Macros

ALL : "$(OUTDIR)\NotMyfault.exe"


CLEAN :
	-@erase "$(INTDIR)\driver.obj"
	-@erase "$(INTDIR)\notmyfault.obj"
	-@erase "$(INTDIR)\notmyfault.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\NotMyfault.exe"
	-@erase "$(OUTDIR)\NotMyfault.ilk"
	-@erase "$(OUTDIR)\NotMyfault.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /WX /EHsc /Zi /Od /D "WIN32" /D "_DEBUG" /D "Amd64" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 /Amd64
RSC=rc.exe
RSC_PROJ=/l 0x409 /D "AMD64" /fo"$(INTDIR)\notmyfault.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NotMyfault.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\NotMyfault.pdb" /debug /machine:AMD64 /out:"$(OUTDIR)\NotMyfault.exe" 
LINK32_OBJS= \
	"$(INTDIR)\driver.obj" \
	"$(INTDIR)\notmyfault.obj" \
	"$(INTDIR)\notmyfault.res"

"$(OUTDIR)\NotMyfault.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("NotMyfault.dep")
!INCLUDE "NotMyfault.dep"
!ELSE 
!MESSAGE Warning: cannot find "NotMyfault.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Release" || "$(CFG)" == "Debug"
SOURCE=.\driver.c

"$(INTDIR)\driver.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\notmyfault.c

"$(INTDIR)\notmyfault.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\notmyfault.rc

"$(INTDIR)\notmyfault.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

