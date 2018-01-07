#include <string.h>
#include "memory_util.h"
#include "profile.h"

static struct profile_t *profile_head;

/*
 * profile_init()
 *
 * Create head of profiling list.
 */
void profile_init()
{
	profile_head = Malloc(sizeof(struct profile_t));

	INIT_LIST_HEAD(&profile_head->list);
	profile_head->name = strdup("profile_head");
	profile_head->data = NULL;
}

/*
 * create_profile()
 */
void create_profile(char *name, void *data)
{
	struct profile_t *profile;

	profile = Malloc(sizeof(struct profile_t));

	INIT_LIST_HEAD(&profile_head->list);
	profile->name = strdup(name);
	profile->data = data;

	list_add_tail(&profile->list, &profile_head->list);
}

/*
 * 
 */
void insert_conn_history(char *hostname)
{
	
}

/*
 * vim: ts=4 sts=4 sw=4
 */
