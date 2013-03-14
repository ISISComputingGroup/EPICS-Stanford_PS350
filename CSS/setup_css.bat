@echo off
set MYLOCDIR=%~dp0
set CSS_NAME=css-win.x86_64
REM this defines MYDIR
call %MYLOCDIR%..\config_env.bat
md %MYLOCDIR%Workspaces
md %MYLOCDIR%%CSS_NAME%
cd /d %MYLOCDIR%%CSS_NAME%
unzip -q %MYLOCDIR%Binaries\%CSS_NAME%.zip
