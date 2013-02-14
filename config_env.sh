#!/bin/sh
mydir="`pwd`"
epics_base_path="${mydir}/base"
export MY_EPICS_BASE="${epics_base_path}"
. base/startup/Site.profile
set_epics_base="EPICS_BASE=${epics_base_path}"
echo "${set_epics_base}" > EPICS_BASE.${EPICS_HOST_ARCH}
