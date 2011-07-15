#!/bin/sh

. commands.sh

#
# Run the simulator
#

${PULSE} case11


#
# Compare current results against the regression data
#

${DIFF} case11.log ./compare/case11.log
${DIFF} case11_o0.xml ./compare/case11_o0.xml
${DIFF} case11_o1.xml ./compare/case11_o1.xml
${DIFF} case11_e0.dat ./compare/case11_e0.dat

