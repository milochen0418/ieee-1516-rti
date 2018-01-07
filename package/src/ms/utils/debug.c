#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include "debug.h"

int debug_mode = DEBUG_NONE;

static struct timeval inittime;
static int init_once = 0;

void init_time(void)
{
    if (init_once == 1)
        return;

    gettimeofday(&inittime, 0);
    init_once = 1;
}

static void message(char *s, int showtime)
{
    if (showtime){
        char buf[100];
        struct timeval tv;
        
        gettimeofday(&tv, 0);
#if ABS_TIME
        time_t tp;
        tp = tv.tv_sec;
        strftime(buf, 100, "[%H:%M:%S", localtime(&tp));
        sprintf(buf+9, ".%06ld]", tv.tv_usec);
#else
        tv.tv_sec -= inittime.tv_sec;
        tv.tv_usec -= inittime.tv_usec;
        if (tv.tv_usec < 0){
            tv.tv_usec += 1000000;
            tv.tv_sec -= 1;
        }

        strftime(buf, 100, "[%H:%M:%S", gmtime(&tv.tv_sec));
        sprintf(buf+9, ".%06ld]", tv.tv_usec);
#endif
        printf("%s %s", buf, s);
    }
    else
        printf("%s", s);
}

void error(const char *fmt, ...)
{
    char s[2048] = "Error: ";
    va_list ap;

    va_start(ap, fmt);
    vsprintf(s+7, fmt, ap);

    message(s, 1);
    va_end(ap);
    exit(1);
}

void warning(const char *fmt, ...)
{
    char s[2048] = "Warning: ";
    va_list ap;

    va_start(ap, fmt);
    vsprintf(s+9, fmt, ap);

    message(s, 1);
    va_end(ap);
}

/* 
 * info_p()
 *
 * Primary mode, time is not shown.
 */
void info_p(int label, const char *fmt, ...)
{
    char s[2048];
    va_list ap;

    if ((debug_mode & label) == 0)
        return;

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    
    message(s, 0);
    va_end(ap);
}

/* 
 * info_p()
 *
 * Time is not shown.
 */
void info(int label, const char *fmt, ...)
{
    char s[2048];
    va_list ap;

    if ((debug_mode & label) == 0)
        return;

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    
    message(s, 1);
    va_end(ap);
}

int get_debugmode(void)
{
    return debug_mode;
}

void set_debugmode(int mode)
{
    debug_mode = mode;
}

/*
 * vim: ts=8 sts=4 sw=4 expandtab
 */
