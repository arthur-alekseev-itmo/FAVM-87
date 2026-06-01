#import "bytecode.h"

#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

long get_file_size(FILE* file) {
    if (file == NULL) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }

    return ftell(file);
}

int calculate_instruction_size(const uint8_t* instruction_ptr) {
    switch ((Opcode)(*instruction_ptr)) {
        case LoadInt: return 9;
        case LoadString: {
            const uint64_t size = *(uint64_t*)(instruction_ptr + 1);
            return (int)size + 9;
        }
        case LoadLocal: return 9;
        case StoreLocal: return 9;
        case LoadGlobal: return 9;
        case StoreGlobal: return 9;
        case BranchTrue: return 9;
        case BranchFalse: return 9;
        case Branch: return 9;
        case CmpEq: return 1;
        case FunctionAddress: return 10;
        case Apply: return 1;
        case ConstructTuple: return 9;
        case DeconstructTuple: return 9;
        case Drop: return 1;
        case GetVariantContent: return 1;
        case GetVariantTag: return 1;
        case ConstructVariant: return 9;
        case Sigbus: return 1;
        case Exit: return 1;
        case Return: return 1;
        case Dup: return 1;
        default:
            printf("Bad instruction opcode: %d", (*instruction_ptr));
            exit(1);
    }
}

void calculate_instruction_starts(const bytecode* bc) {
    long ip = 0;
    long index = 0;
    while (ip < bc->size) {
        bc->instruction_starts[index++] = &bc->instructions[ip];
        ip += calculate_instruction_size(&bc->instructions[ip]);
    }
}

bytecode* read_bytecode(const char* filename) {
    FILE *file = fopen(filename, "rb");

    const long bytecode_size = get_file_size(file);
    fseek(file, 0, SEEK_SET);

    if (bytecode_size == -1) {
        printf("ERR: Error reading file");
        return NULL;
    }

    bytecode* bc = malloc(sizeof(bytecode));
    uint8_t* instructions = malloc(bytecode_size);
    uint8_t** starts = malloc(sizeof(uint8_t*) * bytecode_size);

    fread((char*)instructions, 1, (int)bytecode_size, file);

    fclose(file);

    bc->instruction_starts = starts;
    bc->instructions = instructions;
    bc->size = bytecode_size;

    calculate_instruction_starts(bc);

    return bc;
}

void free_bytecode(bytecode* bc) {
    free(bc->instructions);
    free(bc->instruction_starts);
    free(bc);
}
