#ifndef MCCLIENT_UTILS_H
#define MCCLIENT_UTILS_H

#include "packet.h"

void display_packet (packet p);

void display_server_response (int packet_id, packet_data pd);

#endif //MCCLIENT_UTILS_H
