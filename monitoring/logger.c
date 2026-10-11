#define _POSIX_C_SOURCE 200809L

#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void logger_init(const char *filename)
{
    pthread_mutex_lock(&log_mutex);

    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }

    if (filename != NULL) {
        log_file = fopen(filename, "a");
    }

    pthread_mutex_unlock(&log_mutex);

    logger_log("INFO", "Logger initialized");
}

void logger_log(const char *level, const char *message)
{
    time_t now;
    struct tm time_info;
    char timestamp[32];

    if (level == NULL || message == NULL) {
        return;
    }

    now = time(NULL);

    if (localtime_r(&now, &time_info) == NULL) {
        return;
    }

    if (strftime(timestamp, sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S", &time_info) == 0) {
        return;
    }

    pthread_mutex_lock(&log_mutex);

    fprintf(stdout, "[%s] [%s] %s\n",
            timestamp, level, message);
    fflush(stdout);

    if (log_file != NULL) {
        fprintf(log_file, "[%s] [%s] %s\n",
                timestamp, level, message);
        fflush(log_file);
    }

    pthread_mutex_unlock(&log_mutex);
}

void logger_close(void)
{
    pthread_mutex_lock(&log_mutex);

    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }

    pthread_mutex_unlock(&log_mutex);
}
