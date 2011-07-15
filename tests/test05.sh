#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case05


#
# Compare current results against the regression data
#

${DIFF} case05.log ./compare/case05.log
${DIFF} case05_o0.dat ./compare/case05_o0.dat
${DIFF} case05_o1.dat ./compare/case05_o1.dat
${DIFF} case05_e0.dat ./compare/case05_e0.dat

