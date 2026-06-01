#ifndef FAVM87_ONEF_STDLIB_H
#define FAVM87_ONEF_STDLIB_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint64_t* arguments;
    uint8_t arity;
    uint8_t applied;
    bool is_builtin;
    const uint8_t* function_address;
} Closure;

Closure* alloc_closure(const uint8_t* function_address, uint8_t arity);
Closure* alloc_builtin(const uint8_t* address, uint8_t arity);
Closure* copy_closure(const Closure* old);
void free_closure(Closure* closure);

typedef struct {
    uint8_t size;
    uint64_t** values;
} Tuple;

Tuple* alloc_tuple(uint8_t size);
Tuple* copy_tuple(Tuple* tuple);
void free_tuple(Tuple* tuple);

typedef struct {
    uint64_t tag;
    uint64_t* value;
} Variant;

Variant* alloc_variant(uint64_t tag, uint64_t* value);
void free_variant(Variant* variant);

#endif //FAVM87_ONEF_STDLIB_H