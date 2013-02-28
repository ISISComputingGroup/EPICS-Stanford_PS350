@echo off
set MYDIR=%~dp0
set CSS_DIR=c:\isis_css
md %CSS_DIR%
cd /d %CSS_DIR%
unzip -q %MYDIR%Binaries\css-win.x86_64.zip
copy /y %MYDIR%\Config\settings.ini .
echo %CSS_DIR%\CSS\css.exe -pluginCustomization %CSS_DIR%\CSS\settings.ini > css.bat
