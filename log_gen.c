#include <stdio.h>
#include <stdlib.h>

int main() {
    unsigned short logTable[256] = { 1 };
    unsigned char antiLogTable[256] = { 0 };

    printf("Log table:\n");
    printf("%d, ", logTable[0]);

    int i;
    for (i = 1; i < 256; i++) {
        logTable[i] = logTable[i - 1] * 2;
        if (logTable[i] > 255) logTable[i] ^= 0b100011101;
        antiLogTable[logTable[i]] = i;
        printf("%d, ", logTable[i]);
    }

    antiLogTable[1] = 0;

    printf("\nAntilog table:\n");
    for (i = 0; i < 256; i++) {
        printf("%d, ", antiLogTable[i]);
    }

    return 0;
}
