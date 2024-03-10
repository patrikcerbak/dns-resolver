/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "error.h"
#include "message.h"

Message message_create_question(Options *options) {
    // allocate memory for the header of dns query
    Header *header = malloc(sizeof(Header));
    if (header == NULL) {
        free(options);
        error_exit("Error while allocating memory!");
    }

    // generate random id for the query
    int random_seed = 0;
    while (options->address[++random_seed] != '\0');
    srand(random_seed);
    header->id = htons(rand() % MAX_ID);

    // set the flag for recursion, if desired
    uint16_t flags = 0;
    if (options->recursion) {
        flags |= HEADER_RD_MASK;
    }

    // set other header values
    header->flags = htons(flags);
    header->qdcount = htons(1);
    header->ancount = 0;
    header->nscount = 0;
    header->arcount = 0;

    // generate qname from the address
    char *qname = message_convert_to_qname(options->address);

    // a structure for the message body
    Question *question = malloc(sizeof(Question));
    if (question == NULL) {
        free(options);
        free(header);
        error_exit("Error while allocating memory!");
    }

    // set the question type
    if (options->aaaa) {
        question->qtype = htons(TYPE_AAAA);
    } else if (options->reverse) {
        question->qtype = htons(TYPE_PTR);
        // if PTR, overwrite the qname with the ip address in the arpa format
        free(qname);
        char *arpa_address = message_convert_to_arpa(options->address);
        qname = message_convert_to_qname(arpa_address);
        free(arpa_address);
    } else {
        question->qtype = htons(TYPE_A);
    }
    
    // set question class to default (IN)
    question->qclass = htons(CLASS_IN);

    // construct the message and return
    Message message;
    message.header = header;
    message.qname = qname;
    message.question = question;

    return message;
}

char *message_convert_to_qname(char *name) {
    int len = strlen(name);

    char *qname = malloc(len + 1 + 1); // + 1 + 1 is for the first length octet and \0
    if (qname == NULL) {
        error_exit("Error while allocating memory!");
    }

    int i = 0;
    int location_of_size_label = 0;
    int count = 0;
    while (name[i] != '\0') {
        if (i >= MAX_QNAME_LENGTH - 1) {
            free(qname);
            error_exit("The converted qname is too long!");
        }

        if (name[i] == '.') {
            // if the current character is dot, add count to the last count position and start counting bytes again
            qname[location_of_size_label] = count;
            location_of_size_label = i + 1;
            count = 0;
        } else {
            // else add the character from name to the converted qname
            qname[i + 1] = name[i]; 
            count++;
        }
        
        i++;
    }

    qname[location_of_size_label] = count; // add the last count to the qname
    qname[i + 1] = '\0';
    
    return qname;
}

char *message_convert_to_arpa(char *ip) {
    char *arpa_address = malloc(MAX_QNAME_LENGTH);
    if (arpa_address == NULL) {
        error_exit("Error while allocating memory!");
    }

    struct in_addr ipv4_address;
    struct in6_addr ipv6_address;
    if (inet_pton(AF_INET, ip, &ipv4_address) == 1) {
        // if it is ipv4 address, convert the char to bytes
        unsigned char *bytes = (unsigned char *) &ipv4_address.s_addr;
        // and save the address in the right format ("192.168.0.1.in-addr.arpa")
        sprintf(arpa_address, "%u.%u.%u.%u.in-addr.arpa", bytes[3], bytes[2], bytes[1], bytes[0]);
    } else if (inet_pton(INET_6, ip, &ipv6_address) == 1) {
        // if it is ipv6 address, convert the char to bytes
        unsigned char *bytes = (unsigned char *) &ipv6_address;
        // and save the address in the right format...
        sprintf(arpa_address, "%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.ip6.arpa",
            bytes[15] & IPV6_LOWER, bytes[15] >> IPV6_UPPER,
            bytes[14] & IPV6_LOWER, bytes[14] >> IPV6_UPPER,
            bytes[13] & IPV6_LOWER, bytes[13] >> IPV6_UPPER,
            bytes[12] & IPV6_LOWER, bytes[12] >> IPV6_UPPER,
            bytes[11] & IPV6_LOWER, bytes[11] >> IPV6_UPPER,
            bytes[10] & IPV6_LOWER, bytes[10] >> IPV6_UPPER,
            bytes[9] & IPV6_LOWER, bytes[9] >> IPV6_UPPER,
            bytes[8] & IPV6_LOWER, bytes[8] >> IPV6_UPPER,
            bytes[7] & IPV6_LOWER, bytes[7] >> IPV6_UPPER,
            bytes[6] & IPV6_LOWER, bytes[6] >> IPV6_UPPER,
            bytes[5] & IPV6_LOWER, bytes[5] >> IPV6_UPPER,
            bytes[4] & IPV6_LOWER, bytes[4] >> IPV6_UPPER,
            bytes[3] & IPV6_LOWER, bytes[3] >> IPV6_UPPER,
            bytes[2] & IPV6_LOWER, bytes[2] >> IPV6_UPPER,
            bytes[1] & IPV6_LOWER, bytes[1] >> IPV6_UPPER,
            bytes[0] & IPV6_LOWER, bytes[0] >> IPV6_UPPER
        );
    } else {
        free(arpa_address);
        error_exit("Invalid IP address!");
    }

    return arpa_address;
}

char *message_to_char(Message message) {
    char *buffer = malloc(sizeof(Header) + strlen(message.qname) + 1 + sizeof(Question)); // size of the whole dns question packet
    if (buffer == NULL) {
        error_exit("Error while allocating memory!");
    }

    // copy the header into the buffer
    memcpy(buffer, message.header, sizeof(Header));
    // copy the qname into the buffer
    memcpy(buffer + sizeof(Header), message.qname, strlen(message.qname) + 1);
    // copy the question into the buffer
    memcpy(buffer + sizeof(Header) + strlen(message.qname) + 1, message.question, sizeof(Question));

    return buffer;
}

