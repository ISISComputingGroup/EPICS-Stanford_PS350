REM Argument 1 = directory to link to (e.g. %WORKSPACE%\EPICS)
REM Argument 2 = name for zip file (e.g. EPICS_win7_x64.zip)
REM Argument 3 = architecture (defaults to x64)
REM Argument 4 = static (defaults to shared)

REM Example Jenkins build commands: 

REM cd %WORKSPACE%\EPICS
REM call jenkins_build.bat %WORKSPACE%\EPICS EPICS_win7_x64.zip

REM cd %WORKSPACE%\EPICS
REM call jenkins_build.bat %WORKSPACE%\EPICS EPICS_STATIC_win7_x64.zip windows-x64 static

cd %1
mklink /d C:\EPICS %1

set arch_type=windows-x64
set build_type=shared

if NOT "%3" == "" (
    set arch_type=%3
)
if NOT "%4" == "" (
    set build_type=%4
)

cd c:\EPICS
call build.bat arch_type build_type

C:\"Program Files"\7-Zip\7z.exe a -xr!.svn -xr!CSS -xr!pilot -xr!src %2 *
move /Y %2 c:\Installers\EPICS\.

rd C:\EPICS
