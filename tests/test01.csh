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

${PULSE} case01


#
# Compare current results against the regression data
#

${DIFF} case01.log ./compare/case01.log
${DIFF} case01_o0.dat ./compare/case01_o0.dat
${DIFF} case01_o1.dat ./compare/case01_o1.dat
${DIFF} case01_e0.dat ./compare/case01_e0.dat

