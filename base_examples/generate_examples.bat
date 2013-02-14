REM Generate IOC example
makeBaseApp.pl -t example example
makeBaseApp.pl -i -p example -t example example

REM Generate Channel Access Client example
makeBaseApp.pl -t caClient caClient
