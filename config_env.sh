#!/bin/sh
mydir="`pwd`"
epics_base_path="${mydir}/base"
export MY_EPICS_BASE="${epics_base_path}"
. base/startup/Site.profile

# create include for path to EPICS_BASE
set_epics_base="EPICS_BASE=${epics_base_path}"
echo "${set_epics_base}" > EPICS_BASE.${EPICS_HOST_ARCH}

echo "MODULES_SOFT=${mydir}/modules/soft" >> EPICS_BASE.${EPICS_HOST_ARCH}

# epics base and extensions
export PATH=${mydir}/base/bin/${EPICS_HOST_ARCH}:${PATH}
export PATH=${mydir}/extensions/bin/${EPICS_HOST_ARCH}:${PATH}

# various things in modules\soft
for m in . sscan motor calc asyn; do 
    export PATH=${mydir}/modules/soft/${m}/bin/${EPICS_HOST_ARCH}:${PATH}
done
