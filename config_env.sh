#!/bin/sh
mydir="`pwd`"
base_version="3.14.12.2"
epics_base_path="${mydir}/base/${base_version}"
export MY_EPICS_BASE="${epics_base_path}"
. base/${base_version}/startup/Site.profile

# create include for path to EPICS_BASE
set_epics_base="EPICS_BASE=${epics_base_path}"
echo "${set_epics_base}" > ISIS_CONFIG.${EPICS_HOST_ARCH}

echo "SUPPORT=${mydir}/support" >> ISIS_CONFIG.${EPICS_HOST_ARCH}

# epics base and extensions
export PATH=${epics_base_path}/bin/${EPICS_HOST_ARCH}:${PATH}
export PATH=${mydir}/extensions/bin/${EPICS_HOST_ARCH}:${PATH}

# various things in modules\soft
#for m in . sscan motor calc asyn; do 
#    export PATH=${mydir}/support/${m}/bin/${EPICS_HOST_ARCH}:${PATH}
#done
#export LD_LIBRARY_PATH=${epics_base_path}/lib/${EPICS_HOST_ARCH}:${LD_LIBRARY_PATH}
