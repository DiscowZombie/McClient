#include <stdio.h>
#include "utils.h"

void display_packet (packet p)
{
    printf ("Packet (size = %u): ", p->size);
    for (size_t i = 0; i < p->size; ++i)
        printf ("%.2x ", p->data[i]);
    printf ("\n");
}

void display_server_response (int packet_id, packet_data pd)
{
    printf ("packet_id = %i (size = %u): ", packet_id, pd->size);
    for (size_t i = 0; i < pd->size; ++i)
        printf ("%.2x ", pd->data[i]);
    printf ("\n");
}