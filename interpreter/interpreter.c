#include "interpreter.h"

#include <stdio.h>

#include "../bytecode/instruction.h"

#include <stdlib.h>
#include <string.h>

#include "interpreter_macros.h"
#include "onef_stdlib.h"
#include "onef_builtins.h"

#define INSTR_IMPL(name) stack_ptr name(const uint8_t* ip, const bytecode* bc, stack_ptr stack, stack_ptr locals, stack_ptr globals)
#define INSTR_DECL(name) INSTR_IMPL(name)
#define INSTR_FIN(size) [[clang::musttail]] return next(ip + size, bc, stack, locals, globals)
#define AS(type, expr) *(uint64_t*)(expr)
#define GOTO_INSTR(name) [[clang::musttail]] return name(ip, bc, stack, locals, globals)
#define BRANCH(address) [[clang::musttail]] return next(address, bc, stack, locals, globals)
#define CALL(address) next(address, bc, stack, locals + 32, globals)

#define DEBUG

#ifdef DEBUG
    #define DEBUG_LOG(pattern, ...) printf(pattern, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(patter, ...)
#endif

INSTR_DECL(load_int);
INSTR_DECL(load_string);
INSTR_DECL(load_local);
INSTR_DECL(store_local);
INSTR_DECL(load_global);
INSTR_DECL(store_global);
INSTR_DECL(branch_true);
INSTR_DECL(branch_false);
INSTR_DECL(branch);
INSTR_DECL(cmp_eq);
INSTR_DECL(function_address);
INSTR_DECL(apply);
INSTR_DECL(cons_tuple);
INSTR_DECL(decons_tuple);
INSTR_DECL(drop);
INSTR_DECL(stop);
INSTR_DECL(ret);
INSTR_DECL(get_variant_tag);
INSTR_DECL(get_variant_content);
INSTR_DECL(cons_variant);
INSTR_DECL(dup);

typedef stack_ptr (* builtin_fun)(stack_ptr, stack_ptr);

INSTR_IMPL(next) {
    DEBUG_LOG("Current stack: %p; locals: %p; ", stack, locals);
    Opcode opcode = (Opcode)(*ip);
    switch (opcode) {
    case LoadInt: GOTO_INSTR(load_int);
    case LoadString: GOTO_INSTR(load_string);
    case LoadLocal: GOTO_INSTR(load_local);
    case StoreLocal: GOTO_INSTR(store_local);
    case LoadGlobal: GOTO_INSTR(load_global);
    case StoreGlobal: GOTO_INSTR(store_global);
    case BranchTrue: GOTO_INSTR(branch_true);
    case BranchFalse: GOTO_INSTR(branch_false);
    case Branch: GOTO_INSTR(branch);
    case CmpEq: GOTO_INSTR(cmp_eq);
    case FunctionAddress: GOTO_INSTR(function_address);
    case Apply: GOTO_INSTR(apply);
    case ConstructTuple: GOTO_INSTR(cons_tuple);
    case DeconstructTuple: GOTO_INSTR(decons_tuple);
    case Drop: GOTO_INSTR(drop);
    case GetVariantTag: GOTO_INSTR(get_variant_tag);
    case GetVariantContent: GOTO_INSTR(get_variant_content);
    case ConstructVariant: GOTO_INSTR(cons_variant);
    case Exit: GOTO_INSTR(stop);
    case Return: GOTO_INSTR(ret);
    case Dup: GOTO_INSTR(dup);
        /* Sigbus = 0xff */
    default:
        DEBUG_LOG("Unknown instruction, %p", *ip);
        exit(1);
    }
}

INSTR_IMPL(load_int) {
    DEBUG_LOG("LOAD_INT\n");
    const uint64_t i = AS(uint64_t, ip + 1);
    PUSH(i);
    INSTR_FIN(9);
}

INSTR_IMPL(load_string) {
    DEBUG_LOG("LOAD_STRING\n");
    const uint64_t string_length = AS(uint64_t, ip + 1);
    char* content = malloc(string_length + 1);
    memcpy(content, ip + 9, string_length);
    content[string_length] = '\0';
    PUSH(content);
    INSTR_FIN(9 + string_length);
}

INSTR_IMPL(load_local) {
    const uint64_t index = AS(uint64_t, ip + 1);
    DEBUG_LOG("LOAD_LOCAL %llu\n", index);
    PUSH(locals[index]);
    INSTR_FIN(9);
}

INSTR_IMPL(store_local) {
    const uint64_t index = AS(uint64_t, ip + 1);
    DEBUG_LOG("STORE_LOCAL %llu\n", index);
    POP(value);
    locals[index] = value;
    INSTR_FIN(9);
}

INSTR_IMPL(function_address) {
    const int64_t instruction_id = AS(int64_t, ip + 1);
    DEBUG_LOG("FN_ADDR %llu\n", instruction_id);

    if (instruction_id < 0) {
        DEBUG_LOG("BUILTIN ADDRESS %lld\n", instruction_id);
        const Closure* closure = create_builtin(instruction_id);
        PUSH(closure);
        INSTR_FIN(10);
    }

    const uint8_t arity = AS(uint8_t, ip + 9);
    const uint8_t* address = bc->instruction_starts[instruction_id];
    DEBUG_LOG("FUNCTION ADDRESS %d (%p)\n", instruction_id, address);
    const Closure* closure = alloc_closure(address, arity);
    PUSH(closure);
    INSTR_FIN(10);
}

