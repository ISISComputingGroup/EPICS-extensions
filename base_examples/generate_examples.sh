#!/bin/sh
set -o errexit

makeBaseApp.pl -t example example
makeBaseApp.pl -i -p example -t example example

# Generate Channel Access Client example
makeBaseApp.pl -t caClient caClient
