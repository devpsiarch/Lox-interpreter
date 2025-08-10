#pragma once
#include <map>
#include <string>
#include <any>
#include <exception>
#include <memory>
#include <iostream>
#include "./tool/tool.h"
#include "./callable.h"

/* 
 * The idea for the environment 
 * well have a "std::string" name that binds a "std::any" object and "Type" that 
 * dectates if the object was created in "this" scope or "above"/"not-this" 
 * then, when we delete the object from the env , we only delete the objects that 
 * where created in the "this" same environement*/

// maybe well define more for diff stuff
enum ScopeSpace {
    LOCAL,
    EXTERNAL,
};

class environment {
public:
    std::map<std::string,std::pair<std::any,ScopeSpace>> SymbolTable;
    environment* closing;
    environment(){
        this->closing = nullptr;
    }
    environment(environment&other) : SymbolTable(other.SymbolTable)
    {
        this->closing = (other.closing != nullptr) ? new environment(*other.closing) : nullptr;
    }
    environment& operator=(environment&other){
        if(&other != this){
            this->SymbolTable = other.SymbolTable;
            this->closing = (other.closing != nullptr) ? new environment(*other.closing) : nullptr;
        }
        return *this;
    }
    ~environment();
    void define(const std::string&key,std::any value,ScopeSpace location); // defines a value to a key
    void assign(const std::string&key,std::any value,ScopeSpace location); // asigns a value to a key
    std::pair<std::any,ScopeSpace> get(const std::string&key); // gets the value from the table
    bool isKeyIn(const std::string&key); // check if a key is in the table

    class NameError : std::exception {
    private:
        std::string err_msg;
    public:
        NameError() = delete;
        NameError(const char*reason){
            this->err_msg = reason;
        }
        const char* what() const noexcept override {
            return err_msg.c_str();
        }
    };

};