INSTR_IMPL(branch_true) {
    DEBUG_LOG("BRANCH_TRU\n");
    const uint64_t instruction_id = AS(uint64_t, ip + 1);
    POP(cond);
    if (cond) {
        const uint8_t* address = bc->instruction_starts[instruction_id];
        BRANCH(address);
    }
    INSTR_FIN(9);
}

INSTR_IMPL(branch_false) {
    DEBUG_LOG("BRANCH_FLS\n");
    const uint64_t instruction_id = AS(uint64_t, ip + 1);
    POP(cond);
    if (!cond) {
        const uint8_t* address = bc->instruction_starts[instruction_id];
        BRANCH(address);
    }
    INSTR_FIN(9);
}

INSTR_IMPL(branch) {
    DEBUG_LOG("BRANCH\n");
    const uint64_t instruction_id = AS(uint64_t, ip + 1);
    const uint8_t* address = bc->instruction_starts[instruction_id];
    BRANCH(address);
}

INSTR_IMPL(cmp_eq) {
    DEBUG_LOG("EQ\n");
    POP(x);
    POP(y);
    PUSH(x == y);
    INSTR_FIN(1);
}

INSTR_IMPL(apply) {
    DEBUG_LOG("APPLY\n");
    POP(raw_closure);
    POP(arg);
    const Closure* old_closure = (Closure*)raw_closure;
    Closure* closure = copy_closure(old_closure);
    closure->arguments[closure->applied++] = arg;

    if (closure->applied == closure->arity && closure->is_builtin) {
        const builtin_fun fun = (builtin_fun)closure->function_address;
        for (int i = 0; i < closure->arity; i++) {
            PUSH(closure->arguments[i]);
        }
        stack = fun(stack, locals);
        INSTR_FIN(1);
    }

    if (closure->applied == closure->arity) {
        for (int i = 0; i < closure->arity; i++) {
            PUSH(closure->arguments[i]);
        }
        DEBUG_LOG("CALL %p\n", closure->function_address);
        stack = CALL(closure->function_address);
        INSTR_FIN(1);
    }

    PUSH(closure);
    INSTR_FIN(1);
}

INSTR_IMPL(ret) {
    DEBUG_LOG("RETURN\n");
    return stack;
}

INSTR_IMPL(load_global) {
    DEBUG_LOG("LOAD_GLOBAL\n");
    const uint64_t index = AS(uint64_t, ip + 1);
    PUSH(globals[index]);
    INSTR_FIN(9);
}

INSTR_IMPL(store_global) {
    DEBUG_LOG("LOAD_GLOBAL\n");
    const uint64_t index = AS(uint64_t, ip + 1);
    POP(value);
    globals[index] = value;
    INSTR_FIN(9);
}

INSTR_IMPL(drop) {
    DEBUG_LOG("DROP\n");
    POP(_);
    INSTR_FIN(1);
}

INSTR_IMPL(stop) {
    DEBUG_LOG("STOP\n");
    exit(0);
}

INSTR_IMPL(cons_tuple) {
    DEBUG_LOG("CONSTRUCT_TUPLE\n");
    const uint64_t size = AS(uint64_t, ip + 1);
    const Tuple* tuple = alloc_tuple(size);
    for (int i = 0; i < size; i++) {
        POP(v);
        tuple->values[i] = (uint64_t*)v;
    }
    PUSH(tuple);
    INSTR_FIN(9);
}

INSTR_IMPL(decons_tuple) {
    DEBUG_LOG("DECONSTRUCT_TUPLE\n");
    POP(raw_tuple);
    const Tuple* tuple = (Tuple*)raw_tuple;
    for (int i = 0; i < tuple->size; i++) {
        PUSH(tuple->values[i]);
    }
    INSTR_FIN(9);
}

INSTR_DECL(get_variant_tag) {
    DEBUG_LOG("GET_TAG\n");
    POP(raw_ctor);
    const Variant* variant = (Variant*)raw_ctor;
    PUSH(variant->tag);
    INSTR_FIN(1);
}

INSTR_DECL(get_variant_content) {
    DEBUG_LOG("GET_CONTENT\n");
    POP(raw_ctor);
    const Variant* variant = (Variant*)raw_ctor;
    PUSH(variant->value);
    INSTR_FIN(1);
}

INSTR_DECL(cons_variant) {
    DEBUG_LOG("CONSTRUCT_VARIANT\n");
    POP(content);
    const uint64_t tag = AS(uint64_t, ip + 1);
    const Variant* variant = alloc_variant(tag, (uint64_t*)content);
    PUSH(variant);
    INSTR_FIN(9);
}

INSTR_IMPL(dup) {
    DEBUG_LOG("DUP\n");
    POP(v);
    PUSH(v);
    PUSH(v);
    INSTR_FIN(1);
}

int run_bytecode(const bytecode* bc) {
    stack_ptr stack = calloc(0x10000, 1);
    stack_ptr locals = calloc(0x10000, 1);
    stack_ptr globals = calloc(0x1000, 1);

    next(bc->instructions, bc, stack, locals, globals);

    free(stack);
    free(locals);
    free(globals);

    return 0;
}