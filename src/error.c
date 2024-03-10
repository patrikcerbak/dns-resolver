/*
 * Author: Patrik Cerbak
 * Login: xcerba00
*/

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void error_exit(char *error_text) {
    fprintf(stderr, "%s\n", error_text);
    exit(ERROR_CODE);
}

