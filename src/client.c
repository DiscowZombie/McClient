#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <network.h>
#include "crypt.h"
#include "packet.h"
#include "utils.h"
#include "login_packet.h"

// Protocol ID for Minecraft 1.16.5 : 754
#define MINECRAFT_1_16_5 754

int main ()
{
    int packet_id;
    packet_data response;

    // Ouverture d'une connexion TCP IPv6 avec le serveur MC sur ::1, port 25665
    int sock = tcp_connect (0, "::1", 25565);

    // STEP 1 : C->S Handshake paquet
    packet cs_handshake = craft_cs_handshake ("localhost", 25565, MINECRAFT_1_16_5);
    display_packet (cs_handshake);
    send_packet (sock, cs_handshake);

    // STEP 2 : C->S Login Start
    packet cs_login_start = craft_cs_login_start ("DiscowZombie");
    display_packet (cs_login_start);
    send_packet (sock, cs_login_start);

    // STEP 3 : S->C Encryption Request / Read packet from the server
    response = read_server_response (sock, NULL, &packet_id);
    if (packet_id != 1) {
        printf ("Read a valid packet from server with ID != 1: %u.\n", packet_id);
        return EXIT_FAILURE;
    }

    int server_id_length, public_key_length = 0, verify_token_length = 0;
    char * server_id, * public_key, * verify_token;
    read_encryption_request (response, &server_id, &server_id_length, &public_key, &public_key_length, &verify_token,
                             &verify_token_length);

    // Display Server ID
    printf ("Server ID (length = %u): ", server_id_length);
    for (int i = 0; i < server_id_length; ++i)
        printf ("%c ", server_id[i]);
    printf ("\n");

    // Display public key
    printf ("Public key (length = %u): ", public_key_length);
    for (int i = 0; i < public_key_length; ++i)
        printf ("%.2x ", public_key[i]);
    printf ("\n");

    // Display verify token
    printf ("Verify token (length = %u): ", verify_token_length);
    for (int i = 0; i < verify_token_length; ++i)
        printf ("%.2x ", verify_token[i]);
    printf ("\n");

    // PREPARE STEP 4

    // Generate a shared secret
    char * shared_secret = generate_16_byte_shared_secret ();
    // Display random shared secret
    printf ("Random shared secret (length = 16): ");
    for (int i = 0; i < 16; ++i)
        printf ("%.2x ", shared_secret[i]);
    printf ("\n");

    // Generate a SHA1 hash : "serverId" field on the POST request to the Mojang auth servers
    char * sha1_server_hash = generate_sha1_server_hash (server_id, server_id_length, shared_secret, 16, public_key,
                                                         public_key_length);

    // STEP 4 : Client auth (Send a POST request to Mojang)

    // TODO Sending a POST request over HTTPS is HARD on C !


    // STEP 5 : C->S Encryption Response
    packet cs_encryption_response = craft_cs_encryption_response (shared_secret, 16, verify_token, verify_token_length);
    display_packet (cs_encryption_response);
    send_packet (sock, cs_encryption_response);

    // STEP 6 : Server will send a request to Mojang. Nothing to do.

    // STEP 7 : S->C Set Compression : Optional, ignored while testing on local. Nothing to do for now.

    // STEP 8 : S->C Login Success
    response = read_server_response (sock, NULL, &packet_id);
    if (packet_id != 2) {
        printf ("Read a valid packet from server with ID != 2: %u.\n", packet_id);
        return EXIT_FAILURE;
    }

    unsigned long uuid_most, uuid_least;
    int username_length;
    char * username;
    read_login_success (response, &uuid_most, &uuid_least, &username, &username_length);
    printf ("UUID = %lu %lu, username = %s\n", uuid_most, uuid_least, username);

    // From wiki.vg : The (notchian) server might take a bit to fully transition to the Play state, so it's recommended to wait before sending Play packets,
    sleep (1);

    // Now we can send play packets!
    // TODO: Impl Play packets

    // This pointer was malloc'd by read_login_success call
    free (username);

    // This pointer was malloc'd by generate_sha1_server_hash call
    free (sha1_server_hash);

    // This pointer was malloc'd by generate_16_byte_shared_secret call
    free (shared_secret);

    // Those pointers were malloc'd by read_encryption_request call
    free (verify_token);
    free (public_key);
    free (server_id);

    // Close the socket
    if (close (sock) == -1) {
        perror ("close");
        exit (EXIT_FAILURE);
    }

    return 0;
}