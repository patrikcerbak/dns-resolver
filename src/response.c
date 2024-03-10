/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "message.h"
#include "sending.h"
#include "response.h"

void response_parse_and_print(unsigned char *response) {
    // create the Header structure and map it to the data from response
    Header *response_header = malloc(sizeof(Header));
    if (response_header == NULL) {
        free(response);
        error_exit("Error while allocating memory!");
    }

    memcpy(response_header, response, sizeof(Header));

    // check if the response is authoritative
    char authoritative[YES_LENGTH];
    if ((htons(response_header->flags) & AUTHORITATIVE_MASK) == AUTHORITATIVE_MASK) {
        strcpy(authoritative, "Yes");
    } else {
        strcpy(authoritative, "No");
    }

    // check if the response is recursive
    char recursive[YES_LENGTH];
    if ((htons(response_header->flags) & RECURSIVE_MASK) == RECURSIVE_MASK) {
        strcpy(recursive, "Yes");
    } else {
        strcpy(recursive, "No");
    }

    // check if the response is truncated
    char truncated[YES_LENGTH];
    if ((htons(response_header->flags) & TRUNCATED_MASK) == TRUNCATED_MASK) {
        strcpy(truncated, "Yes");
    } else {
        strcpy(truncated, "No");
    }

    printf("Authoritative: %s, Recursive: %s, Truncated: %s\n", authoritative, recursive, truncated);

    // the number of occurences of each section
    int question_num = htons(response_header->qdcount);
    int answer_num = htons(response_header->ancount);
    int authority_num = htons(response_header->nscount);
    int additional_num = htons(response_header->arcount);

    free(response_header);

    // helper arrays for each section
    int resources_count[] = {question_num, answer_num, authority_num, additional_num};
    char *resources_names[] = {"Question", "Answer", "Authority", "Additional"};

    int position = sizeof(Header); // move the current position in the response (no need for checking the position here)

    for (int i = 0; i < SECTIONS_NUM; i++) {
        printf("%s section (%d)\n", resources_names[i], resources_count[i]);

        // go through all resources in a section
        for (int j = 0; j < resources_count[i]; j++) {
            // calculate the new position in the response
            int temp_position = position;
            while (response[temp_position] != '\0') {
                if ((response[temp_position] & POINTER_MASK) == POINTER_MASK) {
                    temp_position++;
                    break;
                }
                temp_position++;
            }
            temp_position++;

            response_check_position(position, temp_position - position); // and check if the new position is inside the packet length

            // print the name
            printf("  ");
            response_convert_and_print_name(response, position);
            printf("., ");

            position = temp_position; // set the new position

            if (i == 0) {
                // if the section is question, allocate the memory for the Question struct
                Question *question = malloc(sizeof(Question));
                if (question == NULL) {
                    free(response);
                    error_exit("Error while allocating memory!");
                }

                response_check_position(position, sizeof(Question)); // check the new position

                // map the data into the structure and increase position
                memcpy(question, &response[position], sizeof(Question));
                position += sizeof(Question);

                // print type and class
                response_print_type(htons(question->qtype));
                printf(", ");
                response_print_class(htons(question->qclass));
                printf("\n");

                free(question);
            } else {
                // else allocate the memory for the Resource struct
                Resource *resource = malloc(sizeof(Resource));
                if (resource == NULL) {
                    free(response);
                    error_exit("Error while allocating memory!");
                }

                response_check_position(position, sizeof(Resource)); // check the new position

                // map data into the structure and increase position
                memcpy(resource, &response[position], sizeof(Resource));
                position += sizeof(Resource);

                // print type and class
                response_print_type(htons(resource->type));
                printf(", ");
                response_print_class(htons(resource->class));
                printf(", ");

                // put together the TTL and print it
                int ttl = 0;
                ttl |= htons(resource->ttl_upper) << 16;
                ttl |= htons(resource->ttl_lower);
                printf("%d, ", ttl);

                response_check_position(position, htons(resource->rdlength)); // check position

                // print the resource data
                response_print_rdata(htons(resource->type), response, position);

                position += htons(resource->rdlength);

                free(resource);
            }
        }
    }
}

void response_check_position(int position, int increment) {
    // if the position is outside of the packet, exit with error
    if (position + increment >= MAX_BUFFER_LENGTH) {
        error_exit("Error while parsing the response!");
    }
}

void response_print_type(uint16_t type) {
    // print the type
    if (type == TYPE_A) {
        printf("A");
    } else if (type == TYPE_NS) {
        printf("NS");
    } else if (type == TYPE_CNAME) {
        printf("CNAME");
    } else if (type == TYPE_SOA) {
        printf("SOA");
    } else if (type == TYPE_PTR) {
        printf("PTR");
    } else if (type == TYPE_AAAA) {
        printf("AAAA");
    } else if (type == TYPE_MX) {
        printf("MX");
    } else {
        // other type
        printf("%04x", type);
    }
}

void response_print_class(uint16_t class) {
    if (class == CLASS_IN) {
        printf("IN");
    } else {
        // other class
        printf("%04x", class);
    }
}

void response_convert_and_print_name(unsigned char *response, int position) {
    int count = response[position++]; // get the count of octets of a group

    // while the count is not 0...
    while (count != 0) {
        if ((count & POINTER_MASK) == POINTER_MASK) {
            // if the byte is pointer, calculate the new position
            int new_pos = 0;
            new_pos |= (response[position - 1] & ~POINTER_MASK) << 8; // the upper part without the "11" bits at the start
            new_pos |= response[position++]; // the lower part
            position = new_pos;

            count = response[position++];
            continue;
        }

        for (int i = 0; i < count; i++) {
            // print the data chars
            printf("%c", response[position++]);
        }

        count = response[position++]; // get new count

        if (count != 0) {
            printf(".");
        }
    }
}

void response_print_rdata(uint16_t type, unsigned char *response, int position) {
    // check what type the record is and print it accordingly
    if (type == TYPE_A) {
        // print ipv4 address
        printf("%d.%d.%d.%d\n", response[position], response[position + 1], response[position + 2], response[position + 3]);
    } else if (type == TYPE_AAAA) {
        // print ipv6 address
        char address[INET6_ADDRSTRLEN];
        if (inet_ntop(INET_6, response, address, INET6_ADDRSTRLEN) != NULL) {
            printf("%s\n", address);
        } else {
            free(response);
            error_exit("Error while parsing IPv6 address!");
        }
    } else {
        // convert the name to readable format and print
        response_convert_and_print_name(response, position);
        printf("\n");
    }
}

