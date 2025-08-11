#include "../include/environment.h"


environment* environment::push_env_frame(environment* new_env){
    new_env->closing = this;
    return new_env;
}

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


void environment::clean_env_frame(){
    for(auto it = SymbolTable.begin() ; it != SymbolTable.end() && it->second.second == ScopeSpace::LOCAL;){
        const std::type_info& t = it->second.first.type();
        
        if(t == typeid(Callable*)) {
            Callable* p = std::any_cast<Callable*>(it->second.first);
            Function* fun = dynamic_cast<Function*>(p);
            if(fun != nullptr){
                // we loose the definition pointer since the main 
                // execution Intrepreter will clean it for us (its a part of code )
                // dont judge my API fr
                fun->declaration = nullptr;
                delete fun;
                fun = nullptr;
            }
            // we loose the pointer pupusfully , since the main execution will 
            // delete all the function definitions
            p = nullptr;
            it = SymbolTable.erase(it);
        }else{
            ++it;
        }
    }
}


environment::environment(){
    this->closing = nullptr;
}
environment::environment(environment&other) : SymbolTable(other.SymbolTable){
    this->closing = (other.closing != nullptr) ? new environment(*other.closing) : nullptr;
}
environment& environment::operator=(environment&other){
    if(&other != this){
        this->SymbolTable = other.SymbolTable;
        this->closing = (other.closing != nullptr) ? new environment(*other.closing) : nullptr;
    }
    return *this;
}


environment::~environment(){
    // we delete everything that is defined in ScopeSpace::LOCAL
    for(auto it = SymbolTable.begin() ; it != SymbolTable.end() && it->second.second == ScopeSpace::LOCAL;){
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
    // this->closing == nullptr means that we are on top of the stack frame
    // meaning, this is the newest environment added (most nested for now)
    if(this->closing == nullptr) return;
    delete this->closing;
    this->closing = nullptr;
}
