#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "connection_handler.h"
#include "../http/http_parser.h"
#include "../http/forwarder.h"
#include "../security/access_control.h"
#include "../monitoring/logger.h"
#include "../monitoring/error_handler.h"
#include "../monitoring/performance.h"

#define BUFFER_SIZE 4096
#define CLIENT_TIMEOUT_SECONDS 10

static int send_all_client(int fd, const char *data, size_t length)
{
    size_t sent = 0;

    while (sent < length) {
        ssize_t n = send(fd, data + sent, length - sent, 0);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (n == 0)
            return -1;

        sent += (size_t)n;
    }

    return 0;
}

static void send_http_error(int fd, const char *status,
                            const char *body)
{
    char header[512];
    size_t body_length = strlen(body);

    int n = snprintf(header, sizeof(header),
        "HTTP/1.1 %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n",
        status, body_length);

    if (n > 0 && (size_t)n < sizeof(header)) {
        if (send_all_client(fd, header, (size_t)n) == 0)
            send_all_client(fd, body, body_length);
    }
}

void *handle_client(void *arg)
{
    double start_time = performance_start();
    int client_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    struct timeval timeout = {
        .tv_sec = CLIENT_TIMEOUT_SECONDS,
        .tv_usec = 0
    };

    if (arg == NULL) {
        report_error("NETWORK", "Null client argument");
        return NULL;
    }

    client_fd = *(int *)arg;
    free(arg);

    logger_log("INFO", "Client handler started");

    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) < 0) {
        report_error("NETWORK", "Failed to set receive timeout");
        goto cleanup;
    }

    bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0) {
        if (bytes_received < 0)
            report_error("NETWORK", "Failed to receive client request");
        else
            logger_log("INFO", "Client disconnected");

        goto cleanup;
    }

    buffer[bytes_received] = '\0';

    printf("[REQUEST] Received %zd bytes:\n%s\n",
           bytes_received, buffer);

    HttpRequest request;

    if (parse_http_request(buffer, &request) != 0) {
        logger_log("WARN", "Invalid HTTP request");
        send_http_error(client_fd, "400 Bad Request",
                        "Invalid HTTP request\n");
        goto cleanup;
    }

    if (!is_host_allowed(request.host)) {
        logger_log("WARN", "Request rejected by access control");
        send_http_error(client_fd, "403 Forbidden",
                        "Destination blocked by proxy\n");
        goto cleanup;
    }

    char message[384];
    snprintf(message, sizeof(message),
             "Forwarding %.15s request to %.200s:%d%.80s",
             request.method, request.host, request.port, request.path);
    logger_log("INFO", message);

    if (forward_http_request(&request, buffer, client_fd) < 0) {
        report_error("HTTP", "Request forwarding failed");
        send_http_error(client_fd, "502 Bad Gateway",
                        "Proxy could not complete the request\n");
    }

cleanup:
    close(client_fd);

    double elapsed = performance_elapsed_ms(start_time);
    if (elapsed >= 0.0) {
        char timing_message[128];
        snprintf(timing_message, sizeof(timing_message),
                 "Client handler completed in %.2f ms", elapsed);
        logger_log("INFO", timing_message);
    }

    logger_log("INFO", "Client connection closed");
    return NULL;
}
