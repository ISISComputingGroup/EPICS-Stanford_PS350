REM Generate IOC example
set EPICS_BASE=%1
makeBaseApp.pl -t example example
makeBaseApp.pl -i -p example -t example -a %EPICS_HOST_ARCH% example

REM Generate Channel Access Client example
makeBaseApp.pl -t caClient caClient
