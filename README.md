# Computer Networks Proxy Server

A multithreaded HTTP forward proxy server developed in **C** as a Computer Networks mini-project. The project demonstrates TCP socket programming, concurrent client handling, HTTP request parsing and forwarding, response caching, domain access control, logging, error handling, and performance monitoring.

## Overview

A proxy server acts as an intermediary between clients and destination servers. Instead of accessing a destination directly, a client sends an HTTP request to the proxy. The proxy processes the request, checks its access-control rules, checks the cache where applicable, forwards eligible requests to the destination, and returns the response to the client.

This project implements a modular HTTP proxy using POSIX sockets and pthreads on Linux.

## Features

- **TCP socket programming:** Accepts incoming client connections.
- **Concurrent client handling:** Uses POSIX threads to process multiple client connections.
- **HTTP request parsing:** Extracts request information, including the destination host, port, and path.
- **HTTP request forwarding:** Connects to the destination server and relays HTTP responses.
- **Response caching:** Includes an in-memory cache to reduce repeated upstream requests for eligible responses.
- **Domain access control:** Supports blocking and unblocking destination hostnames.
- **Logging:** Records proxy events and errors to the terminal and a log file.
- **Error handling:** Reports request-processing and connection failures.
- **Performance monitoring:** Measures request-processing time and tracks request statistics.

## Technologies Used

- C
- Linux / Ubuntu
- TCP/IP sockets
- POSIX threads (`pthread`)
- HTTP
- GCC compiler
- Git and GitHub

## Project Structure

```text
computer-networks-proxy-server/
├── http/
│   ├── forwarder.c
│   ├── forwarder.h
│   ├── http_parser.c
│   └── http_parser.h
├── monitoring/
│   ├── error_handler.c
│   ├── error_handler.h
│   ├── logger.c
│   ├── logger.h
│   ├── performance.c
│   └── performance.h
├── networking/
│   ├── connection_handler.c
│   ├── connection_handler.h
│   └── server.c
├── security/
│   ├── access_control.c
│   ├── access_control.h
│   ├── cache.c
│   └── cache.h
├── .gitignore
└── README.md
```

### Module Responsibilities

| Module | Responsibility |
|---|---|
| `networking/` | Starts the server, accepts client connections, and manages client handlers. |
| `http/` | Parses HTTP requests and forwards them to destination servers. |
| `security/access_control.c` | Manages destination-host blocking rules. |
| `security/cache.c` | Stores and retrieves eligible cached responses. |
| `monitoring/logger.c` | Records operational events and errors. |
| `monitoring/error_handler.c` | Provides structured error reporting. |
| `monitoring/performance.c` | Measures request-processing time and maintains performance statistics. |

## How It Works

1. The server starts and listens for client connections.
2. A client establishes a TCP connection to the proxy.
3. A worker thread receives and processes the client's HTTP request.
4. The HTTP parser extracts the destination details.
5. The access-control module checks whether the destination is allowed.
6. The proxy checks for an eligible cached response.
7. On a cache miss, the forwarding module connects to the destination server and requests the resource.
8. The proxy returns the response to the client.
9. Logging and performance-monitoring modules record relevant events and timings.

## Prerequisites

Ensure the following are installed on Ubuntu or another suitable Linux environment:

- GCC
- POSIX threads support
- Git
- An active network connection for external HTTP forwarding tests

Install GCC and Git on Ubuntu:

```bash
sudo apt update
sudo apt install build-essential git -y
```

## Compilation

Clone the repository and navigate into the project directory:

```bash
git clone https://github.com/IzzatGoutam/computer-networks-proxy-server.git
cd computer-networks-proxy-server
```

Compile the source files with GCC:

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 \
  networking/server.c \
  networking/connection_handler.c \
  http/http_parser.c \
  http/forwarder.c \
  security/access_control.c \
  security/cache.c \
  monitoring/logger.c \
  monitoring/error_handler.c \
  monitoring/performance.c \
  -pthread -o proxy_server
```

If compilation succeeds, the executable `proxy_server` will be created in the project directory.

## Running the Proxy Server

Start the server:

```bash
./proxy_server
```

The current server configuration uses the local address `127.0.0.1` and port `8888`.

Keep the terminal running while testing the proxy. Use `Ctrl+C` to stop the server.

## Testing

### 1. Test HTTP Forwarding

Open another terminal and run:

```bash
curl --noproxy '' --max-time 15 -v \
  -x http://127.0.0.1:8888 \
  http://example.com/
```

A successful test should show an HTTP response from the destination server.

### 2. Test Domain Blocking

The current integration includes a demonstration rule for `blocked.example`.

Run:

```bash
curl -i --max-time 5 \
  -x http://127.0.0.1:8888 \
  http://blocked.example/
```

The expected result is:

```http
HTTP/1.1 403 Forbidden
```

The proxy should reject the blocked destination before attempting to forward the request.

### 3. Test Caching

Repeat an eligible HTTP GET request:

```bash
curl --noproxy '' --max-time 15 \
  -x http://127.0.0.1:8888 \
  http://example.com/
```

Run the same command again. Depending on the response's cache eligibility and freshness, the second request may be served from the cache.

### 4. Test Concurrent Requests

With the proxy running, execute:

```bash
for i in 1 2 3 4; do
  curl --noproxy '' --max-time 15 -sS \
    -x http://127.0.0.1:8888 \
    http://example.com/ -o "/tmp/proxy-test-$i.html" &
done

wait
```

Inspect the proxy terminal to observe the handling of multiple client requests.

### 5. Inspect Logs

The proxy initializes its log file as `proxy.log`.

Inspect recent entries with:

```bash
tail -n 30 proxy.log
```

To follow new entries as they are written:

```bash
tail -f proxy.log
```

## Error Handling

The proxy reports errors encountered during request processing and upstream communication. Depending on the failure, it can return appropriate HTTP error responses, including:

- `400 Bad Request` — an invalid or malformed request.
- `403 Forbidden` — a destination rejected by access-control rules.
- `502 Bad Gateway` — an upstream connection or forwarding failure.

The exact response depends on the error-handling path implemented in the corresponding source module.

## Limitations

- The current implementation targets plain HTTP requests.
- HTTPS `CONNECT` tunnelling and end-to-end TLS proxying are not implemented.
- The access-control rules and cache are in-memory components; their state does not persist across server restarts.
- Cache effectiveness depends on the response and cache policy.
- Performance depends on destination-server responsiveness, network conditions, and concurrent load.

## Learning Outcomes

This project provides practical experience with:

- TCP client-server communication.
- POSIX socket APIs and multithreading.
- HTTP request parsing and response forwarding.
- Concurrent access to shared data.
- Caching strategies and access-control mechanisms.
- Logging, error handling, and performance measurement.
- Collaborative software development using Git and GitHub.

## Team Contributions

This project was developed collaboratively by a four-member team:

- **Izzat Goutam** — Networking integration and overall project integration.
- **Akruti Panda** — HTTP parsing and forwarding module.
- **Sneha Mohapatra** — Security and caching module.
- **Amrit Aryan Sahoo** — Monitoring, logging, error handling, and performance tracking.

## Academic Context

**Project:** Computer Networks Mini-Project  
**Topic:** HTTP Forward Proxy Server  
**Language:** C  
**Platform:** Linux / Ubuntu

---

Developed as an academic project to demonstrate practical concepts in computer networking, concurrent programming, HTTP communication, caching, access control, and monitoring.
