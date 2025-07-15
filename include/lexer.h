#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include <cmath>
#include "./tool/tool.h"

enum TokenType {
    // signifies no type 
    NONE = 0,

    // single char tokens 
    LEFT_PAREN,RIGHT_PAREN,LEFT_BRACE,RIGHT_BRACE,
    COMMA,DOT,MINUS,PLUS,COLEN,SEMICOLEN,SLASH,STAR,TERNARY,

    // One or two char long
    BANG,BAND_EQUAL,
    EQUAL,EQUAL_EQUAL,
    GREATER,GREATER_EQUAL,
    LESS,LESS_EQUAL,

    // literal 
    STRING,NUMBER,IDENTIFIER,

    // key words
    AND,CLASS,ELSE,FALSE,FUN,FOR,IF,NIL,OR,
    PRINT,RETURN,SUPER,THIS,TRUE,VAR,WHILE,
    BREAK,CONTINUE,

    // refers to the end of the file
    EOFF
};

// returns std::string literal of the TokenType enum
std::string type_to_string(enum TokenType);

// this class is here to help us print 
// error while we are interpriting
class Logger {
public:
    static void report(int line,int col,std::string&where,std::string&msg);
    static void error(int line,int col,std::string&msg);
};

class Token {
public:
    enum TokenType type;
    std::string lexeme;
    std::any literal;
    size_t line;
    size_t col;
    Token(enum TokenType t,
          const std::string&l,
          std::any lit = nullptr,
          size_t line = 0,
          size_t col = 0);
    Token(const Token&other);
    Token& operator=(const Token&other);
    std::string toString();
};

// this is our lexer and its main thing 
// read source and convert it to a vector of tokens
class Lexer {
public:
    std::vector<Token> tokens;
    std::string source;
    size_t start;     // points to the start of lexeme being lexed
    size_t current;   // current char
    size_t line;      //current line
    size_t start_line;// tells me where is the start of the line 
                      // to get the col relative to the line 
    // some constuctors and shit
    
    Lexer() = default;
    Lexer(const std::string&source_code);
    Lexer(const Lexer&other);
    Lexer& operator=(const Lexer&other);
    
    // helper functionality
    private:bool isAtEnd(); // tells us if we are done with the source.
    private:char advance(); // consumes current to next char in source.
    private:char peek(); // advances without consuming form source.
    private:char peekNext();// return next of current without consume.
    private:void addToken(enum TokenType type,std::any a);
    private:void addToken(enum TokenType);
    private:bool match(const char expected);
    private:void skipLine();// skips until endline
    private:void skipToComment();// skips to the end of comment block.
    private:std::string getStringLiteral();// returns the str literal.
    private:double getNumberLiteral();// returns a number literal.
    private:std::string getIdentifierAndKeyword();// ret key/id string.
    // main functionality 
    private:void scanToken();
    public:void scanSource();   // it scans the souce string 
                                // and populates the
                                // and populate the tokens
public:
    inline static std::unordered_map<std::string,TokenType> Keywords = {
        {"and",    TokenType::AND},
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"false",  TokenType::FALSE},
        {"fun",    TokenType::FUN},
        {"for",    TokenType::FOR},
        {"if",     TokenType::IF},
        {"nil",    TokenType::NIL},
        {"or",     TokenType::OR},
        {"print",  TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super",  TokenType::SUPER},
        {"this",   TokenType::THIS},
        {"true",   TokenType::TRUE},
        {"var",    TokenType::VAR},
        {"while",  TokenType::WHILE},
        {"break",  TokenType::BREAK},
        {"continue",  TokenType::CONTINUE}
    };
};
