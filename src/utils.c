#include <stdio.h>
#include "utils.h"

void display_packet (packet p)
{
    printf ("Packet (size = %u): ", p->size);
    for (size_t i = 0; i < p->size; ++i)
        printf ("%.2x ", p->data[i]);
    printf ("\n");
}