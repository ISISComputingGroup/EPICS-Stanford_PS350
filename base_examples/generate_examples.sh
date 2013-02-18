#!/bin/sh
set -o errexit

EPICS_BASE="$1"
MAKEBASEAPP="${EPICS_BASE}/bin/${EPICS_HOST_ARCH}/makeBaseApp.pl"
perl "$MAKEBASEAPP" -t example example
perl "$MAKEBASEAPP" -i -p example -t example -a ${EPICS_HOST_ARCH} example

# Generate Channel Access Client example
perl "$MAKEBASEAPP" -t caClient caClient
