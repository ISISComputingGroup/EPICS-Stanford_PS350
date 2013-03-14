#!/bin/sh
SCRIPT=$(readlink -f ${BASH_SOURCE[0]})
SCRIPTPATH=`dirname "$SCRIPT"`
mydir="$SCRIPTPATH"
export EPICS_BASE_VERSION="3-14-12-2"
epics_base_path="${mydir}/base/${EPICS_BASE_VERSION}"
export MY_EPICS_BASE="${epics_base_path}"
. ${mydir}/base/${EPICS_BASE_VERSION}/startup/Site.profile

SHORT_HOSTNAME=`hostname -s`
export MYPVPREFIX="${SHORT_HOSTNAME}:${USER}:"

# create include for path to EPICS_BASE
set_epics_base="EPICS_BASE=${epics_base_path}"
echo "${set_epics_base}" > ISIS_CONFIG.${EPICS_HOST_ARCH}
echo "SUPPORT=${mydir}/support" >> ISIS_CONFIG.${EPICS_HOST_ARCH}
# echo "INSTALL_LOCATION$=${mydir}/install" >> ISIS_CONFIG.${EPICS_HOST_ARCH}
# echo "INSTALL_LOCATION_APP=${mydir}/install" >> ISIS_CONFIG.${EPICS_HOST_ARCH}

# epics base and extensions
export PATH=${epics_base_path}/bin/${EPICS_HOST_ARCH}:${PATH}
export PATH=${mydir}/extensions/bin/${EPICS_HOST_ARCH}:${PATH}

# various things in modules\soft
#for m in . sscan motor calc asyn; do 
#    export PATH=${mydir}/support/${m}/bin/${EPICS_HOST_ARCH}:${PATH}
#done
#export LD_LIBRARY_PATH=${epics_base_path}/lib/${EPICS_HOST_ARCH}:${LD_LIBRARY_PATH}
sed -e "s=MYPVPREFIX=%MYPVPREFIX%=g" ${mydir}/CSS/Config/settings.ini.in > ${mydir}/CSS/Config/settings.ini
