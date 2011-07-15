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

${PULSE} case11


#
# Compare current results against the regression data
#

${DIFF} case11.log ./compare/case11.log
${DIFF} case11_o0.xml ./compare/case11_o0.xml
${DIFF} case11_o1.xml ./compare/case11_o1.xml
${DIFF} case11_e0.xml ./compare/case11_e0.xml

