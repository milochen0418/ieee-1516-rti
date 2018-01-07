#ifndef __DEBUG_H
#define __DEBUG_H

enum {
    DEBUG_NONE          = 0x0,
    DEBUG_MEMORY        = 0x1,
    DEBUG_PROFILE       = 0x2,
    DEBUG_NETWORK       = 0x4,
    DEBUG_FEDEX         = 0x8,
    DEBUG_OMT           = 0x10,
    DEBUG_MSGHANDLER    = 0x20,
    DEBUG_FM            = 0x40,
    DEBUG_DM            = 0x80,
    DEBUG_OM            = 0x100,
    DEBUG_OWNM          = 0x200,
    DEBUG_TM            = 0x400,
    DEBUG_DDM           = 0x800,
};

#define DEBUG_RTI   DEBUG_FM | DEBUG_DM | DEBUG_OM | DEBUG_OWNM | DEBUG_TM | DEBUG_DDM
#define DEBUG_ALL   DEBUG_MEMORY | DEBUG_PROFILE | DEBUG_NETWORK | DEBUG_FEDEX | \
    DEBUG_OMT | DEBUG_MSGHANDLER | DEBUG_RTI

#ifdef DEBUG
#define debug(label, fmt, args...)	info(label, fmt, ##args)
#define debug_p(label, fmt, args...)	info_p(label, fmt, ##args)
#else
#define debug(label, fmt, args...)
#define debug_p(label, fmt, args...)
#endif

void init_time(void);

/* error: show error message and exit */
void error(const char *fmt, ...);

/* warning: show warning message without exit */
void warning(const char *fmt, ...);

/* debug: show message */
void info(int label, const char *fmt, ...); 
void info_p(int label, const char *fmt, ...); 

int get_debugmode(void);
void set_debugmode(int mode);

#endif /* __DEBUG_H */
/*
 * vim: ts=8 sts=4 sw=4 expandtab
 */
