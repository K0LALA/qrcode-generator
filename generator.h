#ifndef _GEN_H
#define _GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "qdbmp.h"

#define FORMAT_GEN_POLY 0b10100110111
#define FORMAT_MASK 0b101010000010010

#define FINDER_PATTERN_LOOKALIKE 0b10111010000
#define FINDER_PATTERN_LOOKALIKE_SIZE 11

// TODO: Change with some computed values
// Version 1-L
#define SIZE 21
#define DATA_COUNT 19
#define EC_COUNT 7

typedef enum ECLevelEnum {
    LOW = 0b01,
    MEDIUM = 0b00,
    QUARTILE = 0b11,
    HIGH = 0b10 
} EcLevel;

typedef struct QrCodeStruct {
    bool *grid;
    unsigned char size;
    unsigned char lastX;
    unsigned char lastY;
    EcLevel ecLevel;
} QrCode;


void displayCode(const QrCode *code);
int drawCode(const QrCode *code);

void getCodeSizeFromMessage(char *message, QrCode *code);

#endif // _GEN_H

