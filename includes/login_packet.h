#ifndef MCCLIENT_LOGIN_PACKET_H
#define MCCLIENT_LOGIN_PACKET_H

packet craft_cs_handshake (char * server_addr, short server_port, int protocol_version);

packet craft_cs_login_start (char * username);

void read_encryption_request (packet_data pd, char * server_id[], int * server_id_length, char * public_key[],
                              int * public_key_length, char * verify_token[], int * verify_token_length);

packet craft_cs_encryption_response (char * shared_secret, int shared_secret_length, char * verify_token,
                                     int verify_token_length);

void read_login_success (packet_data pd, unsigned long * uuid_most, unsigned long * uuid_least, char * username[],
                         int * username_length);

#endif //MCCLIENT_LOGIN_PACKET_H
