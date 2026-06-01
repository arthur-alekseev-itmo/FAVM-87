#include "onef_stdlib.h"

#include <stdlib.h>

Closure* alloc_closure(const uint8_t* function_address, const uint8_t arity) {
    Closure* closure = malloc(sizeof(Closure));
    closure->function_address = function_address;
    closure->arity = arity;
    uint64_t* args = malloc(sizeof(uint64_t*) * arity);
    closure->arguments = args;
    closure->applied = 0;
    closure->is_builtin = false;
    return closure;
}

Closure* alloc_builtin(const uint8_t* function, const uint8_t arity) {
    Closure* closure = alloc_closure(function, arity);
    closure->is_builtin = true;
    return closure;
}

Closure* copy_closure(const Closure* old) {
    Closure* new = alloc_closure(old->function_address, old->arity);
    new->is_builtin = old->is_builtin;
    new->applied = old->applied;
    for (int i = 0; i < new->applied; i++) {
        new->arguments[i] = old->arguments[i];
    }
    return new;
}

void free_closure(Closure* closure) {
    free(closure->arguments);
    free(closure);
}

Tuple* alloc_tuple(uint8_t size) {
    Tuple* tuple = malloc(sizeof(Tuple));
    tuple->values = malloc(sizeof(uint64_t) * size);
    tuple->size = size;
    return tuple;
}

Tuple* copy_tuple(Tuple* tuple) {
    Tuple* copy = alloc_tuple(tuple->size);
    for (int i = 0; i < tuple->size; i++) {
        copy->values[i] = tuple->values[i];
    }
    return copy;
}

Variant* alloc_variant(uint64_t tag, uint64_t* value) {
    Variant* var = malloc(sizeof(Variant));
    var->tag = tag;
    var->value = value;
    return var;
}
