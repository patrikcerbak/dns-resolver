/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <stdint.h>
#include "arguments.h"

#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_ID 65535 // max id value

#define HEADER_RD_MASK 0x0100 // a mask for recursion desired bit

#define INET_6 10 // AF_INET6 is not availiable on eva

// the dns types:
#define TYPE_A 1
#define TYPE_NS 2
#define TYPE_CNAME 5
#define TYPE_SOA 6
#define TYPE_PTR 12
#define TYPE_MX 25
#define TYPE_AAAA 28

#define CLASS_IN 1 // the dns IN class

#define MAX_QNAME_LENGTH 255 // the max qname length

// helper values for the ipv6 parsing into arpa format:
#define IPV6_LOWER 0x0f
#define IPV6_UPPER 4

/**
 * A strusture for the header of DNS question.
*/
typedef struct Header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} Header;

/**
 * A structure for the type and class of the DNS question.
*/
typedef struct Question {
    uint16_t qtype;
    uint16_t qclass;
} Question;

/**
 * A structure for the whole DNS question message.
*/
typedef struct Message {
    Header *header;
    char *qname;
    Question *question;
} Message;

/**
 * A function for creating a DNS question message.
 * 
 * @param options the options structure with parsed arguments
 * 
 * @return a filled Message structure
*/
Message message_create_question(Options *options);

/**
 * A function for converting domain names to the qname format.
 * 
 * @param domain a domain name
 * 
 * @return a converted domain name
*/
char *message_convert_to_qname(char *domain);

/**
 * A function for converting an IP address to the corresponding DNS message format.
 * 
 * @param ip the IP address
 * 
 * @return a converted IP address
*/
char *message_convert_to_arpa(char *ip);

/**
 * A function for converting the message into an array of chars.
 * 
 * @param message the parsed DNS message
 * 
 * @return a array of chars with the message bytes
*/
char *message_to_char(Message message);

#endif

