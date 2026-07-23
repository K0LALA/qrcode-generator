#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "qdbmp.h"

#define SIZE 21             // We are using version 1

bool isAfterVerticalTimingPattern(const int x)
{
    return x > 6;
}

int getYUpperBoundary(const int x)
{
    if (x > 8 && x < 13)
    {
        // Between top finder patterns
        return 0;
    }
    // Below finder patterns and format info
    return 9;
}

int getYLowerBoundary(const int x)
{
    if (x <= 8)
    {
        // Between left finder patterns and format info
        return 12;
    }
    return SIZE - 1;
}

bool isGoingUp(const int x)
{
    return (int)((x - isAfterVerticalTimingPattern(x)) / 2) % 2;
}

/// Computes the next (x,y) position for the data on the QR-Code
/// @param x A pointer to the current x position, changed in place
/// @param y A pointer to the current y position, changed in place
/// @return true if the QR-Code is finished, false otherwise
bool getNextDataPosition(int* px, int* py)
{
    int x = *px;
    int y = *py;
    // 3 basic moves available: left, top-right, bottom-right
    // Left: if x is odd before vertical timing patterns or if x is even after vertical timing patterns
    //       or if y is either to the bottom and going down or to the top and going up
    // Top-right: (if x is even before timing patterns or if x is odd after timing patterns) and going up
    // Bottom-right: (if x is even before timing patterns or if x is odd after timing patterns) and going down
    // 
    // Going up: if (int)(x / 2) is odd (need to account for timing patterns, substract 1 to x if after timing patterns)
    // Going down: if (int)(x / 2) is even (need to account for timing patterns, substract 1 to x if after timing patterns)
    //
    // Note 1: Timing patterns are located on x=6 and y=6, if going through them, we need to act as if they were not here, so either bump x or y depending on which one was crossed and which direction (horizontal one)
    // Note 2: After the 3 EC block, neither of the rules can manage to go to the correct 4th EC block, specific case is needed

    // Note 2
    if (x == 9 && y == getYLowerBoundary(x))
    {
        x--;
        y = getYLowerBoundary(x);
    }
    // Left
    else if ((x - isAfterVerticalTimingPattern(x)) % 2
        || (y == getYUpperBoundary(x) &&  isGoingUp(x))
        || (y == getYLowerBoundary(x) && !isGoingUp(x)))
    {
        x--;
    }
    // Top- or bottom-right
    else
    {
        y -= isGoingUp(x) * 2 - 1;
        x++;
    }

    // Note 1: Avoid timing patterns
    // Vertical
    if (x == 6) x--;
    // Horizontal
    if (y == 6)
    {
        if (*py == 7)
            y--;
        else
            y++;
    }

    *px = x;
    *py = y;

    return (x == 0 && y == getYLowerBoundary(x));
}

/// Writes the data starting from startX and startY
/// @param code A pointer to the first element of the code
/// @param startX A pointer to the starting x position for the data, will be changed to starting position for next data
/// @param startY A pointer to the starting y position for the data, will be changed to starting position for next data
/// @param data The data, MSB will be written first on the QR-Code
/// @param dataLength, The number of bits from data to write, starting from MSB
/// @return true if the end of the QR-Code is reached, false otherwise
bool writeDataToCode(bool* code, int *startX, int *startY, const unsigned char data, const unsigned char dataLength)
{
    unsigned char dataCopy = data;
    int i;
    for (i = 0; i < dataLength; i++)
    {
        bool bit = dataCopy & (1 << 7);
        if (bit)
        {
            code[*startY * SIZE + *startX] = 1;
        }
        if(getNextDataPosition(startX, startY))
        {
            printf("Couldn't write to QR-Code\n");
            return 1;
        }
        dataCopy = dataCopy << 1;
    }
    return 0;
}

void displayCode(const bool* code, const int size)
{
    printf("Qr-Code:\n");
    int y;
    for (y = 0; y < size; y++)
    {
        int x;
        for (x = 0; x < size; x++)
        {
            printf(code[y * size + x] ? "#" : " ");
        }
        printf("|\n");
    }
}

int drawCode(const bool* code, const int size)
{
    BMP* bmp = BMP_Create(size, size, 24);
    
    int y;
    for (y = 0; y < size; y++)
    {
        int x;
        for (x = 0; x < size; x++)
        {
            UCHAR color = code[y * size + x] ? 0 : 255;
            BMP_SetPixelRGB(bmp, x, y, color, color, color);
        }
    }
    BMP_WriteFile(bmp, "qr.bmp");
    BMP_CHECK_ERROR(stderr,-2);
    BMP_Free(bmp);
}

int main(int argc, char** argv)
{
    bool codeGrid[SIZE*SIZE] = { 0 };

    char* message;
    if (argc >= 2)
    {
        message = argv[1];
    }
    else
    {
        message = "www.wikipedia.org";
    }

    int x = SIZE - 1;
    int y = SIZE - 1;

    unsigned char encodingMode = 0b0100 << 4;        // Byte
    writeDataToCode(codeGrid, &x, &y, encodingMode, 4);
    unsigned char messageLength = strlen(message);
    writeDataToCode(codeGrid, &x, &y, messageLength, 8);

    char* messagePointer = message;
    while(*messagePointer)
    {
        writeDataToCode(codeGrid, &x, &y, *messagePointer, 8);
        messagePointer++;
    }

    // Terminator
    // TODO: Compute actual required size for the terminator
    writeDataToCode(codeGrid, &x,&y, 0, 4);

    // TODO: Add padding to the end of the string if necessary

    displayCode(codeGrid, SIZE);
    drawCode(codeGrid, SIZE);

    unsigned char errorCorrectionLevel = 0b11 << 6;  // Low

    return 0;
}

