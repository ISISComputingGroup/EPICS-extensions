#!/bin/sh
set -o errexit
mydir="`pwd`"
#. build_base.sh
cd "$mydir"
export MY_EPICS_BASE="${mydir}/base"
export EPICS_BASE_PVT="${mydir}/base"
. ${MY_EPICS_BASE}/startup/Site.profile
#
for i in modules/soft; do
    echo "EPICS_BASE=${MY_EPICS_BASE}" > "$i/configure/EPICS_BASE.${EPICS_HOST_ARCH}"
done
#echo 'EPICS_BASE=$(TOP)/../../base' > "modules/soft/configure/EPICS_BASE.${EPICS_HOST_ARCH}"
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
