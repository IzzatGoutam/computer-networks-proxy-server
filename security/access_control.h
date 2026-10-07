#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#define MAX_BLOCKED_HOSTS 50
#define MAX_HOST_LENGTH 256

void access_control_init(void);

int is_host_allowed(
    const char *host
);

int block_host(
    const char *host
);

int unblock_host(
    const char *host
);

void access_control_clear(void);

#endif
