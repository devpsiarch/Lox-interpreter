#pragma once
#include <map>
#include <string>
#include <any>
#include <exception>
#include <memory>

class environment {
private:
    std::map<std::string,std::any> SymbolTable;
    std::unique_ptr<environment> closing;
public:
    environment() = default;
    environment(environment&other) : 
        SymbolTable(other.SymbolTable) , 
        closing(other.closing ? std::make_unique<environment>(*other.closing) : nullptr)
    {}
    environment& operator=(environment&other){
        if(&other != this){
            this->SymbolTable = other.SymbolTable;
            if(other.closing)
                this->closing = std::make_unique<environment>(*other.closing);
            else {
                closing.reset();
            }
        }
        return *this;
    }
    ~environment() = default;
    void define(const std::string&key,std::any value); // defines a value to a key
    void assign(const std::string&key,std::any value); // asigns a value to a key
    std::any get(const std::string&key); // gets the value from the table
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
