#ifndef MCCLIENT_CRYPT_H
#define MCCLIENT_CRYPT_H

char * generate_16_byte_shared_secret ();

char * generate_sha1_server_hash (char * server_id, int server_id_length,
                                  char * shared_secret, int shared_secret_length,
                                  char * server_public_key, int server_public_key_length);

#endif //MCCLIENT_CRYPT_H
