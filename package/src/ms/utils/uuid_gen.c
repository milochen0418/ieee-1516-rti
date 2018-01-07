#include <pthread.h>
#include "uuid_gen.h"

static uint64_t uuid = 0;
static pthread_mutex_t uuid_mutex = PTHREAD_MUTEX_INITIALIZER;

uint64_t uuid_gen(void)
{
	uint64_t id;
	
	pthread_mutex_lock(&uuid_mutex);
	id = uuid;
	uuid++;
	pthread_mutex_unlock(&uuid_mutex);
	
	return id;
}
/*
 * vim: ts=4 sts=4 sw=4
 */
