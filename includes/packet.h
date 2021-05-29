#ifndef MCCLIENT_PACKET_H
#define MCCLIENT_PACKET_H

typedef struct s_packet_data
{
    unsigned int size;
    char * data;
} * packet_data;

typedef struct s_packet
{
    unsigned int size;
    char * data;
} * packet;

packet_data new_data ();

void write_var_int (packet_data p, int varint);

void write_string (packet_data p, char * str);

void write_unsigned_short (packet_data p, unsigned short ushort);

packet to_uncompressed_packet (unsigned int packet_id, packet_data data);

void free_packet_data(packet_data p);

void free_packet(packet p);

#endif //MCCLIENT_PACKET_H
