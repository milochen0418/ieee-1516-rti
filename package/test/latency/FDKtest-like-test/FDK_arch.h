/*這邊是有關TIMER_NOW的函數*/
#ifndef _FDK_ARCH_H
#define _FDK_ARCH_H
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#define HAVE_GETTIMEOFDAY 1

#ifdef WIN32
typedef struct _timeb TIMER_TYPE;
#  define TIMER_NOW(_t)             _ftime(&_t)
#  define TIMER_SECONDS(_t)         ((double)(_t).time + (_t).millitm*1e-3)
#elif HAVE_GETHRTIME
typedef int64_t TIMER_TYPE;
#  define TIMER_NOW(_t)             (_t) = gethrvtime()
#  define TIMER_SECONDS(_t)         ((double)(_t) * 1.0e-9)
#elif HAVE_GETTIMEOFDAY
typedef struct timeval TIMER_TYPE;
#  define TIMER_NOW(_t)             gettimeofday(&_t,NULL)
#  define TIMER_SECONDS(_t)         ((double)(_t).tv_sec + (_t).tv_usec*1e-6)
#else
#endif

#define TIMER_DIFF(_t1, _t2)        (TIMER_SECONDS(_t1)-TIMER_SECONDS(_t2))

#endif	//_FDK_ARCH_H
