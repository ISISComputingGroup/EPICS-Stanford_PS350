#!/bin/sh
set -o errexit
. ./config_env.sh
make $*
cd ISIS/lvDCOM
doxygen Doxyfile
if test -d /isis/www/EPICS/lvDCOM; then
    rm -fr /isis/www/EPICS/lvDCOM/html
    cp -r doc/html /isis/www/EPICS/lvDCOM
fi
