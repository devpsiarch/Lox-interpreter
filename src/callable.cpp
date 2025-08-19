#include "../include/callable.h"
#include <any>

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
Function::Function(FunStatement* stmts){
    this->declaration = stmts;
}

std::any Function::Call(Interpreter& inter, const std::vector<std::any>& args){
    environment* f = new environment();
    for (size_t i = 0; i < declaration->params.size(); ++i) {
        const Token& t = declaration->params[i];
        const std::type_info& type = args[i].type();
        // if the object passed to the function is a callable 
        // we pass it as external and wont be killed when the call is done 
        // else we pass it as a local (fits the values)
        // this allows lots of type to by passed , like refrences

        if(type == typeid(Callable*)){
            f->define(t.lexeme, args[i],ScopeSpace::EXTERNAL);
        }else{
            f->define(t.lexeme, args[i],ScopeSpace::LOCAL);
        }
    }
    try {
        // this function bellow "executeBlock" will delete the given environment after it is done
        inter.executeBlock(declaration->body, f);
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
    // if you want to log
    //log_delete(this->declaration,"Function::~Function");
    if(this->declaration){
        delete this->declaration;
        this->declaration = nullptr;
    }
}
