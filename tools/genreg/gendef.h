
#define SHORTCODE
#define JOBS
#define LIST
#define STAB

#define MINN  3
#define MAXN 99
#define MINK  2
#define MAXK  9
#define MINT  3
#define MAXT  9

#define NOTSET 127
#define COUNTSTEP 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__unix__) && !defined(__APPLE__) && !defined(__cygwin__)
#define DOSTIME
#else

/* The CPU-time code below modified by Brendan McKay */

#include <limits.h>
#include <sys/times.h>
#include <time.h>
#if !defined(CLK_TCK) && !defined(_SC_CLK_TCK)
#include <unistd.h>
#endif
#if !defined(CLK_TCK) && defined(_SC_CLK_TCK)
#define CLK_TCK sysconf(_SC_CLK_TCK)
#endif
#if !defined(CLK_TCK) && defined(CLOCKS_PER_SEC)
#define CLK_TCK CLOCKS_PER_SEC
#endif
#if !defined(CLK_TCK)
#define CLK_TCK 60     /* If the CPU time stated by the program appears
                       to be out by a constant ratio, the most likely
                       explanation is that the code got to this point but
                       60 is the wrong guess.  Another common value is 100. */
#endif
#define time_factor CLK_TCK
#endif

#ifdef time_factor
#define UNIXTIME
#endif

#ifdef DOSTIME
#include <time.h>
#define time_factor CLK_TCK
#endif

#ifdef SHORTCODE
#define APP "scd"
#else
#define APP "lst"
#endif


typedef int SCHAR;
typedef unsigned long long ULONG;
typedef unsigned int UINT;

void ordstart(SCHAR _n,SCHAR _k,SCHAR _t,SCHAR _mid_max,
              SCHAR _splitlevel,UINT _jobs,UINT _jobnr,
              FILE *_lstfile,FILE *_autfile,FILE *_ergfile,
              ULONG _tostore,ULONG _toprint,ULONG _count,
              SCHAR _storeall,SCHAR _printall, ULONG *_anz,
              ULONG _tog6,SCHAR _g6all,FILE *_g6file,SCHAR _nooutput);
SCHAR default_mid_max(SCHAR n,SCHAR k,SCHAR t);
SCHAR default_splitlevel(SCHAR n,SCHAR k,SCHAR t,char bipartite);
