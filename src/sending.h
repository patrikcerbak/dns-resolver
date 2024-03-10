/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include "arguments.h"

#ifndef SENDING_H
#define SENDING_H

#define MAX_BUFFER_LENGTH 512 // the maximum length of a message

/**
 * A function for sending the DNS query and receiving the answer.
 * 
 * @param options the parsed options structure
 * @param message the message to send
 * @param message_length the length of the message to send
 * 
 * @return the server response
*/
unsigned char *sending_packet(Options *options, char *message, int message_length);

#endif

