#!/bin/sh

. commands.sh


#
# Run the simulator
#

${PULSE} case09


#
# Compare current results against the regression data
#

${DIFF} case09.log ./compare/case09.log
${DIFF} case09_o0.dat ./compare/case09_o0.dat
${DIFF} case09_o1.dat ./compare/case09_o1.dat
${DIFF} case09_e0.dat ./compare/case09_e0.dat

