#pragma once
#include "../include/lexer.h"
#include "../include/expr.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

namespace interface {
    void interpret_file(const char*filename);
    void eval_expr(const std::string&line);
    void run_line(Interpreter&inter,const std::string&line);
    void runPrompt();
};

