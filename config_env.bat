set MYDIR=%~dp0

call %MYDIR%base\startup\win32.bat

REM utils (make, sed etc.)
set PATH=%MYDIR%utils_win32;%PATH%

REM epics base and extensions
set PATH=%MYDIR%base\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%extensions\bin\%EPICS_HOST_ARCH%;%PATH%

REM various things in modules\soft
set PATH=%MYDIR%modules\soft\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%modules\soft\sscan\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%modules\soft\motor\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%modules\soft\calc\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%modules\soft\asyn\bin\%EPICS_HOST_ARCH%;%PATH%

REM POCO 
set PATH=%MYDIR%Third_Party\POCO\bin_x64;%PATH%

REM create include for path to EPICS_BASE
echo EPICS_BASE=%MYDIR%base> %MYDIR%EPICS_BASE.%EPICS_HOST_ARCH%
sed -i -e "s=\\=/=g" %MYDIR%EPICS_BASE.%EPICS_HOST_ARCH%