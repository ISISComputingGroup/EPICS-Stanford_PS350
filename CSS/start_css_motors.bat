@echo off
set MYLOCDIR=%~dp0
set CSS_NAME=css-win.x86_64
set CSS_DIR=%MYLOCDIR%%CSS_NAME%
set CSS_SHARE=%MYLOCDIR%Share
REM this defines MYDIR
call %MYLOCDIR%..\config_env.bat
start /min caRepeater.exe
@echo on
cd /d %MYLOCDIR%Workspaces
%CSS_DIR%\CSS\css.exe -pluginCustomization %MYLOCDIR%Config\settings.ini -share_link %CSS_SHARE%=/CSS/Share -data %MYLOCDIR%Workspaces\motors --launcher.openFile "%MYLOCDIR%\Share\motor_opi\topMotors4.opi"
