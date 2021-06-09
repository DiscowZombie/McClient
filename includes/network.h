#ifndef MCCLIENT_NETWORK_H
#define MCCLIENT_NETWORK_H

#include "packet.h"

int tcp_connect (unsigned char ipv4, char * ip, short port);

void send_packet (int socket, packet p);

void display_hex_server_response (int socket);

#endif //MCCLIENT_NETWORK_H
