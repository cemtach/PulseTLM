













//
// Large chunks of the setup here were borrowed from the recent GRACE
// and glib setup files.
//




// Define if you need to in order for stat and other things to work.  
#undef _POSIX_SOURCE

// Define if you have the ANSI C header files.  
#undef STDC_HEADERS

// Define if char is unsigned   
#undef __CHAR_UNSIGNED__   
// Define sizeof(char)          
#define SIZEOF_CHAR         0
// Define sizeof(short)         
#define SIZEOF_SHORT        0
// Define sizeof(int)           
#define SIZEOF_INT          0
// Define sizeof(long)          
#define SIZEOF_LONG         0
// Define sizeof(long long)     
#define SIZEOF_LONG_LONG    0
// Define sizeof(float)         
#define SIZEOF_FLOAT        0
// Define sizeof(double)        
#define SIZEOF_DOUBLE       0
// Define sizeof(long double)   
#define SIZEOF_LONG_DOUBLE  0
// Define sizeof(void *)        
#define SIZEOF_VOID_P       0

// Define to empty if the keyword does not work.  
#undef const

// Define to `int' if <sys/types.h> doesn't define.  
#undef pid_t

// Define to `unsigned' if <sys/types.h> doesn't define.  
#undef size_t

// Define if you have the <unistd.h> header file.  
#undef HAVE_UNISTD_H


// Define if you have <sys/wait.h> that is POSIX.1 compatible.  
#undef HAVE_SYS_WAIT_H

// Define if your <sys/time.h> declares struct tm.  
#undef TM_IN_SYS_TIME

// Define if <time.h> and <sys/time.h> can be both included.  
#undef TIME_WITH_SYS_TIME

// Define if you have the getcwd function.  
#undef HAVE_GETCWD

// Define if you have the strcmp function.  
#undef HAVE_STRCMP

// Define if you have the strlen function.  
#undef HAVE_STRLEN

// Define if you have the strncmp function.  
#undef HAVE_STRNCMP

// Define if you have the clock_gettime function.  
#undef HAVE_CLOCK_GETTIME

// Define if you have the time function.  
#undef HAVE_TIME

// Define if you have the times function.  
#undef HAVE_TIMES

// Define if you have the clock function.  
#undef HAVE_CLOCK

// Define if you have the gethostname function.  
#undef HAVE_GETHOSTNAME

// If words are stored with the most significant byte first (like
//                   Motorola and SPARC, but not Intel and VAX, CPUs 
#undef WORDS_BIGENDIAN

// Define if your FPU arithmetics is of the DEC type.  
#undef HAVE_DEC_FPU

// Define if your FPU arithmetics is of the little endian IEEE type.  
#undef HAVE_LIEEE_FPU

// Define if your FPU arithmetics is of the big endian IEEE type.  
#undef HAVE_BIEEE_FPU
// Define if you have the m library (-lm).  
#undef HAVE_LIBM

// Define if you have the <math.h> header file.  
#undef HAVE_MATH_H

// Define if you have <float.h>.  
#undef HAVE_FLOAT_H

// Define if you have <limits.h>.  
#undef HAVE_LIMITS_H

// Define if you have <ieeefp.h>.  
#undef HAVE_IEEEFP_H

// Define if you have the hypot function.  
#undef HAVE_HYPOT

// Define if you have the cbrt function.  
#undef HAVE_CBRT

// Define if you have the log2 function.  
#undef HAVE_LOG2

// Define if you have the rint function.  
#undef HAVE_RINT

// Define if you have the ceil function.  
#undef HAVE_CEIL

// Define if you have the lgamma function.  
#undef HAVE_LGAMMA

// Define if math.h defines the lgamma function.  
#undef HAVE_LGAMMA_IN_MATH_H

// Define if math.h defines the signgam variable.  
#undef HAVE_SIGNGAM_IN_MATH_H

// Define if you have the asinh function.  
#undef HAVE_ASINH

// Define if you have the acosh function.  
#undef HAVE_ACOSH

// Define if you have the atanh function.  
#undef HAVE_ATANH

// Define if you have the erf function.  
#undef HAVE_ERF

// Define if you have the erfc function.  
#undef HAVE_ERFC

// Define if you have the finite function.  
#undef HAVE_FINITE

// Define if you have the isfinite function.  
#undef HAVE_ISFINITE


// Define if you have the Bessel j0 function.  
#undef HAVE_J0

// Define if you have the Bessel j1 function.  
#undef HAVE_J1

// Define if you have the Bessel jn function.  
#undef HAVE_JN

// Define if you have the Bessel y0 function.  
#undef HAVE_Y0

// Define if you have the Bessel y1 function.  
#undef HAVE_Y1

// Define if you have the Bessel yn function.  
#undef HAVE_YN


// Define as 1 if you have unistd.h.  
#undef HAVE_UNISTD_H

// Define if you have the <sys/time.h> header file.  
#undef HAVE_SYS_TIME_H

// Define if you have the <sys/times.h> header file.  
#undef HAVE_SYS_TIMES_H

// Define if you have the <sys/types.h> header file.  
#undef HAVE_SYS_TYPES_H


// Define if you want to compile in (a basic) support for debugging  
#undef WITH_DEBUG

#if defined(WITH_DEBUG)
#  define DEBUG
#else
#  define NDEBUG
#endif

#undef G_THREADS_ENABLED


#undef HAVE_GETPWUID_R
#undef HAVE_GETPWUID_R_POSIX

#undef PULSE_SIZEOF_GMUTEX
#undef PULSE_BYTE_CONTENTS_GMUTEX

#undef HAVE_PTHREAD_COND_TIMEDWAIT_POSIX
#undef HAVE_PTHREAD_GETSPECIFIC_POSIX
#undef HAVE_PTHREAD_MUTEX_TRYLOCK_POSIX


#undef G_THREAD_SOURCE


