#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "cache.h"

static CacheEntry cache_entries[MAX_CACHE_ENTRIES];
static pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

void cache_init(void)
{
    pthread_mutex_lock(&cache_mutex);
    memset(cache_entries, 0, sizeof(cache_entries));
    pthread_mutex_unlock(&cache_mutex);

    printf("[CACHE] Cache initialized.\n");
}

int cache_get(const char *key, char *data, size_t data_size)
{
    if (key == NULL || data == NULL || data_size == 0)
        return -1;

    pthread_mutex_lock(&cache_mutex);

    for (int i = 0; i < MAX_CACHE_ENTRIES; i++) {
        if (cache_entries[i].valid &&
            strcmp(cache_entries[i].key, key) == 0) {

            if (cache_entries[i].data_size + 1 > data_size) {
                pthread_mutex_unlock(&cache_mutex);
                return -1;
            }

            memcpy(data, cache_entries[i].data,
                   cache_entries[i].data_size);
            data[cache_entries[i].data_size] = '\0';

            printf("[CACHE] HIT: %s\n", key);
            int result = (int)cache_entries[i].data_size;

            pthread_mutex_unlock(&cache_mutex);
            return result;
        }
    }

    printf("[CACHE] MISS: %s\n", key);
    pthread_mutex_unlock(&cache_mutex);
    return 0;
}

int cache_put(const char *key, const char *data, size_t data_size)
{
    int index = -1;

    if (key == NULL || data == NULL || data_size == 0)
        return -1;

    if (strlen(key) >= MAX_CACHE_KEY ||
        data_size >= MAX_CACHE_DATA) {
        printf("[CACHE] Key or response too large.\n");
        return -1;
    }

    pthread_mutex_lock(&cache_mutex);

    for (int i = 0; i < MAX_CACHE_ENTRIES; i++) {
        if (cache_entries[i].valid &&
            strcmp(cache_entries[i].key, key) == 0) {
            index = i;
            break;
        }

        if (index == -1 && !cache_entries[i].valid)
            index = i;
    }

    if (index == -1) {
        index = 0;
        printf("[CACHE] Cache full. Replacing entry.\n");
    }

    strcpy(cache_entries[index].key, key);
    memcpy(cache_entries[index].data, data, data_size);
    cache_entries[index].data[data_size] = '\0';
    cache_entries[index].data_size = data_size;
    cache_entries[index].valid = 1;

    printf("[CACHE] Stored: %s\n", key);
    pthread_mutex_unlock(&cache_mutex);
    return 0;
}

void cache_clear(void)
{
    pthread_mutex_lock(&cache_mutex);
    memset(cache_entries, 0, sizeof(cache_entries));
    pthread_mutex_unlock(&cache_mutex);

    printf("[CACHE] Cache cleared.\n");
}
