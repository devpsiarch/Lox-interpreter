#include "../include/callable.h"

std::any Callable::Call(Interpreter&inter,const std::vector<std::any>&args){
    (void)inter;
    (void)args;
    return nullptr;
}
int Callable::getArity() const {
    return -1;
}
std::string Callable::toString() const{
    return "<weird state fn>";
}

Function::Function(FunStatement* stmts){
    this->declaration = stmts;
}

std::any Function::Call(Interpreter& inter, const std::vector<std::any>& args){
    environment* f = new environment();
    for (size_t i = 0; i < declaration->params.size(); ++i) {
        const Token& t = declaration->params[i];
        f->define(t.lexeme, args[i]);
    }
    inter.executeBlock(declaration->body->stmts, f);
    return nullptr;
}

int Function::getArity() const
{
    return static_cast<int>(declaration->params.size());
}

std::string Function::toString() const
{
    return "<userdefined fn " + declaration->name.lexeme +">";
}

Function::~Function()
{
    delete declaration;
}
