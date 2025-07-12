#include "../include/environment.h"

void environment::define(const std::string&key,std::any value){
    this->SymbolTable[key] = value;
}
void environment::assign(const std::string&key,std::any value){
    if(this->isKeyIn(key)){
        this->define(key,value);
        return;
    }
    std::string reason = "Variable \'" + key + "\' not defined , it cant be assigned.";
    throw NameError(reason.c_str());
}
std::any environment::get(const std::string&key){
    if(this->isKeyIn(key)){
        return this->SymbolTable[key];
    } 
    std::string reason = "Variable \'" + key + "\' not defined.";
    throw NameError(reason.c_str());
}
bool environment::isKeyIn(const std::string&key){
    return this->SymbolTable.find(key) != this->SymbolTable.end();
}
