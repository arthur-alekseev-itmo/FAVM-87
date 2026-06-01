#ifndef FAVM87_ONEF_BUILTINS_H
#define FAVM87_ONEF_BUILTINS_H

#include <stdint.h>

#include "interpreter_macros.h"
#include "onef_stdlib.h"

#define BUILTIN_DEF(name) stack_ptr name(stack_ptr stack_top, stack_ptr locals)

BUILTIN_DEF(iadd);
BUILTIN_DEF(imul);
BUILTIN_DEF(isub);
BUILTIN_DEF(idiv);
BUILTIN_DEF(imod);

BUILTIN_DEF(fadd);
BUILTIN_DEF(fmul);
BUILTIN_DEF(fsub);
BUILTIN_DEF(fdiv);

BUILTIN_DEF(cmpeq);
BUILTIN_DEF(cmpneq);
BUILTIN_DEF(cmple);
BUILTIN_DEF(cmplt);
BUILTIN_DEF(cmpge);
BUILTIN_DEF(cmpgt);

BUILTIN_DEF(print_line);
BUILTIN_DEF(read_line);
BUILTIN_DEF(debug);
BUILTIN_DEF(list_of_string);
BUILTIN_DEF(string_of_list);
BUILTIN_DEF(int_of_char);
BUILTIN_DEF(char_of_int);
BUILTIN_DEF(int_of_string);
BUILTIN_DEF(string_of_int);
BUILTIN_DEF(read_file);

BUILTIN_DEF(not);
BUILTIN_DEF(and);
BUILTIN_DEF(or);

Closure* create_builtin(int64_t index);

#endif //FAVM87_ONEF_BUILTINS_H
