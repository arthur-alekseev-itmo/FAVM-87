#include <stdio.h>
#include "bytecode/bytecode.h"
#include "interpreter/interpreter.h"

int main(int argc, const char** argv) {
    printf("FAVM-87 INITIALIZED\n");

    if (argc < 2) {
        printf("ERR: Input file must be supplied");
        return 1;
    }

    const bytecode* bc = read_bytecode(argv[1]);

    if (bc == NULL) {
        printf("ERR: Bytecode was null");
        return 1;
    }

    run_bytecode(bc);

    return 0;
}