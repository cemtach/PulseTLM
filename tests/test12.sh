#!/bin/sh

. commands.sh

#
# Run the simulator
#

${PULSE} case12


#
# Compare current results against the regression data
#

${DIFF} case12.log ./compare/case12.log
