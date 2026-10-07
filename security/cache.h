#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>

#define MAX_CACHE_ENTRIES 20
#define MAX_CACHE_KEY 2048
#define MAX_CACHE_DATA 65536

typedef struct
{
    char key[MAX_CACHE_KEY];
    char data[MAX_CACHE_DATA];

    size_t data_size;

    int valid;

} CacheEntry;

void cache_init(void);

int cache_get(
    const char *key,
    char *data,
    size_t data_size
);

int cache_put(
    const char *key,
    const char *data,
    size_t data_size
);

void cache_clear(void);

#endif
