# Microsoft Developer Studio Project File - Name="VirtuaNES" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VirtuaNES - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VirtuaNES.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VirtuaNES.mak" CFG="VirtuaNES - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VirtuaNES - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VirtuaNES - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "VirtuaNES - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE "VirtuaNES - Win32 Release_Debugout" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "Zlib" /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\ApuEx\emu2413" /I "NES\PadEx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /i "res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib shlwapi.lib /nologo /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\ApuEx\emu2413" /I "NES\PadEx" /I "Zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib shlwapi.lib libcmtd.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"./output/VirtuaNES.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VirtuaNES___Win32_Profile"
# PROP BASE Intermediate_Dir "VirtuaNES___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\PadEx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /O2 /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\ApuEx\emu2413" /I "NES\PadEx" /I "Zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib shlwapi.lib /nologo /subsystem:windows /profile /map:"VirtuaNES.map" /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VirtuaNES___Win32_Release_Debugout"
# PROP BASE Intermediate_Dir "VirtuaNES___Win32_Release_Debugout"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release_Debugout"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\PadEx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEBUGOUT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /O2 /I "." /I "NES" /I "NES\Mapper" /I "NES\ApuEx" /I "NES\ApuEx\emu2413" /I "NES\PadEx" /I "Zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEBUGOUT" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib imm32.lib dinput.lib shlwapi.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"./output/VirtuaNES.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "VirtuaNES - Win32 Release"
# Name "VirtuaNES - Win32 Debug"
# Name "VirtuaNES - Win32 Profile"
# Name "VirtuaNES - Win32 Release_Debugout"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App.cpp
# End Source File
# Begin Source File

SOURCE=.\Archive.cpp
# End Source File
# Begin Source File

SOURCE=.\Com.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\Crclib.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugOut.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectSound.cpp
# End Source File
# Begin Source File

SOURCE=.\EmuThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Pathlib.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\Recent.cpp
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\RomInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VirtuaNES.rc
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Wnd.cpp
# End Source File
# Begin Source File

SOURCE=.\WndHook.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\Archive.h
# End Source File
# Begin Source File

SOURCE=.\Com.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\Crclib.h
# End Source File
# Begin Source File

SOURCE=.\DebugOut.h
# End Source File
# Begin Source File

SOURCE=.\DirectDraw.h
# End Source File
# Begin Source File

SOURCE=.\DirectInput.h
# End Source File
# Begin Source File

SOURCE=.\DirectSound.h
# End Source File
# Begin Source File

SOURCE=.\EmuThread.h
# End Source File
# Begin Source File

SOURCE=.\GameOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\hq2x.h
# End Source File
# Begin Source File

SOURCE=.\interp.h
# End Source File
# Begin Source File

SOURCE=.\lq2x.h
# End Source File
# Begin Source File

SOURCE=.\lzAscii.h
# End Source File
# Begin Source File

SOURCE=.\lzSight.h
# End Source File
# Begin Source File

SOURCE=.\lzTVlayer.h
# End Source File
# Begin Source File

SOURCE=.\Macro.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\Pathlib.h
# End Source File
# Begin Source File

SOURCE=.\Plugin.h
# End Source File
# Begin Source File

SOURCE=.\Recent.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RomInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\Typedef.h
# End Source File
# Begin Source File

SOURCE=.\VirtuaNESres.h
# End Source File
# Begin Source File

SOURCE=.\Wnd.h
# End Source File
# Begin Source File

SOURCE=.\WndHook.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\CheatImageList.bmp
# End Source File
# Begin Source File

SOURCE=.\res\header_down.ico
# End Source File
# Begin Source File

SOURCE=.\res\header_up.ico
# End Source File
# Begin Source File

SOURCE=.\LauncherImageList.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VirtuaNES.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\res\VirtuaNES.ico
# End Source File
# End Group
# Begin Group "NES"

