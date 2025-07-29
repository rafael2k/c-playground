#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

#define BUFFER_SIZE 16384

// -- IP resolution helper: supports dotted IP or hostname via gethostbyname --
int get_ip_address(const char* host, char* ip_out, size_t ip_out_size) {
    struct in_addr addr;

    // Try IP directly
    if (inet_pton(AF_INET, host, &addr) == 1) {
        strncpy(ip_out, host, ip_out_size);
        ip_out[ip_out_size - 1] = '\0';
        return 0;
    }

    // Try hostname resolution
    struct hostent* he = gethostbyname(host);
    if (!he || he->h_addrtype != AF_INET || !he->h_addr_list[0]) {
        return -1;
    }

    addr = *(struct in_addr*)he->h_addr_list[0];
    if (!inet_ntop(AF_INET, &addr, ip_out, ip_out_size)) {
        return -1;
    }

    return 0;
}

// -- TCP connection with resolved IP --
int create_tcp_connection(const char* ip_str, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// -- HTTP/HTTPS GET Request --
int https_get(const char* host, const char* ip, int port, const char* path, const char* bearer_token, int use_ssl) {
    int sockfd = -1;
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL* ssl = NULL;
    int ret = 1;

    sockfd = create_tcp_connection(ip, port);
    if (sockfd < 0) {
        fprintf(stderr, "Connection to %s (%s) failed.\n", host, ip);
        return 1;
    }

    if (use_ssl) {
        wolfSSL_Init();
        ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
        if (!ctx) {
            fprintf(stderr, "wolfSSL_CTX_new failed\n");
            goto cleanup;
        }

        ssl = wolfSSL_new(ctx);
        if (!ssl) {
            fprintf(stderr, "wolfSSL_new failed\n");
            goto cleanup;
        }

        wolfSSL_set_fd(ssl, sockfd);
        if (wolfSSL_connect(ssl) != WOLFSSL_SUCCESS) {
            fprintf(stderr, "wolfSSL_connect failed\n");
            goto cleanup;
        }
    }

    // Build request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Authorization: Bearer %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host, bearer_token);

    int sent = use_ssl
        ? wolfSSL_write(ssl, request, strlen(request))
        : send(sockfd, request, strlen(request), 0);

    if (sent <= 0) {
        fprintf(stderr, "Failed to send request\n");
        goto cleanup;
    }

    char buffer[BUFFER_SIZE];
    int n;
    while ((n = use_ssl
                ? wolfSSL_read(ssl, buffer, sizeof(buffer) - 1)
                : recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        fputs(buffer, stdout);
    }

    ret = 0;

cleanup:
    if (ssl) wolfSSL_free(ssl);
    if (ctx) wolfSSL_CTX_free(ctx);
    if (use_ssl) wolfSSL_Cleanup();
    if (sockfd >= 0) close(sockfd);
    return ret;
}

// -- Main Program --
int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <http|https> <host> <path> <bearer_token>\n", argv[0]);
        return 1;
    }

    int use_ssl = strcmp(argv[1], "https") == 0;
    const char* host = argv[2];
    const char* path = argv[3];
    const char* token = argv[4];
    int port = use_ssl ? 443 : 80;

    char ip[INET_ADDRSTRLEN];
    if (get_ip_address(host, ip, sizeof(ip)) < 0) {
        fprintf(stderr, "Failed to resolve IP for host: %s\n", host);
        return 1;
    }

    return https_get(host, ip, port, path, token, use_ssl);
}
