dnl PULSE_IF_VAR_EQ (ENV_VAR, VALUE [, EQUALS_ACTION] [, ELSE_ACTION])
AC_DEFUN(PULSE_IF_VAR_EQ,[
        case "$[$1]" in
        "[$2]"[)]
                [$3]
                ;;
        *[)]
                [$4]
                ;;
        esac
])
dnl PULSE_STR_CONTAINS (SRC_STRING, SUB_STRING [, CONTAINS_ACTION] [, ELSE_ACTION])
AC_DEFUN(PULSE_STR_CONTAINS,[
        case "[$1]" in
        *"[$2]"*[)]
                [$3]
                ;;
        *[)]
                [$4]
                ;;
        esac
])
dnl PULSE_ADD_TO_VAR (ENV_VARIABLE, CHECK_STRING, ADD_STRING)
AC_DEFUN(PULSE_ADD_TO_VAR,[
        PULSE_STR_CONTAINS($[$1], [$2], [$1]="$[$1]", [$1]="$[$1] [$3]")
])

dnl PULSE_SIZEOF (INCLUDES, TYPE, ALIAS [, CROSS-SIZE])
AC_DEFUN(PULSE_SIZEOF,
[changequote(<<, >>)dnl
dnl The name to #define.
define(<<AC_TYPE_NAME>>, translit(pulse_sizeof_$3, [a-z *], [A-Z_P]))dnl
dnl The cache variable name.
define(<<AC_CV_NAME>>, translit(pulse_cv_sizeof_$3, [ *], [_p]))dnl
changequote([, ])dnl
AC_MSG_CHECKING(size of $2)
AC_CACHE_VAL(AC_CV_NAME,
[AC_TRY_RUN([#include <stdio.h>
$1
main()
{
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%d\n", sizeof($2));
  exit(0);
}], AC_CV_NAME=`cat conftestval`, AC_CV_NAME=0, ifelse([$4], , , AC_CV_NAME=$4))])dnl
AC_MSG_RESULT($AC_CV_NAME)
AC_DEFINE_UNQUOTED(AC_TYPE_NAME, $AC_CV_NAME)
undefine([AC_TYPE_NAME])dnl
undefine([AC_CV_NAME])dnl
])

dnl PULSE_BYTE_CONTENTS (INCLUDES, TYPE, ALIAS, N_BYTES, INITIALIZER)
AC_DEFUN(PULSE_BYTE_CONTENTS,
[changequote(<<, >>)dnl
dnl The name to #define.
define(<<AC_TYPE_NAME>>, translit(pulse_byte_contents_$3, [a-z *], [A-Z_P]))dnl
dnl The cache variable name.
define(<<AC_CV_NAME>>, translit(pulse_cv_byte_contents_$3, [ *], [_p]))dnl
changequote([, ])dnl
AC_MSG_CHECKING(byte contents of $2)
AC_CACHE_VAL(AC_CV_NAME,
[AC_TRY_RUN([#include <stdio.h>
$1
main()
{
  static $2 tv = $5;
  char *p = (char*) &tv;
  int i;
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  for (i = 0; i < $4; i++)
    fprintf(f, "%s%d", i?",":"", *(p++));
  fprintf(f, "\n");
  exit(0);
}], AC_CV_NAME=`cat conftestval`, AC_CV_NAME=0, AC_CV_NAME=0)])dnl
AC_MSG_RESULT($AC_CV_NAME)
AC_DEFINE_UNQUOTED(AC_TYPE_NAME, $AC_CV_NAME)
undefine([AC_TYPE_NAME])dnl
undefine([AC_CV_NAME])dnl
])

dnl PULSE_SYSDEFS (INCLUDES, DEFS_LIST, OFILE [, PREFIX])
AC_DEFUN(PULSE_SYSDEFS,
[pulse_sysdefso="translit($3, [-_a-zA-Z0-9 *], [-_a-zA-Z0-9])"
pulse_sysdef_msg=`echo $2 | sed 's/:[[^ 	]]*//g'`
if test "x`(echo '\n') 2>/dev/null`" != 'x\n'; then
  pulse_nl='\\n'
else
  pulse_nl='\n'
fi
AC_MSG_CHECKING(system definitions for $pulse_sysdef_msg)
cat >confrun.c <<_______EOF
#include <stdio.h>
$1
int main (int c, char **v) {
  FILE *f = fopen ("$pulse_sysdefso", "a");
  if (!f) return 1;
_______EOF
for pulse_sysdef_input in $2 ; do
	pulse_sysdef=`echo $pulse_sysdef_input | sed 's/^\([[^:]]*\):.*$/\1/'`
	pulse_default=`echo $pulse_sysdef_input | sed 's/^[[^:]]*:\(.*\)$/\1/'`
	echo "#ifdef $pulse_sysdef" >>confrun.c
	echo "  fprintf (f, \"#define PULSE_SYSDEF_%s %s%d${pulse_nl}\", \"$pulse_sysdef\", \"$4\", $pulse_sysdef);" >>confrun.c
	echo "#else" >>confrun.c
	if test $pulse_sysdef != $pulse_default; then
		echo "  fprintf (f, \"#define PULSE_SYSDEF_%s %s%d${pulse_nl}\", \"$pulse_sysdef\", \"$4\", $pulse_default);" >>confrun.c
	else
		echo "  fprintf (f, \"#define PULSE_SYSDEF_%s${pulse_nl}\", \"$pulse_sysdef\");" >>confrun.c
	fi
	echo "#endif" >>confrun.c
done
echo "return 0; }" >>confrun.c
AC_TRY_RUN(`cat confrun.c`, AC_MSG_RESULT(done),
[	for pulse_sysdef_input in $2 ; do
		pulse_sysdef=`echo $pulse_sysdef_input | sed 's/^\([[^:]]*\):.*$/\1/'`
		pulse_default=`echo $pulse_sysdef_input | sed 's/^[[^:]]*:\(.*\)$/\1/'`
		if test $pulse_sysdef != $pulse_default; then
			pulse_default=" $4$pulse_default"
		else
			pulse_default=
		fi
		echo "#define PULSE_SYSDEF_$pulse_sysdef$pulse_default" >>$pulse_sysdefso
	done
	AC_MSG_RESULT(failed)])
rm -f confrun.c
])
