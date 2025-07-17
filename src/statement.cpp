#include "../include/statement.h"

// its not like i can do anything here huh ?

// NOTE: these functions are not (for now) intended to be usefull here 
// they are inherited by the interpreter then overriden to do the accual interpreting work
std::any Statement::Visitor::visitExpressionStatement(ExpressionStatement* estmt){
    (void)estmt;
    return nullptr;
}
std::any Statement::Visitor::visitPrintStatement(PrintStatement* pstmt){
    (void)pstmt;
    return nullptr;
}
std::any Statement::Visitor::visitDeclareStatement(DeclareStatement* dstmt){
    (void)dstmt;
    return nullptr;
}

std::any Statement::Visitor::visitBlockStatement(BlockStatement* bstmt){
    (void)bstmt;
    return nullptr;
}
std::any Statement::Visitor::visitIfStatement(IfStatement* ifstmt){
    (void)ifstmt;
    return nullptr;
}
std::any Statement::Visitor::visitWhileStatement(WhileStatement* wstmt){
    (void)wstmt;
    return nullptr;
}
std::any Statement::Visitor::visitForStatement(ForStatement* fstmt){
    (void)fstmt;
    return nullptr;
}
std::any Statement::Visitor::visitBreakStatement(BreakStatement* brstmt){
    (void)brstmt;
    return nullptr;
}
std::any Statement::Visitor::visitContinueStatement(ContinueStatement* cstmt){
    (void)cstmt;
    return nullptr;
}
std::any Statement::Visitor::visitFunStatement(FunStatement* funstmt){
    (void)funstmt;
    return nullptr;
}
