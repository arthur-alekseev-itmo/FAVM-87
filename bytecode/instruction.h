#ifndef FAVM87_INSTRUCTION_H
#define FAVM87_INSTRUCTION_H

typedef enum {
    LoadInt =  0x1,
    LoadString = 0x2,
    LoadLocal = 0x3,
    StoreLocal = 0x4,
    LoadGlobal = 0x5,
    StoreGlobal = 0x6,
    BranchTrue = 0x7,
    BranchFalse = 0x8,
    Branch = 0x9,
    CmpEq = 0x14,
    FunctionAddress = 0x20,
    Apply = 0x21,
    ConstructTuple = 0x22,
    DeconstructTuple = 0x23,
    Drop = 0x24,
    GetVariantTag = 0x25,
    GetVariantContent = 0x26,
    ConstructVariant = 0x27,
    Exit = 0x28,
    Return = 0x29,
    Dup = 0x30,
    Sigbus = 0xff
} Opcode;

#endif //FAVM87_INSTRUCTION_H