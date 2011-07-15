


#SPDIFF=gdiff
SPDIFF=diff
export SPDIFF

# Nothing to make, all shell scripts.
all: check



check: test01.log test02.log test03.log test04.log test05.log test06.log test07.log test08.log test09.log test10.log test11.log test12.log 



%.log: %.sh Makefile
	./$< > $@




clean: cleandata
	-rm -f *~ core





cleandata:
	-rm -f *.log
	-rm -f *0.bob
	-rm -f *1.bob
	-rm -f *2.bob
	-rm -f *3.bob
	-rm -f *4.bob
	-rm -f *5.bob
	-rm -f *6.bob
	-rm -f *7.bob
	-rm -f *8.bob
	-rm -f *9.bob
	-rm -f *0.pnm
	-rm -f *1.pnm
	-rm -f *2.pnm
	-rm -f *3.pnm
	-rm -f *4.pnm
	-rm -f *5.pnm
	-rm -f *6.pnm
	-rm -f *7.pnm
	-rm -f *8.pnm
	-rm -f *9.pnm
	-rm -f *.vtr
	-rm -f *.dat
	-rm -f *.mtv
	-rm -f c_runLog
	-rm -f *.viz
	-rm -f *.xml

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
