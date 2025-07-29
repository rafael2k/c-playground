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


#define BUFFER_SIZE 4096

// Resolve host to IP address (use IP directly if provided)
int get_ip_address(const char* host, char* ip_out, size_t ip_out_size) {
    struct in_addr addr;
    if (inet_pton(AF_INET, host, &addr) == 1) {
        strncpy(ip_out, host, ip_out_size);
        ip_out[ip_out_size - 1] = '\0';
        return 0;
    }

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

int https_get(const char* host, const char* ip, int port, const char* path, const char* bearer_token, int use_tls) {
    int ret = 1;
    int sockfd = -1;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;

    mbedtls_net_init(&net_ctx);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     NULL, 0)) != 0) {
        fprintf(stderr, "Failed to seed RNG: -0x%04x\n", -ret);
        goto cleanup;
    }

    if ((ret = mbedtls_net_connect(&net_ctx, ip, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
        fprintf(stderr, "Connection failed: -0x%04x\n", -ret);
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

        mbedtls_ssl_set_hostname(&ssl, host);
        mbedtls_ssl_set_bio(&ssl, &net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

        if ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            fprintf(stderr, "SSL handshake failed: -0x%04x\n", -ret);
            goto cleanup;
        }
    }

    // Compose GET request
    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Authorization: Bearer %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host, bearer_token);

    ssize_t sent = use_tls
        ? mbedtls_ssl_write(&ssl, (const unsigned char*)request, strlen(request))
        : write(net_ctx.fd, request, strlen(request));

    if (sent < 0) {
        fprintf(stderr, "Failed to send request: -0x%04x\n", -(int)sent);
        goto cleanup;
    }

    // Read and print response
    char buffer[BUFFER_SIZE];
    int n;
    while ((n = use_tls
                ? mbedtls_ssl_read(&ssl, (unsigned char*)buffer, sizeof(buffer) - 1)
                : read(net_ctx.fd, buffer, sizeof(buffer) - 1)) > 0) {
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
    mbedtls_net_free(&net_ctx);
    return ret;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <http|https> <host> <path> <bearer_token>\n", argv[0]);
        return 1;
    }

    int use_tls = strcmp(argv[1], "https") == 0;
    const char* host = argv[2];
    const char* path = argv[3];
    const char* token = argv[4];
    int port = use_tls ? 443 : 80;

    char ip[INET_ADDRSTRLEN];
    if (get_ip_address(host, ip, sizeof(ip)) < 0) {
        fprintf(stderr, "Failed to resolve IP for host: %s\n", host);
        return 1;
    }

    return https_get(host, ip, port, path, token, use_tls);
}
