
#SPDIFF=gdiff
SPDIFF=@spdiff@
export SPDIFF
CMP=@cmp@
export CMP

BOURNE_SHELL=$(bourne_shell)
C_SHELL=$(c_shell)

# Nothing to make, all shell scripts.
# So don't do anything until called with make check.
bin_SCRIPTS =

## We want to have one way to generate the log files,
## depending on the shells we have available.  It is
## a preferred practice in GNU coding, and given Tom
## Christiansen's article, not to use the C shell for
## this, but we give people the choice.
## http://www.faqs.org/faqs/unix-faq/shell/csh-whynot/
##
## This if...else...endif is an automake conditional.

if HAVE_BOURNE_SHELL

check_SCRIPTS = test01.sh test02.sh test03.sh test04.sh test05.sh test06.sh test07.sh test08.sh test09.sh test10.sh test11.sh test12.sh 

else

check_SCRIPTS = test01.csh test02.csh test03.csh test04.csh test05.csh test06.csh test07.csh test08.csh test09.csh test10.csh test11.csh test12.csh 

endif 

TESTS = $(check_SCRIPTS)



CLEANFILES= \
	*~ core \
	*.log \
	*0.bob \
	*1.bob \
	*2.bob \
	*3.bob \
	*4.bob \
	*5.bob \
	*6.bob \
	*7.bob \
	*8.bob \
	*9.bob \
	*0.pnm \
	*1.pnm \
	*2.pnm \
	*3.pnm \
	*4.pnm \
	*5.pnm \
	*6.pnm \
	*7.pnm \
	*8.pnm \
	*9.pnm \
	*.vtr \
	*.mtv \
	*.dat \
	c_runLog \
	*.viz \
	*.xml
