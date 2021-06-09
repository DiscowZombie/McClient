#ifndef MCCLIENT_PACKET_H
#define MCCLIENT_PACKET_H

#define MALCHK(res)   do { if ((res) == NULL) { raler (#res); } } while (0)

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

void raler (const char * msg);

packet_data new_data ();

void write_var_int (packet_data p, int varint);

// TODO: Implementation should not take start_idx, called should use pointer arithmetics to define proper start. Eg: *(buf + 4)
int read_var_int (const char buf[], int start_idx, int * bytes_read);

void write_string (packet_data p, char * str);

void write_unsigned_short (packet_data p, unsigned short ushort);

void write_bytes (packet_data p, char * bytes, int length);

void read_uuid (packet_data p, int start_idx, unsigned long * uuid_most, unsigned long * uuid_least);

void read_string (packet_data p, int start_idx, char * username[], int * username_length);

packet to_uncompressed_packet (unsigned int packet_id, packet_data data);

packet_data read_server_response (int socket, int * length, int * packet_id);

void free_packet_data (packet_data p);

void free_packet (packet p);

#endif //MCCLIENT_PACKET_H
