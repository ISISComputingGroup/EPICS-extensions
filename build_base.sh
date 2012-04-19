#!/bin/sh
set -o errexit
export MY_EPICS_BASE=.
cd base && . startup/Site.profile && make
