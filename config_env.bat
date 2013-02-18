set MYDIR=%~dp0

set BASE_VERSION=3.14.12.2
set EPICS_BASE=%MYDIR%base\%BASE_VERSION%

call %EPICS_BASE%\startup\win32.bat

REM utils (make, sed etc.)
set PATH=%MYDIR%utils_win32;c:\strawberry\perl\bin;%PATH%

REM epics base and extensions
set PATH=%EPICS_BASE%\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%extensions\bin\%EPICS_HOST_ARCH%;%PATH%

REM various things in modules\soft
set PATH=%MYDIR%support\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\sscan\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\motor\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\calc\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\asyn\bin\%EPICS_HOST_ARCH%;%PATH%

REM POCO 
set PATH=%MYDIR%Third_Party\POCO\bin_x64;%PATH%

REM create include for path to EPICS_BASE
echo EPICS_BASE=%EPICS_BASE%> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
echo SUPPORT=%MYDIR%support>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
sed -i -e "s=\\=/=g" %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
