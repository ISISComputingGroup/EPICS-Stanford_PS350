@echo off
set MYDIR=%~dp0
set CSS_NAME=css-win.x86_64
md %MYDIR%Workspaces
md %MYDIR%%CSS_NAME%
cd /d %MYDIR%%CSS_NAME%
unzip -q %MYDIR%Binaries\%CSS_NAME%.zip
