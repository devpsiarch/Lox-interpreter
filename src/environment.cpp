#include "../include/environment.h"

void environment::define(const std::string&key,std::any value){
    this->SymbolTable[key] = value;
}
void environment::assign(const std::string&key,std::any value){
    if(this->isKeyIn(key)){
        this->SymbolTable[key] = value;
        return;
    }
    if(this->closing != nullptr){
        this->closing->assign(key,value);
        return;
    }
    std::string reason = "Variable \'" + key + "\' not defined , it cant be assigned.";
    throw NameError(reason.c_str());

}
std::any environment::get(const std::string&key){
    if(this->isKeyIn(key)){
        return this->SymbolTable[key];
    }
    if(this->closing != nullptr) return this->closing->get(key);
    std::string reason = "Variable \'" + key + "\' not defined.";
    throw NameError(reason.c_str());
}
bool environment::isKeyIn(const std::string&key){
    return this->SymbolTable.find(key) != this->SymbolTable.end();
}

void environment::kill_but_borrowed_functions(){
    for(auto it = SymbolTable.begin() ; it != SymbolTable.end();){
        const std::type_info& t = it->second.type();
        
        if(t == typeid(Callable*)) {
            // pull the Foo* out, delete it
            Callable* p = std::any_cast<Callable*>(it->second);
            it = SymbolTable.erase(it);
        }else{
            ++it;
        }
    }
}
environment::~environment(){
    for(auto it = SymbolTable.begin() ; it != SymbolTable.end();){
        const std::type_info& t = it->second.type();
        
        if(t == typeid(Callable*)) {
            // pull the Foo* out, delete it
            Callable* p = std::any_cast<Callable*>(it->second);
            delete p;
            it = SymbolTable.erase(it);
        }else{
            ++it;
        }
    }
    delete this->closing;
}
