set MYDIR=%~dp0

call %MYDIR%base\startup\win32.bat

set PATH=%MYDIR%utils_win32;%PATH%
set PATH=%MYDIR%base\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%extensions\bin\%EPICS_HOST_ARCH%;%PATH%

echo EPICS_BASE=%MYDIR%base> %MYDIR%EPICS_BASE.%EPICS_HOST_ARCH%
sed -i -e "s!\\!/!g" %MYDIR%EPICS_BASE.%EPICS_HOST_ARCH%