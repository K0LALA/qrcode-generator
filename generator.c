#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "qdbmp.h"

// A2I
const unsigned char LOG_TABLE[256] = { 1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38, 76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192, 157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35, 70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161, 95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240, 253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226, 217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206, 129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204, 133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84, 168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115, 230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255, 227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65, 130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166, 81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9, 18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22, 44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1 };

// I2A
// Index 0 shouldn't be used
const unsigned char ANTILOG_TABLE[256] = { 0, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75, 4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113, 5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69, 29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166, 6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136, 54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64, 30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61, 202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87, 7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24, 227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46, 55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97, 242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162, 31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246, 108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90, 203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215, 79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175 };

#define FORMAT_GEN_POLY 0b10100110111
#define FORMAT_MASK 0b101010000010010

#define FINDER_PATTERN_LOOKALIKE 0b10111010000
#define FINDER_PATTERN_LOOKALIKE_SIZE 11
const unsigned char finderPatternSkipCount[FINDER_PATTERN_LOOKALIKE_SIZE] = {1, 2, 6, 4, 3, 5, 11, 4, 4, 9, 10};
const unsigned char finderPatternSkipCountReversed[FINDER_PATTERN_LOOKALIKE_SIZE] = {4, 3, 2, 1, 11, 11, 6, 7, 8, 11, 10};

// Using version 1-L
#define SIZE 21
#define DATA_COUNT 19
#define EC_COUNT 7

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

/// Writes the content starting from startX and startY
/// @param code A pointer to the first element of the code
/// @param startX A pointer to the starting x position for the data, will be changed to starting position for next content
/// @param startY A pointer to the starting y position for the data, will be changed to starting position for next content
/// @param content The content, MSB will be written first on the QR-Code
/// @param contentLength The number of bits from content to write, starting from MSB
/// @param dataRecord A pointer to an array of data, used for EC computations, NULL if it not data (e.g. EC modules)
/// @return true if the end of the QR-Code is reached, false otherwise
bool writeToCode(bool* code, int *startX, int *startY, const unsigned char content, const unsigned char contentLength, unsigned char *dataRecord)
{
    static unsigned char dataIndex = 0;

    unsigned char contentCopy = content;
    
    bool isFinished = false;
    
    int i;
    for (i = 0; i < contentLength; i++)
    {
        bool bit = contentCopy & (1 << 7);
        
        if (dataRecord != NULL) {
            dataRecord[dataIndex / 8] |= (bit << (7 - dataIndex % 8));    
            
            dataIndex++;
        }

        if (bit)
        {
            code[*startY * SIZE + *startX] = 1;
        }
        if (isFinished && i < contentLength - 1) {
            printf("Couldn't write to QR-Code, it is full\n");
            break;
        }
        isFinished = getNextDataPosition(startX, startY);
        contentCopy = contentCopy << 1;
    }
    return isFinished;
}

void displayCode(const bool* code, const int size) {
    printf("QR-Code: \n");
    int y;
    for (y = -4; y < size + 4; y++) {
        int x;
        for (x = -4; x < size + 4; x++) {
            bool black = (y >= 0 && y < size && x >= 0 && x < size && code[y * size + x]);
            if (black) printf("\033[38;5;16m");
            printf("██");
            if (black) printf("\033[0m");
        }
        printf("\n");
    }
}

int drawCode(const bool* code, const int size)
{
    BMP* bmp = BMP_Create(size + 8, size + 8, 8);
    
    BMP_SetPaletteColor(bmp, 0, 255, 255, 255);
    BMP_SetPaletteColor(bmp, 1, 0, 0, 0);

    int y;
    for (y = 0; y < size; y++)
    {
        int x;
        for (x = 0; x < size; x++)
        {
            BMP_SetPixelIndex(bmp, x + 4, y + 4, code[y * size + x]);
        }
    }
    BMP_WriteFile(bmp, "qr.bmp");
    BMP_CHECK_ERROR(stderr,-2);
    BMP_Free(bmp);
    return 0;
}

