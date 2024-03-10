/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#define MAX_ARG_COUNT 9 // maximum number of arguments

#define MAX_SERVER_LENGTH 300
#define MAX_ADDRESS_LENGTH 300

// the help message
#define HELP_STRING "Usage: dns [-r] [-x] [-6] -s server [-p port] address\n \
  -r: recursion desired\n \
  -x: do reverse query\n \
  -6: look for AAAA resource, instead of A\n \
  -s server: the DNS server to send the query\n \
  -p port: port to send the query to (default 53)\n \
  address: the queried address\n"

/**
 * A structure to save the parsed arguments information in.
*/
typedef struct Options {
    char recursion;
    char reverse;
    char aaaa;
    char server[MAX_SERVER_LENGTH];
    int port;
    char address[MAX_ADDRESS_LENGTH];
} Options;

/**
 * A function for parsing the program arguments.
 * 
 * @param count the number of arguments
 * @param arguments the arguments itself
 * 
 * @return a filled Options structure
*/
Options *arguments_parse(int count, char **arguments);

#endif

