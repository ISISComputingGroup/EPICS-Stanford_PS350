#!/bin/sh
SCRIPT=$(readlink -f ${BASH_SOURCE[0]})
SCRIPTPATH=`dirname "$SCRIPT"`
MYLOCDIR="$SCRIPTPATH"
CSS_NAME=css-linux.x86_64
CSS_DIR=${MYLOCDIR}/${CSS_NAME}
CSS_SHARE=${MYLOCDIR}/Share
# this defines MYDIR
. ${MYLOCDIR}/../config_env.sh
caRepeater &
cd ${MYLOCDIR}/Workspaces
${CSS_DIR}/CSS/css -pluginCustomization ${MYLOCDIR}/Config/settings.ini -share_link ${CSS_SHARE}=/CSS/Share -data ${MYLOCDIR}/Workspaces/main
