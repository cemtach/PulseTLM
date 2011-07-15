#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case08


#
# Compare current results against the regression data
#

${DIFF} case08.log ./compare/case08.log
${DIFF} case08_e0.dat ./compare/case08_e0.dat
