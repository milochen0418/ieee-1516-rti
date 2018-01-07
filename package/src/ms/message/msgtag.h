#ifndef _MSGTAG
#define _MSGTAG


struct  ConnectNode
{
    uint32_t id;
    char address[20];
    uint32_t port;
};
typedef struct ConnectNode connect_t;
#define JOINTAG 2
#define UPDATETAG 3
#define CONNECTTAG 4
#define LEAVETAG 5
#define FEDEX 12
#define USRMSG 519



#endif
