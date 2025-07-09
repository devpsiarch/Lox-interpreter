#include "../include/tool/tool.h"
#include "../include/expr.h"
#include "../include/lexer.h"
#include "../include/parser.h"

std::string type_to_string(const std::type_info& t){
    if (t == typeid(double)) {
        return "Number";
    }
    else if (t == typeid(bool)) {
        return "Bool"; 
    }
    else if (t == typeid(std::string)) {
        return "String";
    }
    else if (t == typeid(std::nullptr_t)) {
        return "nil";
    }
    else {
        return "nil";
    }
}

std::string stdany_to_string(const std::any& a){
    if(!a.has_value()) return "nil";

    const std::type_info& t = a.type();
    std::ostringstream out;

    if (t == typeid(int)) {
        out << std::any_cast<int>(a);
    }
    else if (t == typeid(double)) {
        out << std::any_cast<double>(a);
    }
    else if (t == typeid(bool)) {
        out << (std::any_cast<bool>(a) ? "true" : "false");
    }
    else if (t == typeid(std::string)) {
        out << std::any_cast<std::string>(a);
    }
    else if (t == typeid(std::nullptr_t)) {
        out << "nil";
    }
    else {
        // fallback: print the mangled type name
        out << "nil";
    }

    return out.str();
}

std::string read_file(const std::string&filename){
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
