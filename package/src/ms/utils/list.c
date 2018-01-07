#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "memory_util.h"
#include "debug.h"

/*
 * search_queue()
 *
 * Search queue to find the first perfect-match item with key '__key'
 *
 * If item exists, return its address. Otherwise, return NULL.
 */
void *search_queue(struct queue_item_t *__head, void *__key, int __keysize)
{
    list_t *l;
    void *key;

    if (__head == NULL)
        error("null head queue pointer\n");
    if (__key == NULL)
        error("null key pointer\n");
    
    list_for_each(l, &__head->list)
    {
        key = ((struct queue_item_t *)l)->key;
        if (!memcmp(key, __key, __keysize)){
            return l;
        }
    }
    
    return NULL;
}

/*
 * search_queue_cmp()
 *
 * Search queue to find the first perfect-match item with key '__key' 
 *      and the compare function '__com_fn'
 */
void *search_queue_cmp(struct queue_item_t *__head, void *__key, 
        int (*__cmp_fn)(void *cmp1, void *cmp2))
{
    list_t *l;
    void *key;

    if (__head == NULL)
        error("null head queue pointer\n");
    if (__key == NULL)
        error("null key pointer\n");
    if (__cmp_fn == NULL)
        error("null compare function pointer\n");
    
    list_for_each(l, &__head->list)
    {    
        key = ((struct queue_item_t *)l)->key;
        if (!__cmp_fn(key, __key))
            return l;
    }

    return NULL;

}

/*
 * insert_queue()
 *
 * Add item to tail of queue '__head' with key '__key'.
 */
void insert_queue(struct queue_item_t *__head, void *__key, void *__data)
{
    struct queue_item_t *item;
    
    if (__head == NULL)
        error("null head queue pointer\n");
    if (__key == NULL)
        error("null key pointer\n");
    
    item = Malloc(sizeof(struct queue_item_t));
    INIT_LIST_HEAD(&item->list);
    item->key = __key;
    item->data = __data;
    
    list_add_tail(&item->list, &__head->list);
}

/*
 * remove_queue()
 *
 * Remove item '__item' from its queue.
 */
void remove_queue(struct queue_item_t *__item)
{
    if (__item == NULL)
        error("null item pointer\n");
    
    list_del(&__item->list);
}

/*
 * delete_item()
 *
 * Release memory of item '__item'.
 */
void delete_item(struct queue_item_t *__item)
{

    if (__item == NULL)
        error("null item pointer\n");

    if (__item->key)
        free(__item->key);

    free(__item);
}

/*
 * vim: ts=4 sts=4 sw=4 expandtab
 */
