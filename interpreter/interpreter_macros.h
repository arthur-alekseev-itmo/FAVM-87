#ifndef FAVM87_INTERPRETER_MACROS_H
#define FAVM87_INTERPRETER_MACROS_H

typedef uint64_t* stack_ptr;

#define POP(name) const uint64_t name = *(--stack)
#define READ(name, offset) const uint64_t name = *((stack) - (offset))
#define PUSH(value) *(stack)++ = (int64_t)(value)
#define WRITE(name, offset) *((stack) - (offset)) = (int64_t)(value)

#endif //FAVM87_INTERPRETER_MACROS_H