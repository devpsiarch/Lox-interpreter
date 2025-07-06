#include "../include/tool/tool.h"

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
        out << "nullptr";
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
void runFile(const std::string&filename){
    std::string source_code = read_file(filename);
    // interpritation here 
    std::cout << "source code : \n" << source_code << '\n'; 
}
void runPrompt(){
    std::string line;
    std::cout << "=> ";
    std::getline(std::cin,line);
    while(line != "END"){
        // interpritation here
        std::cout << "line code : \n" << line << '\n';
        std::getline(std::cin,line);
    }
}
