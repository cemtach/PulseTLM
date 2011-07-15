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

${PULSE} case02


#
# Compare current results against the regression data
#

${DIFF} case02.log ./compare/case02.log
${DIFF} case02_o0.dat ./compare/case02_o0.dat
${DIFF} case02_o1.dat ./compare/case02_o1.dat
${DIFF} case02_e0.dat ./compare/case02_e0.dat

