#!/bin/sh

. commands.sh

#
# Run the simulator
#

${PULSE} case07


#
# Compare current results against the regression data
#

${DIFF} case07.log ./compare/case07.log
${DIFF} case07_o0.dat ./compare/case07_o0.dat
${DIFF} case07_o1.dat ./compare/case07_o1.dat
${DIFF} case07_e0.dat ./compare/case07_e0.dat
${DIFF} case07_o2.viz ./compare/case07_o2.viz
${CMP} case07_o2_000000.bob ./compare/case07_o2_000000.bob
${CMP} case07_o2_000100.bob ./compare/case07_o2_000100.bob
${CMP} case07_o2_000200.bob ./compare/case07_o2_000200.bob
${CMP} case07_o2_000300.bob ./compare/case07_o2_000300.bob
${CMP} case07_o2_000400.bob ./compare/case07_o2_000400.bob
${CMP} case07_o2_000500.bob ./compare/case07_o2_000500.bob
${CMP} case07_o2_000600.bob ./compare/case07_o2_000600.bob
${CMP} case07_o2_000700.bob ./compare/case07_o2_000700.bob
${CMP} case07_o2_000800.bob ./compare/case07_o2_000800.bob
${CMP} case07_o2_000900.bob ./compare/case07_o2_000900.bob
${CMP} case07_o2_001000.bob ./compare/case07_o2_001000.bob
${CMP} case07_o2_001003.bob ./compare/case07_o2_001003.bob
