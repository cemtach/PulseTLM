#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case04


#
# Compare current results against the regression data
#

${DIFF} case04.log ./compare/case04.log
${DIFF} case04_o0.dat ./compare/case04_o0.dat
${DIFF} case04_o1.dat ./compare/case04_o1.dat
${DIFF} case04_e0.dat ./compare/case04_e0.dat

