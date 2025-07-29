#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"


#define BUFFER_SIZE 16384


// Very basic check for valid IPv4 dotted quad
int is_valid_ipv4(const char *ip) {
    struct in_addr addr;
    return inet_pton(AF_INET, ip, &addr) == 1;
}

int connect_tcp(const char* ip, int port) {
    int sockfd;
    struct sockaddr_in server;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) {
        perror("inet_pton()");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect()");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int https_get(const char* ip, int port, const char* host, const char* path, const char* bearer_token, int use_tls) {
    int ret = 1;
    int sockfd = connect_tcp(ip, port);
    if (sockfd < 0) return 1;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     NULL, 0)) != 0) {
        fprintf(stderr, "RNG seed failed: -0x%04x\n", -ret);
        goto cleanup;
    }

    if (use_tls) {
        if ((ret = mbedtls_ssl_config_defaults(&conf,
                                               MBEDTLS_SSL_IS_CLIENT,
                                               MBEDTLS_SSL_TRANSPORT_STREAM,
                                               MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
            fprintf(stderr, "SSL config failed: -0x%04x\n", -ret);
            goto cleanup;
        }

        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

        if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
            fprintf(stderr, "SSL setup failed: -0x%04x\n", -ret);
            goto cleanup;
        }

        mbedtls_ssl_set_hostname(&ssl, host);  // Needed for SNI
        mbedtls_ssl_set_bio(&ssl, &sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);

        if ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            fprintf(stderr, "SSL handshake failed: -0x%04x\n", -ret);
            goto cleanup;
        }
    }

    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Authorization: Bearer %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host, bearer_token);

    ssize_t sent = use_tls
        ? mbedtls_ssl_write(&ssl, (const unsigned char*)request, strlen(request))
        : write(sockfd, request, strlen(request));

    if (sent < 0) {
        fprintf(stderr, "Request send failed: -0x%04x\n", -(int)sent);
        goto cleanup;
    }

    char buffer[BUFFER_SIZE];
    int n;
    while ((n = use_tls
                ? mbedtls_ssl_read(&ssl, (unsigned char*)buffer, sizeof(buffer) - 1)
                : read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        fputs(buffer, stdout);
    }

    ret = 0;

cleanup:
    if (use_tls) {
        mbedtls_ssl_close_notify(&ssl);
        mbedtls_ssl_free(&ssl);
        mbedtls_ssl_config_free(&conf);
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    close(sockfd);
    return ret;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <http|https> <ip> <port> <path> <bearer_token>\n", argv[0]);
        return 1;
    }

    int use_tls = strcmp(argv[1], "https") == 0;
    const char* ip = argv[2];
    const char* port_str = argv[3];
    const char* path = argv[4];
    const char* token = argv[5];
    int port = atoi(port_str);

    if (!is_valid_ipv4(ip)) {
        fprintf(stderr, "Invalid IP address format: %s\n", ip);
        return 1;
    }

    // Use IP for socket connect, but still send correct Host header for TLS SNI and HTTP
    const char* fake_host = "example.com";  // Replace with correct host if needed

    return https_get(ip, port, fake_host, path, token, use_tls);
}
