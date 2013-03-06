#!/bin/sh
MYDIR=`pwd`
CSS_NAME=css-linux.x86_64
mkdir ${MYDIR}/Workspaces
mkdir ${MYDIR}/${CSS_NAME}
cd ${MYDIR}/${CSS_NAME}
unzip -q ${MYDIR}/Binaries/${CSS_NAME}.zip