/// Converts the whole array from alpha notation to integer notation using the log table
void alpha2int(unsigned char *array, const size_t size) {
    int i;
    for (i = 0; i < size; i++) {
        array[i] = LOG_TABLE[array[i]];
    }
}

/// Converts the whole array from integer notation to alpha notation using the antilog table
void int2alpha(unsigned char *array, const size_t size) {
    int i;
    for (i = 0; i < size; i++) {
        array[i] = ANTILOG_TABLE[array[i]];
    }
}

/// Computes the generator polynomial for the given number of terms
/// We always start with the following polynomial: (a^0x^1 + a^0x^0) where a is alpha (c.f. alpha notation)
/// For each step we multiply the current polynomial with the following polynomial: (a^0x^1 + a^ix^0) where i is the step going from 1 to count - 1 (inclusive)
/// @param generatorPolynomial The array in which the generator polynomial will be stored, the value at index i indicates the coefficient for x^i using alpha notation
// @param count The number of EC codewords, the generator polynomial will have one more term than that value
void computeGeneratorPolynomial(unsigned char *generatorPolynomial, const unsigned char count) {
    // Alpha notation
    generatorPolynomial[0] = 0;
    generatorPolynomial[1] = 0;

    int i;
    for (i = 1; i < count; i++) {
        // It is needed to go from top to bottom so as to not make a copy of the array for the multiplication
        int j;
        for (j = i + 1; j >= 0; j--) {
            // Alpha notation               // Coefficients:
            unsigned short zeroTerm = 0;    // a^i
            unsigned char  oneTerm  = 0;    // a^0

            if (j != i + 1) {
                zeroTerm = generatorPolynomial[j] + i;
                if (zeroTerm > 255) zeroTerm = (zeroTerm % 256) + (zeroTerm / 256);
                zeroTerm = LOG_TABLE[zeroTerm];
            }
            if (j != 0) {
                oneTerm  = generatorPolynomial[j - 1];
                oneTerm  = LOG_TABLE[oneTerm ];
            }
            // Integer notation

            generatorPolynomial[j] = ANTILOG_TABLE[zeroTerm ^ oneTerm];
            // Alpha notation
        }
    }
}

