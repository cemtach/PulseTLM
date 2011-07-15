#!/bin/sh

. commands.sh

#
# Run the simulator
#

${PULSE} case01


#
# Compare current results against the regression data
#

${DIFF} case01.log ./compare/case01.log
${DIFF} case01_o0.dat ./compare/case01_o0.dat
${DIFF} case01_o1.dat ./compare/case01_o1.dat
${DIFF} case01_e0.dat ./compare/case01_e0.dat

