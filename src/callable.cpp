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
