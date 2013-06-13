@echo off

REM usage is  config_env arch build_type
REM arch is e.g. windows-x64 or win32-x86 (default: whatever last specified, or windows-x64 on first run)
REM build_type is static or shared (default: whatever last specified, or shared)

set MYDIR=%~dp0

set KIT_ROOT=%MYDIR%

set EPICS_BASE_VERSION=3-14-12-2
set EPICS_BASE=%MYDIR%base\%EPICS_BASE_VERSION%

set EPICS_HOST_ARCH=windows-x64
set MY_BUILD_TYPE=shared
if exist "%MYDIR%epics_host_arch.txt" (
    for /f %%i in ( %MYDIR%epics_host_arch.txt ) do set EPICS_HOST_ARCH=%%i
)
if exist "%MYDIR%build_type.txt" (
    for /f %%i in ( %MYDIR%build_type.txt ) do set MY_BUILD_TYPE=%%i
)
if NOT "%1" == "" (
    set EPICS_HOST_ARCH=%1
)
if NOT "%2" == "" (
    set MY_BUILD_TYPE=%2
)
echo %EPICS_HOST_ARCH%> "%MYDIR%epics_host_arch.txt"
echo %MY_BUILD_TYPE%> "%MYDIR%build_type.txt"

set MYPVPREFIX=%COMPUTERNAME%:%USERNAME%:

@echo ### $Id$ ###
@echo Using EPICS base %EPICS_BASE_VERSION% for %EPICS_HOST_ARCH% (%MY_BUILD_TYPE%)
@echo Setting PV prefix to "%MYPVPREFIX%"

call %EPICS_BASE%\startup\win32.bat

REM utils (make, sed etc.)
set PATH=%MYDIR%utils_win32;%PATH%

REM perl
if exist "c:\strawberry\perl\bin\perl.exe" set PATH=c:\strawberry\perl\bin;%PATH%
if exist "c:\strawberry\perl\perl\bin\perl.exe" set PATH=c:\strawberry\perl\perl\bin;%PATH%

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
set PATH=%MYDIR%support\autosave\R5_0\bin\%EPICS_HOST_ARCH%;%PATH%

REM POCO 
REM set PATH=%MYDIR%Third_Party\POCO\bin_x64;%PATH%

REM create include for path to EPICS_BASE
echo EPICS_BASE=%EPICS_BASE%> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
echo SUPPORT=%MYDIR%support>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
echo ISISSUPPORT=%MYDIR%ISIS>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%

REM we only have ATL is we use full Visual studio, not express
if exist "%VCINSTALLDIR%atlmfc\include" echo HAVE_ATL=YES>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%

REM echo INSTALL_LOCATION=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM echo INSTALL_LOCATION_APP=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM IOCS_APPL_TOP
copy /y %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH% %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%.bak
sed -e "s=\\=/=g" %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%.bak > %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
sed -e "s=MYPVPREFIX=%MYPVPREFIX%=g" %MYDIR%CSS\Config\settings.ini.in > %MYDIR%CSS\Config\settings.ini

if "%MY_BUILD_TYPE%" == "static" (
    echo SHARED_LIBRARIES=NO> %EPICS_BASE%\configure\CONFIG_SITE_ISIS
    echo STATIC_BUILD=YES>> %EPICS_BASE%\configure\CONFIG_SITE_ISIS
) else (
    echo SHARED_LIBRARIES=YES> %EPICS_BASE%\configure\CONFIG_SITE_ISIS
    echo STATIC_BUILD=NO>> %EPICS_BASE%\configure\CONFIG_SITE_ISIS
)

