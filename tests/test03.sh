#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case03


#
# Compare current results against the regression data
#

${DIFF} case03.log ./compare/case03.log
${DIFF} case03_o0.dat ./compare/case03_o0.dat
${DIFF} case03_o1.dat ./compare/case03_o1.dat
${DIFF} case03_e0.dat ./compare/case03_e0.dat