# PROP Default_Filter ""
# Begin Group "Mapper"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NES\EEPROM.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper.h
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper000.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper000.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper001.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper001.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper002.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper002.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper003.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper003.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper004.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper004.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper005.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper005.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper006.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper006.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper007.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper007.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper008.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper008.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper009.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper009.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper010.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper010.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper011.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper011.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper012.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper012.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper013.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper013.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper015.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper015.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper016.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper016.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper017.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper017.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper018.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper018.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper019.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper019.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper021.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper021.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper022.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper022.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper023.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper023.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper024.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper024.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper025.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper025.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper026.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper026.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper027.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper027.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper032.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper032.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper033.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper033.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper034.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper034.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper040.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper040.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper041.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper041.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper042.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper042.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper043.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper043.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper044.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper044.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper045.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper045.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper046.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper046.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper047.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper047.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper048.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper048.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper050.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper050.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper051.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper051.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper057.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper057.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper058.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper058.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper060.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper060.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper061.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper061.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper062.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper062.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper064.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper064.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper065.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper065.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper066.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper066.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper067.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper067.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper068.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper068.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper069.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper069.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper070.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper070.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper071.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper071.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper072.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper072.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper073.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper073.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper074.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper074.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper075.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper075.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper076.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper076.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper077.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper077.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper078.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper078.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper079.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper079.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper080.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper080.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper082.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper082.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper083.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper083.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper085.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper085.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper086.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper086.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper087.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper087.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper088.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper088.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper089.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper089.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper090.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper090.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper091.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper091.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper092.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper092.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper093.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper093.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper094.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper094.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper095.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper095.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper096.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper096.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper097.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper097.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper099.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper099.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper100.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper100.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper101.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper101.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper105.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper105.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper107.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper107.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper108.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper108.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper109.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper109.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper110.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper110.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper112.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper112.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper113.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper113.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper114.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper114.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper115.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper115.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper116.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper116.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper117.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper117.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper118.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper118.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper119.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper119.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper122.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper122.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper133.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper133.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper134.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper134.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper135.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper135.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper140.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper140.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper142.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper142.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper151.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper151.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper160.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper160.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper164.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper164.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper165.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper165.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper167.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper167.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper180.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper180.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper181.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper181.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper182.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper182.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper183.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper183.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper185.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper185.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper187.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper187.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper188.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper188.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper189.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper189.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\Mapper190.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper190.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper191.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper191.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper193.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper193.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper194.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper194.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper198.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper198.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper200.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper200.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper201.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper201.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper202.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper202.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper222.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper222.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper225.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper225.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper226.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper226.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper227.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper227.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper228.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper228.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper229.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper229.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper230.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper230.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper231.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper231.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper232.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper232.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper233.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper233.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper234.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper234.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper235.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper235.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper236.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper236.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper240.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper240.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper241.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper241.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper242.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper242.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper243.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper243.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper244.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper244.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper245.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper245.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper246.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper246.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper248.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper248.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper249.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper249.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper251.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper251.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper252.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper252.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper254.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper254.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper255.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\Mapper255.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NES\MapperFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\MapperFDS.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\MapperFDS.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\MapperNSF.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\MapperNSF.h

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ApuEx"

# PROP Default_Filter ""
# Begin Group "emu2413"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NES\2413tone.h
# End Source File
# Begin Source File

SOURCE=.\NES\emu2413.c
# End Source File
# Begin Source File

SOURCE=.\NES\emu2413.h
# End Source File
# Begin Source File

SOURCE=.\NES\vrc7tone.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\NES\APU_FDS.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_FDS.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_FME7.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_FME7.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_INTERFACE.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_INTERNAL.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_INTERNAL.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_MMC5.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_MMC5.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_N106.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_N106.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_VRC6.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_VRC6.h
# End Source File
# Begin Source File

SOURCE=.\NES\APU_VRC7.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU_VRC7.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\NES\APU.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\APU.h
# End Source File
# Begin Source File

SOURCE=.\NES\Cpu.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\CPU.h
# End Source File
# Begin Source File

SOURCE=.\NES\MMU.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\MMU.h
# End Source File
# Begin Source File

SOURCE=.\NES\Nes.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\Nes.h
# End Source File
# Begin Source File

SOURCE=.\NES\PAD.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\PAD.h
# End Source File
# Begin Source File

SOURCE=.\NES\PPU.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\PPU.h
# End Source File
# Begin Source File

SOURCE=.\NES\ROM.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\ROM.h
# End Source File
# Begin Source File

SOURCE=.\NES\ROM_Patch.cpp

!IF  "$(CFG)" == "VirtuaNES - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VirtuaNES - Win32 Release_Debugout"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NES\State.h
# End Source File
# Begin Source File

SOURCE=.\NES\VsUnisystem.cpp
# End Source File
# Begin Source File

SOURCE=.\NES\VsUnisystem.h
# End Source File
# End Group
# Begin Group "Zlib"

# PROP Default_Filter "*.c *.cpp *.h"
# Begin Source File

SOURCE=.\zlib\unzip.c
# End Source File
# Begin Source File

SOURCE=.\zlib\unzip.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zlib.lib
# End Source File
# End Group
# End Target
# End Project
