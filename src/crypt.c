#include <time.h>
#include <stdlib.h>
#include "crypt.h"
#include "packet.h"

char * generate_16_byte_shared_secret ()
{
    char * shared_secret;
    MALCHK(shared_secret = malloc (16 * sizeof (char)));

    for (int i = 0; i < 16; ++i) {
        shared_secret[i] = rand () % 256;
    }

    return shared_secret;
}

char * generate_sha1_server_hash (char * server_id, int server_id_length,
                                  char * shared_secret, int shared_secret_length,
                                  char * server_public_key, int server_public_key_length)
{
    // SHA1 produce a 20-byte hash

    (void) server_id;
    (void) server_id_length;
    (void) shared_secret;
    (void) shared_secret_length;
    (void) server_public_key;
    (void) server_public_key_length;

    // TODO Mojang sha1 string
    // TODO Go Ex : https://gist.github.com/toqueteos/5372776
    // TODO : Test online : https://play.golang.org/p/hhayRT1VWgj

    return NULL;
}