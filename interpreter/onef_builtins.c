#include "onef_builtins.h"
#import "onef_stdlib.h"

#include <stdio.h>
#include <stdlib.h>

#define BUILTIN_IMPL(name) stack_ptr name(stack_ptr stack, stack_ptr locals)
#define IMPL_END return stack

#define BUILTIN_TYPED_OPERATOR_IMPL(name, t, op) BUILTIN_IMPL(name) { \
    POP(b);                                                           \
    POP(a);                                                           \
    PUSH((t)(a) op (t)(b));                                           \
    IMPL_END;                                                         \
}

#define RETURN_BUILTIN(name, arity) return alloc_builtin(name, arity);

BUILTIN_TYPED_OPERATOR_IMPL(iadd, int64_t, +);
BUILTIN_TYPED_OPERATOR_IMPL(isub, int64_t, -);
BUILTIN_TYPED_OPERATOR_IMPL(imul, int64_t, *);
BUILTIN_TYPED_OPERATOR_IMPL(idiv, int64_t, /);
BUILTIN_TYPED_OPERATOR_IMPL(imod, int64_t, %);

BUILTIN_TYPED_OPERATOR_IMPL(fadd, double, +);
BUILTIN_TYPED_OPERATOR_IMPL(fsub, double, -);
BUILTIN_TYPED_OPERATOR_IMPL(fmul, double, *);
BUILTIN_TYPED_OPERATOR_IMPL(fdiv, double, /);

BUILTIN_TYPED_OPERATOR_IMPL(cmpeq, uint64_t, ==);
BUILTIN_TYPED_OPERATOR_IMPL(cmpneq, uint64_t, !=);
BUILTIN_TYPED_OPERATOR_IMPL(cmple, uint64_t, <=);
BUILTIN_TYPED_OPERATOR_IMPL(cmplt, uint64_t, <);
BUILTIN_TYPED_OPERATOR_IMPL(cmpge, uint64_t, >=);
BUILTIN_TYPED_OPERATOR_IMPL(cmpgt, uint64_t, >);

BUILTIN_TYPED_OPERATOR_IMPL(and, uint64_t, &);
BUILTIN_TYPED_OPERATOR_IMPL(or, uint64_t, |);

BUILTIN_IMPL(not) {
    POP(v);
    PUSH(~v);
    IMPL_END;
}

BUILTIN_IMPL(print_line) {
    POP(str);
    printf("%s\n", (char*)str);
    PUSH(0);
    IMPL_END;
}

BUILTIN_IMPL(string_of_int) {
    POP(i);
    char* str = malloc(30);
    snprintf(str, 30, "%lld", (int64_t)i);
    PUSH(str);
    IMPL_END;
}

BUILTIN_IMPL(list_cons) {
    POP(h);
    POP(t);
    const Tuple* tuple = alloc_tuple(2);
    tuple->values[0] = (uint64_t*)h;
    tuple->values[1] = (uint64_t*)t;
    const Variant* joint = alloc_variant(-1, (uint64_t*)tuple);
    PUSH(joint);
    IMPL_END;
}

BUILTIN_IMPL(list_of_string) {
    POP(raw_string);
    char* string = (char*)raw_string;
    int i = 0;
    Variant* list = alloc_variant(-2, 0);
    while (string[i++] != '\0') {
        const Tuple* tuple = alloc_tuple(2);
        tuple->values[0] = (uint64_t*)(uint64_t)string[i - 1];
        tuple->values[1] = (uint64_t*)list;
        list = alloc_variant(-2, (uint64_t*)tuple);
    }
    PUSH(list);
    IMPL_END;
}

Closure* create_builtin(const int64_t index) {
    switch (index) {
    case -1: RETURN_BUILTIN(cmplt, 2);
    case -2: RETURN_BUILTIN(cmpgt, 2);
    case -3: RETURN_BUILTIN(cmpeq, 2);
    case -4: RETURN_BUILTIN(cmpneq, 2);
    case -5: RETURN_BUILTIN(cmpge, 2);
    case -6: RETURN_BUILTIN(cmple, 2);
    case -7: RETURN_BUILTIN(list_cons, 2);
    case -8: RETURN_BUILTIN(iadd, 2);
    case -9: RETURN_BUILTIN(isub, 2);
    case -10: RETURN_BUILTIN(imul, 2);
    case -11: RETURN_BUILTIN(idiv, 2);
    case -12: RETURN_BUILTIN(imod, 2);
    case -13: RETURN_BUILTIN(fadd, 2);
    case -14: RETURN_BUILTIN(fmul, 2);
    case -15: RETURN_BUILTIN(and, 2);
    case -16: RETURN_BUILTIN(or, 2);
    case -17: RETURN_BUILTIN(print_line, 1);
    /* case -18: RETURN_BUILTIN(read_line, 0);
    case -19: RETURN_BUILTIN(debug, 1); */
    case -20: RETURN_BUILTIN(list_of_string, 1);
    /* case -21: RETURN_BUILTIN(string_of_list, 1);
    case -22: RETURN_BUILTIN(int_of_char, 1);
    case -23: RETURN_BUILTIN(char_of_int, 1);
    case -24: RETURN_BUILTIN(int_of_string, 1); */
    case -25: RETURN_BUILTIN(string_of_int, 1);
    /* case -26: RETURN_BUILTIN(read_file, 1); */
    case -27: RETURN_BUILTIN(not, 1);
    default:
        printf("ERR: Unknown builtin: %d", index);
        exit(1);
    }
}
