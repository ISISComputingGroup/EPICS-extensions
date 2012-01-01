Windows

install strawberry perl
Open a visual studio win x64 win64 command console

edit startup/win32.bat 
- line "ActiveState perl" update to include where strawbetter perl is e.f

set PATH=c:\strawberry\perl\bin;%PATH%

- line "gnuwin32 make" change to utils_win32 directory

set PATH=c:\development\EPICS\utils_win32;%PATH%

- line "Visual Studio 2010" 
uncomment "window-x64" line and comment out "win32-x86" line

line "--- EPICS ---"
set EPICS_HOST_ARCH=windows-x86   rather than win32-x86
update the set PATH lines to include the stuff


make

cd base_examples
generate_examples.bat


subsequently

Open a visual studio win x64 win64 command console
run startup/win32.bat