cmake_minimum_required(VERSION 4.0)
project(FAVM87 C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-fsanitize=address -g)
add_link_options(-fsanitize=address)

add_executable(FAVM87 main.c
        bytecode/bytecode.c
        bytecode/bytecode.h
        bytecode/instruction.h
        bytecode/instruction.c
        interpreter/interpreter.h
        interpreter/interpreter.c
        interpreter/onef_stdlib.h
        interpreter/onef_stdlib.c
        interpreter/onef_builtins.h
        interpreter/onef_builtins.c
        interpreter/interpreter_macros.h)
