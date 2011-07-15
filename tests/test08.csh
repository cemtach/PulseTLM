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

${PULSE} case08


#
# Compare current results against the regression data
#

${DIFF} case08.log ./compare/case08.log
${DIFF} case08_e0.dat ./compare/case08_e0.dat
