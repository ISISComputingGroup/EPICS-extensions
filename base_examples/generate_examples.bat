REM Generate IOC example
makeBaseApp.pl -t example example
makeBaseApp.pl -i -p example -t example -a %EPICS_HOST_ARCH% example

REM Generate Channel Access Client example
makeBaseApp.pl -t caClient caClient
