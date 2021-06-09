#include <string.h>
#include <netinet/in.h>
#include <stdnoreturn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "packet.h"

#define READ_BUF_SIZE 1024

noreturn void raler (const char * msg)
{
    perror (msg);
    exit (EXIT_FAILURE);
}

packet_data new_data ()
{
    packet_data pd;

    MALCHK(pd = malloc (sizeof (struct s_packet_data)));
    pd->data = NULL;
    pd->size = 0;

    return pd;
}

static void increase_packet_data_size (packet_data p, unsigned int octets)
{
    // Nous devons agrandir la taille des données de (octets) octets
    size_t required_size = p->size + octets;

    MALCHK(p->data = realloc (p->data, required_size));
    p->size = required_size;
}

static void increase_packet_size (packet p, unsigned int octets)
{
    // Nous devons agrandir la taille des données de (octets) octets
    size_t required_size = p->size + octets;

    MALCHK(p->data = realloc (p->data, required_size));
    p->size = required_size;
}

static void write_packet_var_int (packet p, int varint)
{
    // Taile d'un VarInt : Entre 1 et 5 octets (cf https://wiki.vg/Protocol#VarInt_and_VarLong)

    // MC paquet are big-endian (except for VarInt and VarLong)
    // Most OS are little-indian
    char temp;
    do {
        temp = varint & 0b01111111;
        varint >>= 7;
        if (varint != 0) {
            temp |= 0b10000000;
        }
        increase_packet_size (p, 1);
        p->data[p->size - 1] = temp;
    } while (varint != 0);
}

static size_t measure_packet_id_var_int_length (unsigned int paquet_id_varint)
{
    size_t count = 0;

    char temp;
    do {
        temp = paquet_id_varint & 0b01111111;
        paquet_id_varint >>= 7;
        if (paquet_id_varint != 0) {
            temp |= 0b10000000;
        }
        ++count;
    } while (paquet_id_varint != 0);

    return count;
}

static void write_packet_byte_array (packet p, packet_data data)
{
    unsigned int size_before = p->size;
    increase_packet_size (p, data->size);

    for (size_t i = size_before; i < p->size; ++i)
        p->data[i] = data->data[i - size_before];
}

void write_var_int (packet_data p, int varint)
{
    // Taile d'un VarInt : Entre 1 et 5 octets (cf https://wiki.vg/Protocol#VarInt_and_VarLong)

    // MC paquet are big-endian (except for VarInt and VarLong)
    // Most OS are little-indian
    char temp;
    do {
        temp = varint & 0b01111111;
        varint >>= 7;
        if (varint != 0) {
            temp |= 0b10000000;
        }
        increase_packet_data_size (p, 1);
        p->data[p->size - 1] = temp;
    } while (varint != 0);
}

int read_var_int (const char buf[], int start_idx, int * bytes_read)
{
    int result = 0, octets_read = 0;
    char byte;

    do {
        byte = buf[start_idx + octets_read];
        int value = (byte & 0b01111111);
        result |= (value << (7 * octets_read));

        octets_read++;
        if (octets_read > 5) {
            raler ("VarInt is too big.");
        }
    } while ((byte & 0b10000000) != 0);

    *bytes_read = octets_read;
    return result;
}

void write_string (packet_data p, char * str)
{
    int str_len = strlen (str);

    write_var_int (p, str_len); // Écrire la taille de la chaine de caractères

    // Il faut ensuite str_len octets pour écrire le string UTF8
    increase_packet_data_size (p, str_len);

    size_t offset = p->size - str_len;
    for (size_t i = 0; i < strlen (str); ++i) {
        p->data[offset + i] = str[i];
    }
}

void write_unsigned_short (packet_data p, unsigned short ushort)
{
    // Taille d'un UShort : 2 octets
    increase_packet_data_size (p, 2);

    // MC paquet are big-endian (except for VarInt and VarLong)
    // Most OS are little-indian

    // Copier ushort dans le paquet (little-indian -> big-indian)
    unsigned short big_indian = htons (ushort); // Host to network short mean. little-indian to big-indian
    p->data[p->size - 2] = big_indian & 0xff;
    p->data[p->size - 1] = (big_indian >> 8) & 0xff;
}

void write_bytes (packet_data p, char * bytes, int length)
{
    increase_packet_data_size (p, length);

    for (int i = 0; i < length; ++i) {
        p->data[p->size - length + i] = bytes[i];
    }
}

packet to_uncompressed_packet (unsigned int packet_id, packet_data data)
{
    packet p;

    MALCHK(p = malloc (sizeof (struct s_packet)));
    p->data = NULL;
    p->size = 0;

    write_packet_var_int (p, data->size + measure_packet_id_var_int_length (packet_id));
    write_packet_var_int (p, packet_id);

    write_packet_byte_array (p, data);

    return p;
}

packet_data read_server_response (int socket, int * length, int * packet_id)
{
    char buf[READ_BUF_SIZE];
    ssize_t read_size;
    packet_data pd = NULL;
    int len, pid, data_len, buf_data_start, last_write_index = 0;

    do {
        // Index de début des données du packet dans le buffer "buf"
        buf_data_start = 0;

        if ((read_size = read (socket, buf, READ_BUF_SIZE)) < 0) {
            perror ("read");
            exit (EXIT_FAILURE);
        }

        // Nous lisons le premier fragment : début du packet
        if (pd == NULL) {
            len = read_var_int (buf, 0, &buf_data_start);

            int bds = 0;
            pid = read_var_int (buf, buf_data_start, &bds);
            buf_data_start += bds; // taille en octet des deux VarInt

            data_len = len - bds;

            MALCHK(pd = malloc (sizeof (struct s_packet_data)));
            MALCHK(pd->data = malloc (data_len * sizeof (char)));
            pd->size = data_len;
        }

        for (int i = buf_data_start; i < read_size; ++i) {
            pd->data[last_write_index] = buf[i];
            ++last_write_index; // TODO: Opti = (read_size - buf_data_start) ?
        }
    } while (read_size == READ_BUF_SIZE);

    if (length != NULL)
        *length = len;
    if (packet_id != NULL)
        *packet_id = pid;

    return pd;
}

void read_uuid (packet_data p, int start_idx, unsigned long * uuid_most, unsigned long * uuid_least)
{
    unsigned long um = 0, ul = 0;

    for (int i = 0; i < 8; ++i) {
        um += p->data[start_idx + i] * (int) pow (2, 15 - i);
    }
    for (int i = 0; i < 8; ++i) {
        ul += p->data[start_idx + 8 + i] * (int) pow (2, 7 - i);
    }

    *uuid_most = um;
    *uuid_least = ul;
}

void read_string (packet_data p, int start_idx, char * username[], int * username_length)
{
    int bytes_read = 0;
    int str_len = read_var_int (p->data, start_idx, &bytes_read);

    char * u;
    MALCHK(u = malloc (str_len * sizeof (char)));

    for (int i = 0; i < str_len; ++i) {
        u[i] = p->data[start_idx + bytes_read + i];
    }

    *username = u;
    *username_length = str_len;
}

void free_packet_data (packet_data p)
{
    free (p->data);
    free (p);
}

void free_packet (packet p)
{
    free (p->data);
    free (p);
}