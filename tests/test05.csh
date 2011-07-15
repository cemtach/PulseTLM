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
set DIFF = spiff



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

