#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case02


#
# Compare current results against the regression data
#

${DIFF} case02.log ./compare/case02.log
${DIFF} case02_o0.dat ./compare/case02_o0.dat
${DIFF} case02_o1.dat ./compare/case02_o1.dat
${DIFF} case02_e0.dat ./compare/case02_e0.dat

