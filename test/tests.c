/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/arguments.h"
#include "../src/message.h"
#include "../src/sending.h"

void arguments_parse_test() {
    printf("arguments_parse_test: ");

    char *argv[] = {"dns", "-s", "1.1.1.1", "google.com", "-r", "-6", "-p", "40"};
    Options *options = arguments_parse(8, argv);
    if (strcmp("1.1.1.1", options->server) != 0 ||
        strcmp("google.com", options->address) != 0 ||
        options->recursion != 1 ||
        options->aaaa != 1 ||
        options->port != 40) {
        
        printf("FAILED");
    } else {
        printf("OK");
    }
    free(options);

    printf("\n");
}

void message_create_question_test() {
    printf("message_create_question_test: ");

    Options *options = malloc(sizeof(Options));
    if (options == NULL) {
        printf("FAILED\n");
        return;
    }

    options->aaaa = 0;
    strcpy(options->address, "google.com");
    options->port = 53;
    options->recursion = 0;
    options->reverse = 0;
    strcpy(options->server, "1.1.1.1");

    Message message = message_create_question(options);

    if (message.header->flags != 0 ||
        message.header->qdcount != htons(1) ||
        message.header->ancount != 0 ||
        message.header->arcount != 0 ||
        message.question->qclass != htons(CLASS_IN) ||
        message.question->qtype != htons(TYPE_A)) {

        printf("FAILED");
    } else {
        printf("OK");
    }

    free(message.header);
    free(message.qname);
    free(message.question);
    free(options);

    printf("\n");
}

void message_convert_to_qname_test() {
    printf("message_convert_to_qname_test: ");

    char *converted = message_convert_to_qname("www.vutbr.cz");
    char correct[] = {0x03, 0x77, 0x77, 0x77, 0x05, 0x76, 0x75, 0x74, 0x62, 0x72, 0x02, 0x63, 0x7a, 0x00};

    if (strcmp(correct, converted) != 0) {
        printf("FAILED");
    } else {
        printf("OK");
    }

    printf("\n");
}

void message_convert_to_arpa_test() {
    printf("message_convert_to_arpa_test: ");

    int correct = 1;

    // ipv4 test
    char *converted_4 = message_convert_to_arpa("192.168.0.1");
    char correct_4[] = "1.0.168.192.in-addr.arpa";
    if (strcmp(correct_4, converted_4) != 0) {
        correct = 0;
    }

    // ipv6 test
    char *converted_6 = message_convert_to_arpa("c4db:8080:1:1::667:6f6f");
    char correct_6[] = "f.6.f.6.7.6.6.0.0.0.0.0.0.0.0.0.1.0.0.0.1.0.0.0.0.8.0.8.b.d.4.c.ip6.arpa";
    if (strcmp(correct_6, converted_6) != 0) {
        correct = 0;
    }

    if (correct) {
        printf("OK");
    } else {
        printf("FAILED");
    }

    free(converted_4);
    free(converted_6);

    printf("\n");
}

void sending_packet_test() {
    printf("sending_packet_test: ");

    char *argv[] = {"dns", "-s", "1.1.1.1", "google.com", "-r", "-6"};
    Options *options = arguments_parse(6, argv); // parse arguments
    Message message = message_create_question(options); // create message to send
    char *buffer = message_to_char(message); // convert the message to chars
    int length = sizeof(Header) + strlen(message.qname) + 1 + sizeof(Question); // the length of the packet to send
    free(message.header);
    free(message.qname);
    free(message.question);

    unsigned char *answer = sending_packet(options, buffer, length); // send the message and receive the answer
    unsigned char correct[] = {0xc4, 0xdb, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01};

    for (int i = 2 /* don't compare id */; i < 28; i++) {
        if (correct[i] != answer[i]) {
            free(answer);
            printf("FAILED\n");
            return;
        }
    }

    free(answer);
    printf("OK\n");
}

int main() {
    arguments_parse_test();
    message_create_question_test();
    message_convert_to_qname_test();
    message_convert_to_arpa_test();
    sending_packet_test();

    return 0;
}