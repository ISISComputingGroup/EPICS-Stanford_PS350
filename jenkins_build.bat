REM Argument 1 = directory to link to (e.g. %WORKSPACE%\EPICS)
REM Argument 2 = name for zip file (e.g. EPICS_win7_x64.zip)
REM Argument 3 = architecture (defaults to x64)
REM Argument 4 = static (defaults to not static)
REM For example, in Jenkins the build command might be: call jenkins_build.bat %WORKSPACE%\EPICS EPICS_win7_x64.zip

cd %1
mklink /d C:\EPICS %1

cd c:\EPICS
call build.bat %3 %4

C:\"Program Files"\7-Zip\7z.exe a -xr!.svn -xr!CSS -xr!pilot -xr!src %2 *
move /Y %2 c:\Installers\EPICS\.

rd C:\EPICS
