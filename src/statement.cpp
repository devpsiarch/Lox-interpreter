#include "../include/statement.h"

// its not like i can do anything here huh ?

std::any Statement::Visitor::visitExpressionStatement(ExpressionStatement* estmt){
    return nullptr;
}
std::any Statement::Visitor::visitPrintStatement(PrintStatement* pstmt){
    return nullptr;
}
