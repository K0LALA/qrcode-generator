#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "generator.h"

enum EXIT_CODES {
    SUCCESS = EXIT_SUCCESS,
    TESTS_DID_NOT_PASS = 1,
    MISSING_DATA = 2
};

#endif // _MAIN_H

