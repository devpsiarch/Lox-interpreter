#include "../include/environment.h"

void environment::define(const std::string&key,std::any value,ScopeSpace location){
    this->SymbolTable[key] = {value,location};
}
void environment::assign(const std::string&key,std::any value,ScopeSpace location){
    if(this->isKeyIn(key)){
        this->SymbolTable[key] = {value,location};
        return;
    }
    if(this->closing != nullptr){
        this->closing->assign(key,value,location);
        return;
    }
    std::string reason = "Variable \'" + key + "\' not defined , it cant be assigned.";
    throw NameError(reason.c_str());

}
std::pair<std::any,ScopeSpace> environment::get(const std::string&key){
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

environment::~environment(){
    // we delete everything that is defined in ScopeSpace::LOCAL
    for(auto it = SymbolTable.begin() ; it != SymbolTable.end() && it->second.second != ScopeSpace::EXTERNAL;){
        const std::type_info& t = it->second.first.type();
        
        if(t == typeid(Callable*)) {
            Callable* p = std::any_cast<Callable*>(it->second.first);
            delete p;
            p = nullptr;
            it = SymbolTable.erase(it);
        }else{
            ++it;
        }
    }
    delete this->closing;
    this->closing = nullptr;
}
