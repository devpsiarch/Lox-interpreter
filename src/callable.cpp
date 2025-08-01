#include "../include/callable.h"

/* Usual callable generic class implimentation */

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

/* Function implimentation here , since Function -> Callable (as in inherits) */

// we are adding "closures" to functions , as in a function keeps track of its "global"
// variables when delcared inside another function (bit crazy but some ppl like this shit)
Function::Function(FunStatement* stmts,environment* borrowed_env){
    this->declaration = stmts;
    this->colosure = borrowed_env;
}

std::any Function::Call(Interpreter& inter, const std::vector<std::any>& args){
    environment* f = new environment(*this->colosure);
    for (size_t i = 0; i < declaration->params.size(); ++i) {
        const Token& t = declaration->params[i];
        f->define(t.lexeme, args[i]);
    }
    try {
        inter.executeBlock(declaration->body->stmts, f);
    }catch(const Interpreter::ReturnFlow&e){
        return e.ret_value;
    }
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

Function::~Function(){
    delete this->declaration;
    this->colosure = nullptr;
}