void debugArray(unsigned char* array, unsigned char size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

/// Computes the EC Codewords for the given message by performing a polynomial long division between a polynomial generated based on the message and a generator polynomial, the remainder of this division contains all the EC codewords
/// @param result A pointer to an array of size ECCount which will have its content replaced by the EC codewords
/// @param ECCount The number of codewords for the EC
/// @param messageCodewords The codewords for the message, the first part of the message is in the start of the array
/// @param messageCodewordsCount The number of codewords for the message
void getECCodewords(unsigned char *result, const unsigned char ECCount, const unsigned char *messageCodewords, const unsigned char messageCodewordsCount) {
    unsigned char *dividend = (unsigned char *)malloc(messageCodewordsCount * sizeof(char));
    if (dividend == NULL) return;
    memcpy(dividend, messageCodewords, messageCodewordsCount);

    // Allocate for both generator and generatorMultiplied
    unsigned char *generator = (unsigned char*)malloc(2 * (ECCount + 1));
    if (generator == NULL) {
        free(dividend);
        return;
    }
    computeGeneratorPolynomial(generator, ECCount);
    unsigned char *generatorMultiplied = generator + ECCount + 1;

    unsigned char termsCount = messageCodewordsCount;

    int i, j;
    for (i = 0; i < messageCodewordsCount; i++) {
        int2alpha(dividend, termsCount);

        // Multiply the generator by the leading term of the remaining dividend
        // Alpha notation simplifies multiplication
        for (j = 0; j < ECCount + 1; j++) {
            unsigned short a = generator[j] + dividend[0];
            if (a > 255) a %= 255;
            generatorMultiplied[j] = a;
        }

        alpha2int(dividend, termsCount);
        alpha2int(generatorMultiplied, ECCount + 1);

        // XOR the dividend with the resulting generator
        // Integer notation needs to be used for the XOR to work
        for (j = 0; j < termsCount; j++) {
            dividend[j] = j + 1 < termsCount ? dividend[j + 1] : 0;
            if (j < ECCount + 1) dividend[j] ^= generatorMultiplied[ECCount - j - 1];
        }

        termsCount = termsCount > ECCount ? termsCount - 1 : termsCount;
    }

    memcpy(result, dividend, ECCount);

    free(dividend);
    free(generator);
}

/// Adds a finder pattern to the code at given coordinates
/// @param code A pointer to the code's grid being a 1D array
/// @param x The top-left corner's x position to place the finder pattern on the grid
/// @param y The top-left corner's y position to place the finder pattern on the grid
void writeFinderPatternToCode(bool* code, const unsigned char x, const unsigned char y) {
    // Draw a 7x7 square but avoid the second inner shell
    unsigned char sy;
    for (sy = 0; sy < 7; sy++) {
        unsigned char sx;
        for (sx = 0; sx < 7; sx++) {
            code[(sy + y) * SIZE + (sx + x)] = !(((sx == 1 || sx == 5) && (sy > 0 && sy < 6)) || ((sy == 1 || sy == 5) && (sx > 0 && sx < 6)));
        }
    }
}

void addFunctionPatterns(bool* code) {
    // Timing patterns
    int i;
    for (i = 8; i < SIZE - 8; i++) {
        code[i * SIZE + 6] = ~i & 1;
        code[6 * SIZE + i] = ~i & 1;
    }

    // Finder patterns
    writeFinderPatternToCode(code, 0, 0);
    writeFinderPatternToCode(code, SIZE - 7, 0);
    writeFinderPatternToCode(code, 0, SIZE - 7);

    // Separators
    for (i = 0; i < 8; i++) {
        // Top-Left
        code[7 * SIZE + i] = 0;
        code[i * SIZE + 7] = 0;
        // Top-Right
        code[7 * SIZE + SIZE - 8 + i] = 0;
        code[i * SIZE + SIZE - 8]     = 0;
        // Bottom-Left
        code[(SIZE - 8) * SIZE + i]     = 0;
        code[(SIZE - 8 + i) * SIZE + 7] = 0;
    }

    // Dark module
    code[(SIZE - 8) * SIZE + 8] = 1;
}

/// Computes the whole 15-bits format information with error correction
/// @param ECLevel The error correction level using the 2 LSB
/// @param mask The mask pattern used, using the 3 LSB
/// @return The format information using 15-bits out of 16, the MSB is not used
unsigned short computeFormatInfoEC(const unsigned char ECLevel, const unsigned char mask) {
    unsigned short formatInfo = 0;

    formatInfo |= ECLevel << 13;
    formatInfo |= mask << 10;

    unsigned short formatEC = formatInfo;
    unsigned short generatorPolynomial;
    unsigned char length = 15;
    
    for (;;) {
        while (~formatEC & (1 << (length - 1))) length--;

        if (length <= 10) break;

        generatorPolynomial = FORMAT_GEN_POLY << (length - 11);

        formatEC ^= generatorPolynomial;
    }

    formatInfo |= formatEC;
    formatInfo ^= FORMAT_MASK;

    return formatInfo;
}

/// Adds complete format information to the code
/// @param code The QR-Code
/// @param ECLevel The error correction level for the data, in the 2 LSB
/// @param mask The mask used for the data, in the 3 LSB
/// @return The 15 bits of format information, the MSB is left to 0 and not used
unsigned short addFormatInfo(bool *code, const unsigned char ECLevel, const unsigned char mask) {
    unsigned short formatInfo = computeFormatInfoEC(ECLevel, mask);

    bool value;
    int i;
    for (i = 0; i < 15; i++) {
        value = formatInfo & (1 << (14 - i));
        if (i < 7) {
            code[8 * SIZE + i + (i > 5)]        = value;
            code[(SIZE - i - 1) * SIZE + 8]     = value;
        }
        else {
            code[(15 - i - (i > 8)) * SIZE + 8] = value;
            code[8 * SIZE + SIZE - 8 + i - 7]   = value;
        }
    }

    return formatInfo;
}

bool mask0(const unsigned char x, const unsigned char y) { return (y + x) % 2 == 0; }
bool mask1(const unsigned char x, const unsigned char y) { return y % 2 == 0; }
bool mask2(const unsigned char x, const unsigned char y) { return x % 3 == 0; }
bool mask3(const unsigned char x, const unsigned char y) { return (y + x) % 3 == 0; }
bool mask4(const unsigned char x, const unsigned char y) { return (y/2 + x/3) % 2 == 0; }
bool mask5(const unsigned char x, const unsigned char y) { return (y * x) % 2 + (y * x) % 3 == 0; }
bool mask6(const unsigned char x, const unsigned char y) { return ((y * x) % 2 + (y * x) % 3) % 2 == 0; }
bool mask7(const unsigned char x, const unsigned char y) { return ((y + x) % 2 + (y * x) % 3) % 2 == 0; }

/// Returns the function for the adequate pattern depending on the position
/// @param mask The index of the mask to use
/// @return A pointer to the mask's function, it has 2 parameters for the coordinates and outputs a bool, 1 if the value at said coordinates needs to be changed, 0 otherwise
bool (*getMaskPattern(unsigned char mask))(const unsigned char, const unsigned char) {
    switch (mask) {
    case 0:
        return mask0;
    case 1:
        return mask1;
    case 2:
        return mask2;
    case 3:
        return mask3;
    case 4:
        return mask4;
    case 5:
        return mask5;
    case 6:
        return mask6;
    case 7:
        return mask7;
    default:
        return NULL;
    }
}

/// Applies the specified mask to the code's data
/// @param code The QR-Code
/// @param mask The index of the mask to use
void applyMask(bool *code, unsigned char mask) {
    bool (*maskPattern)(const unsigned char, const unsigned char) = getMaskPattern(mask);

    int y;
    for (y = 0; y < SIZE; y++) {
        int x;
        for (x = 0; x < SIZE; x++) {
            code[y * SIZE + x] ^= (*maskPattern)(x, y);
        }
    }
}

/// Evaluates the code according to the first rule
/// Looping through each row and column, add a penalty for each group of give or more modules of the same color
/// @param code The QR-Code, not changed
/// @return The penalty for this rule
unsigned int evaluateConsecutiveModules(const bool *code) {
    unsigned int penalty = 0;

    unsigned char sameModuleCount;
    bool lastModule;

    int x,y;
    for (y = 0; y < SIZE; y++) {
        sameModuleCount = 1;
        lastModule = code[y * SIZE];
        for (x = 1; x < SIZE; x++) {
            if (code[y * SIZE + x] == lastModule) sameModuleCount++;
            else {
                lastModule = !lastModule;
                if (sameModuleCount >= 5) penalty += sameModuleCount - 2;
                sameModuleCount = 1;
            }
        }
    }

    for (x = 0; x < SIZE; x++) {
        sameModuleCount = 1;
        lastModule = code[x];
        for (y = 1; y < SIZE; y++) {
            if (code[y * SIZE + x] == lastModule) sameModuleCount++;
            else {
                lastModule = !lastModule;
                if (sameModuleCount >= 5) penalty += sameModuleCount - 2;
                sameModuleCount = 1;
            }
        }
    }

    return penalty;
}

/// Evaluates the code according to the second rule
/// Add a penalty for any 2x2 square of the same color
/// @param code The QR-Code, not changed
/// @return The penalty for this rule
unsigned int evaluateSquareModules(const bool* code) {
    unsigned int penalty = 0;
    
    int x,y;
    for (y = 1; y < SIZE; y++) {
        for (x = 1; x < SIZE; x++) {
            bool v = code[y * SIZE + x];
            if (v == code[(y - 1) * SIZE + x] && v == code[y * SIZE + x - 1] && v == code[(y - 1) * SIZE + x - 1]) penalty += 3;
        }
    }

    return penalty;
}

/// Checks if either finder pattern look-alike is located at the pointed coordinates
/// @param code The QR-Code, not changed
/// @param x The x-coordinate of the starting position for the look-alike
/// @param y The y-coordinate of the starting position for the look-alike
/// @param isHorizontal Whether to check for position horizontally (true) or vertically (false)
/// @return The amount to shift x or y in order to find the next possible position, 0 if look-alike was found
unsigned char checkFinderLookAlike(const bool *code, unsigned char x, unsigned char y, bool isHorizontal) {
    unsigned char notReversed = FINDER_PATTERN_LOOKALIKE_SIZE;
    unsigned char reversed = FINDER_PATTERN_LOOKALIKE_SIZE;
    
    int i;
    for (i = 0; i < FINDER_PATTERN_LOOKALIKE_SIZE; i++) {
        const bool value = code[y * SIZE + x];
        if (notReversed == FINDER_PATTERN_LOOKALIKE_SIZE && value != (FINDER_PATTERN_LOOKALIKE & (1 << (FINDER_PATTERN_LOOKALIKE_SIZE - 1 - i)))) notReversed = i;
       if (reversed == FINDER_PATTERN_LOOKALIKE_SIZE && value != (FINDER_PATTERN_LOOKALIKE &  (1 << i))) reversed = i;

       if (isHorizontal) x++;
       else              y++;
    }

    if (reversed == FINDER_PATTERN_LOOKALIKE_SIZE || notReversed == FINDER_PATTERN_LOOKALIKE_SIZE) return 0;
    unsigned char skipCount = finderPatternSkipCount[notReversed];
    unsigned char skipCountReversed = finderPatternSkipCountReversed[reversed];
    return skipCount < skipCountReversed ? skipCount : skipCountReversed;
}

/// Evaluates the code according to the third rule
/// Add a penalty if there are patterns that look similar to the finder patterns
/// @param code The QR-Code, not changed
/// @return The penalty for this rule
unsigned int evaluateFinderPatternsLookAlike(const bool* code) {
    unsigned int penalty = 0;

    int x, y;
    for (y = 0; y < SIZE; y++) {
        x = SIZE - FINDER_PATTERN_LOOKALIKE_SIZE;
        while (x >= 0) {
            unsigned char skipCount = checkFinderLookAlike(code, x, y, true);
            if (skipCount == 0) {
                penalty += 40; 
                x -= 11;
                continue;
            }

            x -= skipCount;
        }
    }

    for (x = 0; x < SIZE; x++) {
        y = SIZE - FINDER_PATTERN_LOOKALIKE_SIZE;
        while (y >= 0) {
            unsigned char skipCount = checkFinderLookAlike(code, x, y, false);
            if (skipCount == 0) {
                penalty += 40;
                y -= 11;
                continue;
            }

            y -= skipCount;
        }
    }

    return penalty;
}

/// Evaluates the code according to the fourth rule
/// If there are more or less black modules than white
/// @param code The QR-Code, not changed
/// @return The penalty for this rule
unsigned int evaluateNotBalanced(const bool *code) {
    unsigned int penalty = 0;

    unsigned int moduleCount = SIZE * SIZE;
    unsigned int darkModuleCount = 0;

    int x, y;
    for (y = 0; y < SIZE; y++) {
        for (x = 0; x < SIZE; x++) {
            darkModuleCount += code[y * SIZE + x];
        }
    }

    unsigned int lowProportion = darkModuleCount * 20 / moduleCount;
    unsigned int highProportion = lowProportion + 1;
    lowProportion = abs(lowProportion - 10);
    highProportion = abs(highProportion - 10);

    penalty += 10 * (lowProportion < highProportion ? lowProportion : highProportion);

    return penalty;
}

/// Tests all masks to choose the best
/// @param code The QR-Code's grid, will be modified with the best suiting mask, function patterns are not included
/// @param ECLevel The error correction level used
/// @return The best mask used
unsigned char useBestMask(bool *code, const unsigned char ECLevel) {
    // For each mask, apply the mask, add version information and function patterns
    // Evaluate the mask, compare with minimum
    unsigned int lowestPenalty = -1;
    unsigned char lowestMask = 0;

    const size_t codeSize = sizeof(bool) * SIZE * SIZE;
    bool *copy = (bool*)malloc(codeSize);

    displayCode(code, SIZE);

    int mask;
    for (mask = 0; mask < 8; mask++) {
        memcpy(copy, code, codeSize);

        applyMask(copy, mask);
        addFunctionPatterns(copy);
        unsigned short formatString = addFormatInfo(copy, ECLevel, mask);

        int i;
        for (i = 14; i >= 0; i--) {
            printf("%d", (bool) (formatString & (1 << i)));
        }
        printf("\n");

        displayCode(copy, SIZE);

        unsigned int penalty = evaluateConsecutiveModules(copy)
                             + evaluateSquareModules(copy)
                             + evaluateFinderPatternsLookAlike(copy)
                             + evaluateNotBalanced(copy);
        
        printf("%u\n", penalty);

        if (penalty < lowestPenalty) {
            lowestPenalty = penalty;
            lowestMask = mask;
        }
    }

    // In the end, get the most efficient mask and apply it to the code
    applyMask(code, lowestMask);

    return lowestMask;
}

int main(int argc, char** argv)
{
    bool codeGrid[SIZE*SIZE] = { 0 };

    // The beginning of the message has the highest exponent
    unsigned char messageCodewords[DATA_COUNT] = { 0 };

    char* message;
    if (argc >= 2)
    {
        message = argv[1];
    }
    else
    {
        message = "github.com/K0LALA";
    }

    int x = SIZE - 1;
    int y = SIZE - 1;

    unsigned char encodingMode = 0b0100 << 4;        // Byte
    writeToCode(codeGrid, &x, &y, encodingMode, 4, messageCodewords);
    unsigned char messageLength = strlen(message);
    writeToCode(codeGrid, &x, &y, messageLength, 8, messageCodewords);

    char* messagePointer = message;
    while(*messagePointer)
    {
        writeToCode(codeGrid, &x, &y, *messagePointer, 8, messageCodewords);
        messagePointer++;
    }

    // Terminator
    // TODO: Compute actual required size for the terminator
    writeToCode(codeGrid, &x,&y, 0, 4, messageCodewords);

    // Padding
    unsigned char paddingCount = 19 - 2 - messageLength;

    int i;
    for (i = 0; i < paddingCount; i++) {
        writeToCode(codeGrid, &x, &y, i & 1 ? 0b00010001 : 0b11101100, 8, messageCodewords);
    }

    unsigned char errorCorrectionLevel = 0b01;  // Low
    
    unsigned char generator[EC_COUNT + 1] = { 0 };
    computeGeneratorPolynomial(generator, EC_COUNT);

    unsigned char ECCodewords[EC_COUNT] = { 0 };
    getECCodewords(ECCodewords, EC_COUNT, messageCodewords, DATA_COUNT);

    for ( i = 0; i < EC_COUNT; i++) {
        writeToCode(codeGrid, &x, &y, ECCodewords[i], 8, NULL);
    }

    printf("\n");

    // Masking
    unsigned char mask = useBestMask(codeGrid, errorCorrectionLevel);
    
    // Function Patterns
    addFunctionPatterns(codeGrid);

    // Format Information
    addFormatInfo(codeGrid, errorCorrectionLevel, mask);

    displayCode(codeGrid, SIZE);
    drawCode(codeGrid, SIZE);

    return 0;
}

