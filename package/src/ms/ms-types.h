#ifndef __MS_TYPES_H
#define __MS_TYPES_H

#include <stdint.h>

#if 0
#define SERVERID		0
#define V_PUBLISH		100
#define V_SUBSCRIBE		101
#define V_END			102
#define V_UNPUBLISH		103
#define V_UNSUBSCRIBE	104
#define V_GETCOUNT		105
#define V_GETMEMBER		106
#define V_TAG			0
#endif

#define SS_usrmsg		519
#define SS_anysource	520

typedef uint32_t	ClassHandle;
typedef uint32_t	AttributeHandle;
typedef uint32_t	ObjectHandle;

enum error_code
{
	MS_SUCCESS = 0,
	MS_FAILED,
	MS_INTERNEL_ERROR,
};

enum msg_type_t
{
	TYPE_RO = 0,
	TYPE_TSO,
	TYPE_Event,
};

enum trans_type_t
{
	TYPE_RELIABLE = 0,
	TYPE_BEST_EFFORT,
};


typedef struct federateID
{
	uint32_t vid;	/* virtual server ID */
	uint32_t fid;	/* local federate ID */
} federateID;

typedef struct Message_Info
{
	struct federateID source;
	enum msg_type_t msg_type;
	enum trans_type_t trans_type;
	char msg_desc[128];
	uint32_t msg_body_size;
} Msg_Info;

#endif	/* __MS_TYPE_H */

/*
 * vim: ts=4 sts=4 sw=4
 */
