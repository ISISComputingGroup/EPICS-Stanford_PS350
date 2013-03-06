#!/bin/sh
MYLOCDIR=`pwd`
CSS_NAME=css-linux.x86_64
set CSS_DIR=${MYLOCDIR}/${CSS_NAME}
set CSS_SHARE=${MYLOCDIR}/Share
REM this defines MYDIR
. ${MYLOCDIR}\..\config_env.sh
start /min caRepeater.exe
cd ${MYLOCDIR}/Workspaces
${CSS_DIR}/CSS/css -pluginCustomization ${MYLOCDIR}/Config/settings.ini -share_link ${CSS_SHARE}=/CSS/Share -data ${MYLOCDIR}/Workspaces/main
