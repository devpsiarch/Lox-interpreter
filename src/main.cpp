#include "../include/lexer.h"
// reads the file content to std::string
std::string read_file(const std::string&filename);
// interprits the whole file at one
void runFile(const std::string&filename);
// interprites as "REPl".
void runPrompt();

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
int main(int argc,char *argv[]){
    Lexer l = Lexer(read_file("lox_examples/test.lox"));
    l.scanSource();
    for(Token&t:l.tokens){
        std::cout << t.toString() << '\n';
    }
    return 0;
    if(argc > 2 ){
        std::cout << "usage: ./main <script.lox>\n";
        return 1;
    }else if(argc == 2){
        // run in interactive mode
        runFile(argv[1]);
    }else if(argc == 1){
        // interprite the source code
        runPrompt();
    }
    return 0;
}
