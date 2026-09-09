#include "test.h"

int main (int argc, char **argv) {
    
    char *message;
    if (argc <= 1) {
        printf("You at least have to enter some data to encode.\n");
        return MISSING_DATA;
    }

    QrCode code;

    getCodeSizeFromMessage(message, &code);

    // TODO: Use QrCode struct for all functions in generator.c
    // TODO: Investigate, last run was not showing data on the code, only masking, format info and function patterns
    return SUCCESS;
}

