#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <network.h>
#include "packet.h"
#include "utils.h"

int main ()
{
    // Ouverture d'une connexion TCP IPv6 avec le serveur MC sur ::1, port 25665
    int sock = tcp_connect (0, "::1", 25565);

    // Création d'un paquet C->S Handshake
    packet_data cs_handshake_data = new_data ();
    write_var_int (cs_handshake_data, 754); // 754 : Minecraft 1.16.5
    write_string (cs_handshake_data, "localhost");
    write_unsigned_short (cs_handshake_data, 25565);
    write_var_int (cs_handshake_data, 2);

    packet cs_handshake = to_uncompressed_packet (0, cs_handshake_data);
    display_packet (cs_handshake);

    // Envoie du paquet C->S handshake
    if (send (sock, cs_handshake->data, cs_handshake->size, 0) < 0) {
        perror ("send C->S Handshake");
        return EXIT_FAILURE;
    }

    free_packet_data (cs_handshake_data);
    free_packet (cs_handshake);

    // Création d'un paquet C->S Login Start
    packet_data cs_login_start_data = new_data ();
    write_string (cs_login_start_data, "DiscowZombie");

    packet cs_login_start = to_uncompressed_packet (0, cs_login_start_data);
    display_packet (cs_login_start);

    // Envoie du paquet C->S Login Start
    if (send (sock, cs_login_start->data, cs_login_start->size, 0) < 0) {
        perror ("send C->S Login Start");
        return EXIT_FAILURE;
    }

    free_packet_data (cs_login_start_data);
    free_packet (cs_login_start);

    // Je dois recevoir une "Encryption Request" du serveur
    display_hex_server_response (sock);

    // Fermeture de la socket
    if (close (sock) == -1) {
        perror ("close");
        exit (EXIT_FAILURE);
    }

    return 0;
}