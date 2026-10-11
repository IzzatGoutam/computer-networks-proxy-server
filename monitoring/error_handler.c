#include "error_handler.h"
#include "logger.h"

#include <stdio.h>

void report_error(const char *component, const char *message)
{
    char formatted_message[512];

    if (component == NULL) {
        component = "UNKNOWN";
    }

    if (message == NULL) {
        message = "Unspecified error";
    }

    snprintf(formatted_message, sizeof(formatted_message),
             "%s: %s", component, message);

    logger_log("ERROR", formatted_message);
}
