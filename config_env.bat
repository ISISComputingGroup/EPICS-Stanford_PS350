@echo off

set MYDIR=%~dp0

set KIT_ROOT=%MYDIR%

set EPICS_BASE_VERSION=3-14-12-2
set EPICS_BASE=%MYDIR%base\%EPICS_BASE_VERSION%

if "%1" == "" (
    if exist "%MYDIR%DEFAULT_HOST_ARCH.txt" (
        for /f %%i in ( %MYDIR%DEFAULT_HOST_ARCH.txt ) do set EPICS_HOST_ARCH=%%i
    ) else (
        set EPICS_HOST_ARCH=windows-x64
    )
) else (
    set EPICS_HOST_ARCH=%1
)

@echo %EPICS_HOST_ARCH%> "%MYDIR%DEFAULT_HOST_ARCH.txt"

set MYPVPREFIX=%COMPUTERNAME%:%USERNAME%:

@echo ### $Id$ ###
@echo Using EPICS base %EPICS_BASE_VERSION% for %EPICS_HOST_ARCH% and setting PV prefix to "%MYPVPREFIX%"

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

REM we only have ATL is we use full Visual studio, not express
if exist "%VCINSTALLDIR%atlmfc\include" echo HAVE_ATL=YES>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%

REM echo INSTALL_LOCATION=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM echo INSTALL_LOCATION_APP=%MYDIR%install>> %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
REM IOCS_APPL_TOP
copy /y %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH% %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%.bak
sed -e "s=\\=/=g" %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%.bak > %MYDIR%ISIS_CONFIG.%EPICS_HOST_ARCH%
sed -e "s=MYPVPREFIX=%MYPVPREFIX%=g" %MYDIR%CSS\Config\settings.ini.in > %MYDIR%CSS\Config\settings.ini
