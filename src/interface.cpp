#include "../include/interface.h"

void interface::interpret_file(const char*filename){
    Expression::Visitor v;
    Lexer l = Lexer(read_file(filename));
    l.scanSource();
    // pasing <syntatic analysis>
    parser p = parser(l.tokens);
    // we can out more step here before the parsing 
    // like if lox was staticly typed , we would check the types here
    // we can also put a <symantic analysis> layer here and resolve a bunch of variables
    // or add a optimlzation layer here
    std::vector<Statement*> stmnt =  p.parserProgram();
    Interpreter inter;
    inter.InterpretProgram(stmnt);
}


void interface::eval_expr(const std::string&line){
    Lexer l = Lexer(line);
    l.scanSource();
    parser p = parser(l.tokens);
    Expression* expr = p.parse();
    if(expr == nullptr) return ;           
    Expression::Visitor v;
    Interpreter inter;
    inter.Interpret(expr);
    std::string ast_str = v.astprinter(expr);
    std::cout << ast_str << '\n';
    delete expr;
    expr = nullptr;
}


void interface::run_line(Interpreter&inter,const std::string&line){
    Lexer l = Lexer(line);
    l.scanSource();
    parser p = parser(l.tokens);
    std::vector<Statement*> stmts = p.parserProgram();
    inter.InterpretProgram(stmts);
}
void interface::runPrompt(){
    std::string line;
    bool shouldclose = false;
    Interpreter inter(true);
    while(!shouldclose){
        // interpritation here
        std::cout << "=> ";
        std::getline(std::cin,line);
        if(line == "exit"){
            shouldclose = true;
            continue;
        }else if(line == "clear"){
            system("clear");
            continue;
        }
        //eval_expr(line);
        run_line(inter,line);
    }
}


