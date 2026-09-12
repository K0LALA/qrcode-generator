#include "main.h"

int main (int argc, char **argv) {
    
    if (argc <= 1) {
        printf("You at least have to enter some data to encode.\n");
        return MISSING_DATA;
    }

    const char *message = argv[1];

    QrCode code;

    fillQrCode(&code, message);
    free(code.grid);

    // TODO: Investigate, last run was not showing data on the code, only masking, format info and function patterns
    return SUCCESS;
}

