/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "arguments.h"

Options *arguments_parse(int count, char **arguments) {
    // check argument count
    if (count > MAX_ARG_COUNT) {
        error_exit("Too many arguments!");
    }

    // allocate memory for the Options structure
    Options *options = malloc(sizeof(Options));
    if (options == NULL) {
        error_exit("Error while allocating memory!");
    }

    // set default settings
    options->recursion = 0;
    options->reverse = 0;
    options->aaaa = 0;
    options->server[0] = '\0';
    options->port = 53;
    options->address[0] = '\0';

    // go through all arguments (except the first one - name of the file)
    for (int i = 1; i < count; i++) {
        if (strcmp(arguments[i], "-h") == 0 || strcmp(arguments[i], "--help") == 0) {
            printf(HELP_STRING);
            exit(0);
        } else if (strcmp(arguments[i], "-r") == 0) {
            options->recursion = 1;
        } else if (strcmp(arguments[i], "-x") == 0) {
            options->reverse = 1;
        } else if (strcmp(arguments[i], "-6") == 0) {
            options->aaaa = 1;
        } else if (strcmp(arguments[i], "-s") == 0) {
            if (arguments[++i] != NULL) {
                int length = 0;
                while (arguments[i][++length] != '\0'); // get the length of the server
                if (length < MAX_SERVER_LENGTH) {
                    strcpy(options->server, arguments[i]);
                } else {
                    free(options);
                    error_exit("The server length is too long!");
                }
            }
        } else if (strcmp(arguments[i], "-p") == 0) {
            if (arguments[++i] != NULL) {
                // convert the port from string to number
                char *endptr_port;
                int port = strtol(arguments[i], &endptr_port, 10);
                // check the port range
                if (port >= 0 && port <= 65535) {
                    options->port = port;
                } else {
                    free(options);
                    error_exit("The specified port is out of range!");
                }
            }
        } else {
            int length = 0;
            while (arguments[i][++length] != '\0'); // get the length of the address
            if (length < MAX_ADDRESS_LENGTH) {
                strcpy(options->address, arguments[i]);
            } else {
                free(options);
                error_exit("The address length is too long!");
            }
        }
    }

    // check for mandatory arguments
    if (options->server[0] == '\0') {
        free(options);
        error_exit("You must specify a DNS server!");
    } else if (options->address[0] == '\0') {
        free(options);
        error_exit("You must specify an address!");
    }

    // -x and -6 arguments
    if (options->reverse && options->aaaa) {
        free(options);
        error_exit("Cannot combine -x and -6 arguments!");
    }

    return options;
}

