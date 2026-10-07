#include <stdio.h>
#include <string.h>

#include "access_control.h"

static char blocked_hosts[
    MAX_BLOCKED_HOSTS
][MAX_HOST_LENGTH];

static int blocked_count = 0;

void access_control_init(void)
{
    memset(
        blocked_hosts,
        0,
        sizeof(blocked_hosts)
    );

    blocked_count = 0;

    printf(
        "[ACCESS] Access control initialized.\n"
    );
}

int is_host_allowed(
    const char *host)
{
    if (host == NULL ||
        host[0] == '\0') {
        return 0;
    }

    for (int i = 0;
         i < blocked_count;
         i++) {

        if (strcmp(
                blocked_hosts[i],
                host
            ) == 0) {

            printf(
                "[ACCESS] BLOCKED: %s\n",
                host
            );

            return 0;
        }
    }

    printf(
        "[ACCESS] ALLOWED: %s\n",
        host
    );

    return 1;
}

int block_host(
    const char *host)
{
    if (host == NULL ||
        host[0] == '\0') {
        return -1;
    }

    if (blocked_count >=
        MAX_BLOCKED_HOSTS) {
        return -1;
    }

    for (int i = 0;
         i < blocked_count;
         i++) {

        if (strcmp(
                blocked_hosts[i],
                host
            ) == 0) {
            return 0;
        }
    }

    strncpy(
        blocked_hosts[blocked_count],
        host,
        MAX_HOST_LENGTH - 1
    );

    blocked_hosts[blocked_count][
        MAX_HOST_LENGTH - 1
    ] = '\0';

    blocked_count++;

    printf(
        "[ACCESS] Host blocked: %s\n",
        host
    );

    return 0;
}

int unblock_host(
    const char *host)
{
    if (host == NULL) {
        return -1;
    }

    for (int i = 0;
         i < blocked_count;
         i++) {

        if (strcmp(
                blocked_hosts[i],
                host
            ) == 0) {

            for (int j = i;
                 j < blocked_count - 1;
                 j++) {

                strcpy(
                    blocked_hosts[j],
                    blocked_hosts[j + 1]
                );
            }

            blocked_hosts[
                blocked_count - 1
            ][0] = '\0';

            blocked_count--;

            printf(
                "[ACCESS] Host unblocked: %s\n",
                host
            );

            return 0;
        }
    }

    return -1;
}

void access_control_clear(void)
{
    memset(
        blocked_hosts,
        0,
        sizeof(blocked_hosts)
    );

    blocked_count = 0;

    printf(
        "[ACCESS] Access control cleared.\n"
    );
}
