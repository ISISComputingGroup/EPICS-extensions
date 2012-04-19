#!/bin/sh
set -o errexit

MAKEBASEAPP=../base/bin/${EPICS_HOST_ARCH}/makeBaseApp.pl
perl $MAKEBASEAPP -t example example
perl $MAKEBASEAPP -i -p example -t example example

# Generate Channel Access Client example
perl $MAKEBASEAPP -t caClient caClient
