set MYDIR=%~dp0

set BASE_VERSION=3.14.12.2
set EPICS_BASE=%MYDIR%base\%BASE_VERSION%

call %EPICS_BASE%\startup\win32.bat

REM utils (make, sed etc.)
set PATH=%MYDIR%utils_win32;c:\strawberry\perl\bin;%PATH%

REM epics base and extensions
set PATH=%EPICS_BASE%\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%extensions\bin\%EPICS_HOST_ARCH%;%PATH%

REM various things in the support modules area
set PATH=%MYDIR%support\sscan\2-8\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\motor\6-7-1\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\calc\2-9\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\asyn\4-18\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\seq\2.1.11\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\ip\2-13\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\ipac\2.11\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\busy\1-4\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\StreamDevice\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\pcre\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\galil\1-4\bin\%EPICS_HOST_ARCH%;%PATH%
set PATH=%MYDIR%support\eurotherm2k\1-11\bin\%EPICS_HOST_ARCH%;%PATH%

REM POCO 
set PATH=%MYDIR%Third_Party\POCO\bin_x64;%PATH%

REM create include for path to EPICS_BASE
echo EPICS_BASE=%EPICS_BASE%> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
echo SUPPORT=%MYDIR%support>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM echo INSTALL_LOCATION=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM echo INSTALL_LOCATION_APP=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
sed -i -e "s=\\=/=g" %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
