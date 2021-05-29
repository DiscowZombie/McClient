#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include "network.h"

#define DEFAULT_REC_BUF_SIZE 1024

int tcp_connect (unsigned char ipv4, char * ip, short port)
{
    // Ouverture de la socket (soit en IPv4, soit en IPv6)
    int sock = socket (ipv4 ? AF_INET : AF_INET6, SOCK_STREAM, 0);
    if (sock == -1) {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    struct sockaddr_in server_addr4;
    struct sockaddr_in6 server_addr6;
    int res;
    if (ipv4) {
        memset (&server_addr4, 0, sizeof (server_addr4));
        server_addr4.sin_family = AF_INET; // IPv4
        server_addr4.sin_port = htons (port);
        res = inet_pton (AF_INET, ip, &server_addr4.sin_addr); // Convert IP param to valid IPv4
    } else {
        memset (&server_addr6, 0, sizeof (server_addr6));
        server_addr6.sin6_family = AF_INET6; // IPv6
        server_addr6.sin6_port = htons (port);
        res = inet_pton (AF_INET6, ip, &server_addr6.sin6_addr); // Convert IP param to valid IPv6
    }
    if (res <= 0) {
        perror ("inet_pton");
        exit (EXIT_FAILURE);
    }

    if (connect (sock, (ipv4 ? (struct sockaddr *) &server_addr4 : (struct sockaddr *) &server_addr6),
                 (ipv4 ? (sizeof (server_addr4)) : (sizeof (server_addr6)))) < 0) {
        perror ("connect");
        exit (EXIT_FAILURE);
    }

    return sock;
}

void display_hex_server_response (int socket)
{
    char buf[DEFAULT_REC_BUF_SIZE];
    ssize_t read_size;

    do {
        if ((read_size = read (socket, buf, DEFAULT_REC_BUF_SIZE)) < 0) {
            perror ("read");
            exit (EXIT_FAILURE);
        }
        // Affichage de la réponse
        for (int i = 0; i < read_size; ++i)
            printf ("%.2x ", buf[i]);
        printf ("\n");
    } while (read_size == DEFAULT_REC_BUF_SIZE);
}