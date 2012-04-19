#!/bin/sh
set -o errexit
mydir="`pwd`"
export MY_EPICS_BASE="${mydir}/base"
export EPICS_BASE_PVT="${mydir}/base"
#
cd base
. ${MY_EPICS_BASE}/startup/Site.profile
make
cd "$mydir"
#
for i in modules/soft; do
    echo "EPICS_BASE=${MY_EPICS_BASE}" > "$i/configure/EPICS_BASE.${EPICS_HOST_ARCH}"
done
#
cd base_examples
sh generate_examples.sh
make clean
make
cd ..
#
cd modules/soft
make clean
for i in asyn pcreApp StreamDevice; do 
 ( cd $i; make clean; make )
done
make
