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

${PULSE} case10


#
# Compare current results against the regression data
#

${DIFF} case10.log ./compare/case10.log
${DIFF} case10_o0_000000.mtv ./compare/case10_o0_000000.mtv
${DIFF} case10_o0_000020.mtv ./compare/case10_o0_000020.mtv
${DIFF} case10_o0_000040.mtv ./compare/case10_o0_000040.mtv
${DIFF} case10_o0_000060.mtv ./compare/case10_o0_000060.mtv
${DIFF} case10_o0_000080.mtv ./compare/case10_o0_000080.mtv
${DIFF} case10_o0_000100.mtv ./compare/case10_o0_000100.mtv
${DIFF} case10_o0_000120.mtv ./compare/case10_o0_000120.mtv
${DIFF} case10_o0_000140.mtv ./compare/case10_o0_000140.mtv
${DIFF} case10_o0_000160.mtv ./compare/case10_o0_000160.mtv
${DIFF} case10_o0_000180.mtv ./compare/case10_o0_000180.mtv
${DIFF} case10_o0_000200.mtv ./compare/case10_o0_000200.mtv
${DIFF} case10_o0_000220.mtv ./compare/case10_o0_000220.mtv
${DIFF} case10_o0_000240.mtv ./compare/case10_o0_000240.mtv
${DIFF} case10_o0_000260.mtv ./compare/case10_o0_000260.mtv
${DIFF} case10_o0_000280.mtv ./compare/case10_o0_000280.mtv
${DIFF} case10_o0_000300.mtv ./compare/case10_o0_000300.mtv
${DIFF} case10_o0_000320.mtv ./compare/case10_o0_000320.mtv
${DIFF} case10_o0_000340.mtv ./compare/case10_o0_000340.mtv
${DIFF} case10_o0_000360.mtv ./compare/case10_o0_000360.mtv
${DIFF} case10_o0_000380.mtv ./compare/case10_o0_000380.mtv
${DIFF} case10_o0_000400.mtv ./compare/case10_o0_000400.mtv
${DIFF} case10_o0_000420.mtv ./compare/case10_o0_000420.mtv
${DIFF} case10_o0_000440.mtv ./compare/case10_o0_000440.mtv
${DIFF} case10_o0_000460.mtv ./compare/case10_o0_000460.mtv
${DIFF} case10_o0_000480.mtv ./compare/case10_o0_000480.mtv
${DIFF} case10_o0_000500.mtv ./compare/case10_o0_000500.mtv
${DIFF} case10_o0_000520.mtv ./compare/case10_o0_000520.mtv
${DIFF} case10_o0_000540.mtv ./compare/case10_o0_000540.mtv
${DIFF} case10_o0_000560.mtv ./compare/case10_o0_000560.mtv
${DIFF} case10_o0_000580.mtv ./compare/case10_o0_000580.mtv
${DIFF} case10_o0_000600.mtv ./compare/case10_o0_000600.mtv
${DIFF} case10_o0_000620.mtv ./compare/case10_o0_000620.mtv
${DIFF} case10_o0_000640.mtv ./compare/case10_o0_000640.mtv
${DIFF} case10_o0_000660.mtv ./compare/case10_o0_000660.mtv
${DIFF} case10_o0_000680.mtv ./compare/case10_o0_000680.mtv
${DIFF} case10_o0_000700.mtv ./compare/case10_o0_000700.mtv
${DIFF} case10_o0_000720.mtv ./compare/case10_o0_000720.mtv
${DIFF} case10_o0_000740.mtv ./compare/case10_o0_000740.mtv
${DIFF} case10_o0_000760.mtv ./compare/case10_o0_000760.mtv
${DIFF} case10_o0_000780.mtv ./compare/case10_o0_000780.mtv
${DIFF} case10_o0_000800.mtv ./compare/case10_o0_000800.mtv
${DIFF} case10_o0_000820.mtv ./compare/case10_o0_000820.mtv
${DIFF} case10_o0_000840.mtv ./compare/case10_o0_000840.mtv
${DIFF} case10_o0_000860.mtv ./compare/case10_o0_000860.mtv
${DIFF} case10_o0_000880.mtv ./compare/case10_o0_000880.mtv
${DIFF} case10_o0_000900.mtv ./compare/case10_o0_000900.mtv
${DIFF} case10_o0_000920.mtv ./compare/case10_o0_000920.mtv
${DIFF} case10_o0_000940.mtv ./compare/case10_o0_000940.mtv
${DIFF} case10_o0_000960.mtv ./compare/case10_o0_000960.mtv
${DIFF} case10_o0_000980.mtv ./compare/case10_o0_000980.mtv
${DIFF} case10_o0_001000.mtv ./compare/case10_o0_001000.mtv


