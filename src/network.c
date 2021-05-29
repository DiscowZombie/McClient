#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "network.h"

#define DEFAULT_REC_BUF_SIZE 1024

int tcp_connect_v4 (char * ip, short port)
{
    // Ouverture socket TCP en IPv4
    int sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons (port);
    if (inet_pton (AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror ("inet_pton");
        exit (EXIT_FAILURE);
    }

    if (connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
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