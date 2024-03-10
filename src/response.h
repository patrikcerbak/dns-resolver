/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <stdint.h>

#ifndef RESPONSE_H
#define RESPONSE_H

#define YES_LENGTH 4 // the length of "Yes" + '\0'

// masks for header:
#define AUTHORITATIVE_MASK 0x0400
#define RECURSIVE_MASK 0x0180
#define TRUNCATED_MASK 0x0200

#define SECTIONS_NUM 4 // the number of sections in dns packet

#define POINTER_MASK 0xc0 // a mask for a pointer in resource data

/**
 * A structure for the resource part of the DNS response.
*/
typedef struct Resource {
    uint16_t type;
    uint16_t class;
    // ttl is 32 bit, but needs to be done using two 16 bit integers, because the structure would otherwise have wrong size
    int16_t ttl_upper;
    int16_t ttl_lower;
    uint16_t rdlength;
} Resource;

/**
 * A function for parsing the DNS response and printing it.
 * 
 * @param response the DNS response
*/
void response_parse_and_print(unsigned char *response);

/**
 * A helper function for checking if the position is still in the DNS packet range (512 bytes).
 * 
 * @param position the current position in the response
 * @param increment the increment for the position
*/
void response_check_position(int position, int increment);

/**
 * A function for printing the type of the resource.
 * 
 * @param type the type value
*/
void response_print_type(uint16_t type);

/**
 * A function for printing the class of the resource.
 * 
 * @param class the class value
*/
void response_print_class(uint16_t class);

/**
 * A function for converting the name from response to readable form and printing it.
 * 
 * @param response the DNS response
 * @param position the current position in the response
*/
void response_convert_and_print_name(unsigned char *response, int position);

/**
 * A function for printing the rdata from the DNS response.
 * 
 * @param type the type of resource
 * @param response the DNS response
 * @param position the current position in the response
*/
void response_print_rdata(uint16_t type, unsigned char *response, int position);

#endif

