#include <packet.h>
#include <malloc.h>
#include "login_packet.h"

packet craft_cs_handshake (char * server_addr, short server_port, int protocol_version)
{
    // 1. C->S: Handshake
    packet_data cs_handshake_data = new_data ();
    write_var_int (cs_handshake_data, protocol_version);
    write_string (cs_handshake_data, server_addr);
    write_unsigned_short (cs_handshake_data, server_port);
    write_var_int (cs_handshake_data, 2); // Next State set to 2 for Login

    packet cs_handshake = to_uncompressed_packet (0, cs_handshake_data);

    free_packet_data (cs_handshake_data);

    return cs_handshake;
}

packet craft_cs_login_start (char * username)
{
    // 2. C->S: Login Start
    packet_data cs_login_start_data = new_data ();
    write_string (cs_login_start_data, username);

    packet cs_login_start = to_uncompressed_packet (0, cs_login_start_data);

    free_packet_data (cs_login_start_data);

    return cs_login_start;
}

void read_encryption_request (packet_data pd,
                              char * _server_id[], int * _server_id_length,
                              char * _public_key[], int * _public_key_length,
                              char * _verify_token[], int * _verify_token_length)
{
    // 3. S->C : Encryption Request

    // Lecture du string "Server ID"
    int strlen_octets = 0;
    int strlen = read_var_int (pd->data, 0, &strlen_octets);
    char * server_id;
    MALCHK(server_id = malloc (strlen * sizeof (char)));
    for (int i = 0; i < strlen; ++i) {
        server_id[i] = pd->data[strlen_octets + i];
    }

    // Lecture de la public key
    int public_key_octets = 0;
    int public_key_length = read_var_int (pd->data, strlen_octets + strlen, &public_key_octets);
    char * public_key;
    MALCHK(public_key = malloc (public_key_length * sizeof (char)));
    for (int i = 0; i < public_key_length; ++i) {
        public_key[i] = pd->data[strlen_octets + strlen + public_key_octets + i];
    }

    // Lecture du verify token
    int verify_token_octets = 0;
    int verify_token_length = read_var_int (pd->data,
                                            strlen_octets + strlen + public_key_octets + public_key_length,
                                            &verify_token_octets);
    char * verify_token;
    MALCHK(verify_token = malloc (verify_token_length * sizeof (char)));
    for (int i = 0; i < verify_token_length; ++i) {
        verify_token[i] = pd->data[strlen_octets + strlen + public_key_octets + public_key_length +
                                   verify_token_octets + i];
    }

    free_packet_data (pd);

    // Gestion des paramètres de retour
    if (_server_id != NULL) {
        *_server_id = server_id;
        *_server_id_length = strlen;
    } else {
        free (server_id);
    }

    if (_public_key != NULL) {
        *_public_key = public_key;
        *_public_key_length = public_key_length;
    } else {
        free (public_key);
    }

    if (_verify_token != NULL) {
        *_verify_token = verify_token;
        *_verify_token_length = verify_token_length;
    } else {
        free (verify_token);
    }
}

packet craft_cs_encryption_response (char * shared_secret, int shared_secret_length, char * verify_token,
                                     int verify_token_length)
{
    // 5. C->S: Encryption Response
    packet_data cs_encryption_response_data = new_data ();
    write_var_int (cs_encryption_response_data, shared_secret_length);
    write_bytes (cs_encryption_response_data, shared_secret, shared_secret_length);
    write_var_int (cs_encryption_response_data, verify_token_length);
    write_bytes (cs_encryption_response_data, verify_token, verify_token_length);

    packet cs_encryption_response = to_uncompressed_packet (1, cs_encryption_response_data);

    free_packet_data (cs_encryption_response_data);

    return cs_encryption_response;
}

void read_login_success (packet_data pd, unsigned long * uuid_most, unsigned long * uuid_least, char * username[],
                         int * username_length)
{
    // 8. S->C : Login Success
    read_uuid (pd, 0, uuid_most, uuid_least);
    read_string (pd, 16, username, username_length);

    free_packet_data (pd);
}