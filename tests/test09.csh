#!/bin/csh

#
# The location of the executable PulseTLM
#
set PULSE = ../src/PulseTLM

#
# Spiff is a nice file differencing package that actually compares
# floating point information.  If spiff is not avialable, use diff or
# maybe xdiff to perform the differencing to the results that came
# with the distribution.
#
set DIFF=${SPDIFF}



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

