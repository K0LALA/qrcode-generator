#include "main.h"

#ifdef UNIT_TESTS

static bool grid1[121] = {0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1,
                        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                        0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
                        0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1,
                        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                        1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1,
                        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
                        1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0,
                        0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0,
                        1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0};
static const QrCode code1 = { grid1, 0, 11, 10, 10, BYTE, LOW };

static bool grid2[441] = {
1,1,1,1,1,1,1,0,1,1,0,0,0,0,1,1,1,1,1,1,1,
1,0,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,
1,0,1,1,1,0,1,0,1,0,0,1,1,0,1,0,1,1,1,0,1,
1,0,1,1,1,0,1,0,1,0,0,0,0,0,1,0,1,1,1,0,1,
1,0,1,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,0,1,
1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,
1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,0,1,0,1,1,0,0,0,0,1,0,1,0,1,1,1,1,1,
0,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,0,0,1,
0,0,1,1,0,1,1,1,0,1,1,0,0,0,1,0,1,1,0,0,0,
0,1,1,0,1,1,0,1,0,0,1,1,0,1,0,1,0,1,1,1,0,
1,0,0,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,
0,0,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,0,1,0,1,
1,1,1,1,1,1,1,0,1,0,1,0,0,0,0,1,0,1,1,0,0,
1,0,0,0,0,0,1,0,0,1,0,1,1,0,1,1,0,1,0,0,0,
1,0,1,1,1,0,1,0,1,0,1,0,0,0,1,1,1,1,1,1,1,
1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,
1,0,1,1,1,0,1,0,1,0,0,0,1,1,1,1,0,1,0,0,1,
1,0,0,0,0,0,1,0,1,0,1,1,0,1,0,0,0,1,0,1,1,
1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1
};
// HELLO WORLD in Alphanumeric encoding
static const QrCode code2 = { grid2, 0, 21, 20, 20, ALPHA, HIGH };

static const unsigned char codewords1[4] = { 0x3a, 0xf1, 0xd8, 0xa0 };

bool runUnitTests() {
    bool passed = true;

    assert(evaluateConsecutiveModules(&code1) == 30);
    assert(evaluateSquareModules(&code1) == 60);
    assert(evaluateFinderPatternsLookAlike(&code1) == 160);
    assert(evaluateNotBalanced(&code1) == 0);

    assert(evaluateConsecutiveModules(&code2) == 180);
    assert(evaluateSquareModules(&code2) == 90);
    assert(evaluateFinderPatternsLookAlike(&code2) == 80);
    assert(evaluateNotBalanced(&code2) == 0);

    assert(getMostEfficientEncoding("0123456789") == NUMERIC);
    assert(getMostEfficientEncoding("ALPHA-TEXT") == ALPHA);
    assert(getMostEfficientEncoding("OLZEASG7B9") == ALPHA);
    assert(getMostEfficientEncoding("Z $%*+-./:") == ALPHA);
    assert(getMostEfficientEncoding("byte-text!") == BYTE);
    assert(getMostEfficientEncoding("Hello, World!\n") == BYTE);

    // Note: Functions that call the function writeToCode prevent the program from being used afterwards
    // That is because of the static variable dataIndex which keeps its status after being used by the tests
        
    return passed;
}

#endif

int main (int argc, char **argv) {

#ifdef UNIT_TESTS
    if (!runUnitTests()) return TESTS_DID_NOT_PASS;
    printf("Unit tests all passed.\n");
#endif

    if (argc <= 1) {
        printf("You at least have to enter some data to encode.\n");
        return MISSING_DATA;
    }

    const char *message = argv[1];

    QrCode code;

    fillQrCode(&code, message);
    free(code.grid);

    return SUCCESS;
}

