#define _POSIX_C_SOURCE 200809L

#include "performance.h"

#include <time.h>

static double current_time_ms(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return -1.0;
    }

    return (double)ts.tv_sec * 1000.0
         + (double)ts.tv_nsec / 1000000.0;
}

double performance_start(void)
{
    return current_time_ms();
}

double performance_elapsed_ms(double start_time)
{
    double end_time = current_time_ms();

    if (start_time < 0.0 || end_time < 0.0) {
        return -1.0;
    }

    return end_time - start_time;
}
