Building on Windows
-------------------

* Initial Setup

You need to have perl installed - either strawberry perl (http://strawberryperl.com/) or active state perl (http://www.activestate.com/activeperl/downloads)

edit base/startup/win32.bat 
- update the line "ActiveState perl" to include where perl is insralled e.g.

  set PATH=c:\strawberry\perl\bin;%PATH%

- update the line "gnuwin32 make" and change to point to the checked out "utils_win32" directory e.g.

  set PATH=c:\development\EPICS\utils_win32;%PATH%

- update the line "Visual Studio 2010" 
  
  uncomment "window-x64" line and comment out "win32-x86" line

- update the line "--- EPICS ---"
  
  use "set EPICS_HOST_ARCH=windows-x64"  rather than win32-x86
  update the other   set PATH   lines to point to your epics checkout location

* Building base

- Open a visual studio win x64 win64 command console
- cd to the "base" checked out epics distribution
- type    startup/win32.bat    to set your environment
- type    make

* build examples

- cd base_examples
- run   generate_examples.bat
- make

* building in general

- Open a visual studio win x64 win64 command console and run "base/startup/win32.bat" before running make
- type "make clean uninstall" to remove build files

----
Freddie Akeroyd, 03/01/2012