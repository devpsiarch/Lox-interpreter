#pragma once
#include "./lexer.h"
#include "./expr.h"
#include "./statement.h"
#include <exception>
#include <memory>
#include <vector>
// we define the complete grammer for the language as follows 
/*  

        program        → declaration* EOFF ;

        declaration    → varDeclare | statement | funDeclare ;
   
        funDeclare     → "fun" function  ;

        function       → IDENTIFIER '(' parameters? ')' blockstatement; 

        parameters     → IDENTIFIER ( ',' IDENTIFIER )*  ;

        statement      → exprStmt | printStmt | block | 
                        if_statement | while_statement | for_statement | 
                        break_statement | continue_statement;

        blockstatement → '{' declaration* '}' ;

        exprStmt       → expression ';' ;

        printStmt      → "print" expression ";" ;

        if_statement   → "if" '(' expression ')' statement ("else" statement)? ;

        while_statement→ "while" '(' expression ')' statement ;

        break_statement→ "break" ';' ;
        
        continue_statement→ "continue" ';' ;

        for_statement  → "for" '(' ( varDeclare | exprStmt | ';') expression? ";" expression ";" ')' statement;

        expression     → assignement;

        assignement    →  IDENTIFIER "=" assignement | conditional | logic_or;

        logic_or       → logic_and ("or" logic_and)*;
    
        logic_and      → equality ("and" equality)* ;
 
        // down here (in the conditional expression) am assuming that 
        // we have onyl expression in the middle and right , 
        // i dont recall the possiblitly of havinf statements in there place 
        // instread but hey , for now they are good
        
        conditional    → equality ( "?" expression ":" expression )? ;

        equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        comparison     → term       ( ( ">"  | ">=" ) term       )* ;
        term           → factor     ( ( "-"  | "+"  ) factor     )* ;
        factor         → unary      ( ( "/"  | "*" | '%' ) unary      )* ;
        unary          → ( "!" | "-" ) unary 
                        | call ;
        call           → primary ( "(" arguments ")" )*  ;
        arguments      → expression ( "," expression )*  ;
        primary        → NUMBER | STRING | "true" | "false" | "nil" | IDENTIFIER
                        | "(" expression ( "," expression )* ")" ;


    where each "entry" or "element" or "piece" of the grammer is an expression
    that we create recursivly
 * */

class parser {
private:
    class parserError : public std::exception {
    public:
        Token faulty_token;
        std::string msg;
        // here i want every user of this class to use the para constructor
        // because i want to store the token causing the error while parsing
        // for debugging puposes
        parserError() = delete;
        parserError(Token t,const char*reason) : faulty_token(t) {
            this->msg = reason;
        }
        const char* what() const noexcept override {
            return msg.c_str();
        }
    };
    std::vector<Token> tokens;
    size_t current;
public:
    parser(const std::vector<Token>&toks);
    parser(const parser&p);
    parser& operator=(const parser&p);
    ~parser() = default;
   
private:
    bool isAtEnd(); // check if the current token is the end of tokens "EOFF type"
    bool check(TokenType type);// checks if the current token is of said type 
    Token advance();// consumes the current token and returns it
    Token previous(); // returns the previous token 
    Token peek(); // returns the current token without consuming it
    template <typename ...Token> bool match(Token... tokens);

    
    Token consume(TokenType type,const char*msg);
    Expression* expression();
    Expression* conditional();
    Expression* comma_expression();
    Expression* equality();
    Expression* comparison();
    Expression* term();
    Expression* factor();
    Expression* unary();
    Expression* call();
    Expression* get_call(std::unique_ptr<Expression>&caller);
    Expression* primary();
    Expression* assignement();
    Expression* logic_or();
    Expression* logic_and();

    Statement* statement();
    Statement* declaration();

    Statement* print_statement();
    Statement* expression_statement();
    Statement* declare_statement();
    Statement* if_statement();
    Statement* while_statement();
    Statement* for_statement();
    Statement* break_statement();
    Statement* continue_statement();
    Statement* function_statement();

    std::vector<Statement*> block_statement();

    void synchronize(void); // when encourtering a syntax error , we are suppoed to skip 
                            // ahead then contiue parsing to detect as many erros as possible
                            // well use it later ...
public:
    Expression* parse();
    std::vector<Statement*> parserProgram();
};
