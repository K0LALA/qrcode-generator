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

typedef enum EncodingEnum {
    NUMERIC = 0b0001,   // Decimal digits (0-9), uses 4 bits
    ALPHA   = 0b0010,   // Digits, uppercase letters and some symbols
    BYTE    = 0b0100,   // ISO-8859-1 character set
    KANJI   = 0b1000,   // Double-byte characters from the Shift JIS character set
    ECI     = 0b0111    // Directly specifies the character set used
} Encoding;

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
    Encoding encoding : 4;
    EcLevel ecLevel : 2;
} QrCode;


void displayCode(const QrCode *code);
int drawCode(const QrCode *code);

int fillQrCode(QrCode *code, const char *message);

#endif // _GEN_H

