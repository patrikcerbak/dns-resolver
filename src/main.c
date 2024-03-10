/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "message.h"
#include "response.h"
#include "sending.h"

int main(int argc, char **argv) {
    Options *options = arguments_parse(argc, argv); // parse arguments

    Message message = message_create_question(options); // create message to send

    char *buffer = message_to_char(message); // convert the message to chars

    int length = sizeof(Header) + strlen(message.qname) + 1 + sizeof(Question); // the length of the packet to send
    
    free(message.header);
    free(message.qname);
    free(message.question);

    unsigned char *answer = sending_packet(options, buffer, length); // send the message and receive the answer

    free(options);
    free(buffer);

    response_parse_and_print(answer); // parse and print the answer

    free(answer);

    return 0;
}

