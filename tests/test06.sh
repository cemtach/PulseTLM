#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case06


#
# Compare current results against the regression data
#

${DIFF} case06.log ./compare/case06.log
${DIFF} case06_o0.dat ./compare/case06_o0.dat
${DIFF} case06_o1.dat ./compare/case06_o1.dat
${DIFF} case06_e0.dat ./compare/case06_e0.dat

