#ifndef MCCLIENT_NETWORK_H
#define MCCLIENT_NETWORK_H

int tcp_connect_v4 (char * ip, short port);

void display_hex_server_response(int socket);

#endif //MCCLIENT_NETWORK_H
