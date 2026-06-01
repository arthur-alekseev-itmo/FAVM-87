#ifndef FAVM87_BYTECODE_H
#define FAVM87_BYTECODE_H

#include "stdint.h"

typedef struct {
    uint8_t* instructions;
    uint8_t** instruction_starts;
    uint32_t size;
} bytecode;


bytecode* read_bytecode(const char* filename);

void free_bytecode(bytecode* bc);

#endif //FAVM87_BYTECODE_H