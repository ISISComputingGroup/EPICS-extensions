#!/bin/sh
set -o errexit
. ./config_env.sh
make $*
