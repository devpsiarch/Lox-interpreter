#pragma once
#include "./lexer.h"
#include "./expr.h"
#include <exception>
#include <memory>
// we define the complete grammer for the language as follows 
/*  

        expression     → conditional;

        conditional    → equality ( "?" expression ":" expression )? ;

        equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        comparison     → term       ( ( ">"  | ">=" ) term       )* ;
        term           → factor     ( ( "-"  | "+"  ) factor     )* ;
        factor         → unary      ( ( "/"  | "*"  ) unary      )* ;
        unary          → ( "!" | "-" ) unary
                        | primary ;
        primary        → NUMBER | STRING | "true" | "false" | "nil"
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
    Expression* primary();

    void synchronize(void); // when encourtering a syntax error , we are suppoed to skip 
                            // ahead then contiue parsing to detect as many erros as possible
                            // well use it later ...
    public: Expression* parse();
};
