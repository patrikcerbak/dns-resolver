/*
 * Author: Patrik Cerbak
 * Login: xcerba00
 * 
 * Parts of the code in this file are inspired by the second lecture of ISA,
 * Pokrocile programovani sitovych aplikaci TCP/IP by Petr Matousek,
 * https://www.fit.vut.cz/study/course/ISA/.cs
 * 
*/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "arguments.h"
#include "error.h"
#include "sending.h"

unsigned char *sending_packet(Options *options, char *message, int message_length) {
    int sock;
    int sent_len;
    struct sockaddr_in server;
    struct sockaddr_in from;
    socklen_t len;
    unsigned char *buffer = calloc(MAX_BUFFER_LENGTH, sizeof(char));
    if (buffer == NULL) {
        free(options);
        free(message);
        error_exit("Error while allocating memory!");
    }

    // get dns server address
    struct hostent *host;
    struct in_addr ipv4_address;
    if ((host = gethostbyname(options->server)) != NULL) {
        memcpy(&ipv4_address, host->h_addr_list[0], sizeof(struct in_addr));
        // set the ipv4 address, family and port
        server.sin_addr = ipv4_address;
        server.sin_family = AF_INET;
        server.sin_port = htons(options->port);   
    } else {
        free(options);
        free(message);
        free(buffer);
        error_exit("Failed to get the DNS server!");
    }

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        free(options);
        free(message);
        free(buffer);
        error_exit("Failed to create socket!");
    }

    // send the data to the server
    len = sizeof(server);
    sent_len = sendto(sock, message, message_length, 0, (struct sockaddr *) &server, len);
    // check if it was sent properly
    if (sent_len == -1) {
        free(options);
        free(message);
        free(buffer);
        close(sock);
        error_exit("Failed to send the DNS query!");
    } else if (sent_len != message_length) {
        free(options);
        free(message);
        free(buffer);
        close(sock);
        error_exit("Failed to send the whole DNS query!");
    }

    len = sizeof(from);
    // read the answer from the server 
    if (recvfrom(sock, buffer, MAX_BUFFER_LENGTH, 0, (struct sockaddr *) &from, &len) == -1) {
        free(options);
        free(message);
        free(buffer);
        close(sock);
        error_exit("Failed to send the whole DNS query!");
    }

    close(sock);
    return buffer;
}

