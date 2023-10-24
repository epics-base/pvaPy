#!/bin/bash
#
# Perl installation fix
#

echo "*** Start of post-link.sh ***"

echo "Fixing Conda's Perl installation"
perl $PREFIX/opt/epics/startup/fix-perl.pl

echo "*** End of post-link.sh ***"

