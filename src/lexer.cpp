#include "../include/lexer.h"
#define TOOL_IMPLI
#include "../include/tool/tool.h"
void Logger::report(int line,int col,std::string&where,std::string&msg){
    std::cout << "[line" << line << ':' << col << "] Error " << where << ": "
        << msg << '\n';
}
void Logger::error(int line,int col,std::string&msg){
    std::string s = "";
    Logger::report(line,col,s,msg);
}

Token::Token(enum TokenType t,
      const std::string&l,
      std::any lit,
      size_t line,
      size_t col){
    this->type = t;
    this->lexeme = l;
    this->literal = lit;
    this->line = line;
    this->col = col;
}
Token::Token(const Token&other) : 
    type(other.type) ,
    lexeme(other.lexeme) , 
    literal(other.literal) , 
    line(other.line) , 
    col(other.col) {}

Token& Token::operator=(const Token&other) {
    if(&other != this){
        this->type = other.type;
        this->lexeme = other.lexeme;
        this->literal = other.literal;
        this->line = other.line;
        this->col = other.col;
    }
    return *this;
}


std::string type_to_string(enum TokenType t){
    switch(t){
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::MINUS: return "MINUS";
        case TokenType::PLUS: return "PLUS";
        case TokenType::SEMICOLEN: return "SEMICOLEN";
        case TokenType::COLEN: return "COLEN";
        case TokenType::TERNARY: return "TERNARY";
        case TokenType::SLASH: return "SLASH";
        case TokenType::STAR: return "STAR";
        case TokenType::MOD: return "MOD";

        case TokenType::BANG: return "BANG";
        case TokenType::BAND_EQUAL: return "BAND_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";

        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";

        case TokenType::AND: return "AND";
        case TokenType::CLASS: return "CLASS";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::FUN: return "FUN";
        case TokenType::FOR: return "FOR";
        case TokenType::IF: return "IF";
        case TokenType::NIL: return "NIL";
        case TokenType::OR: return "OR";
        case TokenType::PRINT: return "PRINT";
        case TokenType::RETURN: return "RETURN";
        case TokenType::SUPER: return "SUPER";
        case TokenType::THIS: return "THIS";
        case TokenType::TRUE: return "TRUE";
        case TokenType::VAR: return "VAR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::BREAK: return "BREAK";
        case TokenType::CONTINUE: return "CONTINUE";

        case TokenType::EOFF: return "EOFF";

        default: return "UNKNOWN_TOKEN";
    }
}

std::string Token::toString(){
    return type_to_string(this->type) +
        " " + this->lexeme +
        " " + stdany_to_string(this->literal) + 
        " " + std::to_string(this->line) + 
        " " + std::to_string(this->col);
}


Lexer::Lexer(const Lexer& other)
    : tokens(other.tokens),
      source(other.source),
      start(other.start),
      current(other.current),
      line(other.line) ,
      start_line(other.start_line) {}

Lexer& Lexer::operator=(const Lexer& other) {
    if (this != &other) {
        this->tokens = other.tokens;
        this->source = other.source;
        this->start = other.start;
        this->current = other.current;
        this->line = other.line;
        this->start_line = other.start_line;
    }
    return *this;
}


Lexer::Lexer(const std::string&source_code){
    this->source = source_code;
    this->start = 0;
    this->current = 0;
    this->line = 1;
    this->start_line = 0;
}

bool Lexer::isAtEnd(){
    return this->current >= this->source.length();
}

char Lexer::advance(){
    return this->source[this->current++];
}

char Lexer::peek(){
    return this->source[this->current];
}
char Lexer::peekNext(){
    if(current+1 >= this->source.length()) return '\0';
    return this->source[this->current+1];
}
bool Lexer::match(const char expected){
    if(this->isAtEnd()) return false;
    if(this->source[this->current] != expected) return false;
    this->advance();
    return true;
}
void Lexer::skipLine(){
    while(this->peek() != '\n' && !this->isAtEnd()) advance();
}
void Lexer::skipToComment(){
    advance();
    advance();
    std::stack<char> tracker;
    tracker.push('*');
    while (!tracker.empty()) {
        // if we see another '/*', increase nesting
        if (peek() == '/' && peekNext() == '*') {
            tracker.push('*');
            advance(); // consume '/'
            advance(); // consume '*'
            continue;
        }

        // if we see the matching '*/', pop one level
        if (peek() == '*' && peekNext() == '/') {
            tracker.pop();
            advance(); // consume '*'
            advance(); // consume '/'
            continue;
        }

        // count newlines
        if (peek() == '\n') {
            this->line++;
            this->start_line = this->current;
        }

        advance(); // consume any other character
    } 
}
std::string Lexer::getStringLiteral(){
    while(this->peek() != '"' && !this->isAtEnd()) advance();
    std::string extracted = this->source.substr(
        this->start+1,
        this->current-this->start-1
    );
    advance();
    return extracted;
}
std::string Lexer::getIdentifierAndKeyword(){
    while(std::isalpha(this->peek()) && !this->isAtEnd()) advance(); 
    return this->source.substr(
        this->start,
        this->current-this->start
    );
}
double Lexer::getNumberLiteral(){
    while(std::isdigit(this->peek())) advance();
    if(this->peek() == '.' && std::isdigit(this->peekNext()))
        advance();
    while(std::isdigit(this->peek())) advance();
    return std::stod(this->source.substr(start,current));
}
void Lexer::addToken(enum TokenType type,std::any a){
    std::string lexeme = this->source.substr(
        this->start,
        this->current-this->start
    );
    this->tokens.push_back(Token(
        type,lexeme,a,this->line,this->current-this->start_line
    ));
}
void Lexer::addToken(enum TokenType type){
    this->addToken(type,nullptr);
}
void Lexer::scanToken(){
    char c = this->advance();
    switch(c){
        case '(': this->addToken(LEFT_PAREN); break;
        case ')': this->addToken(RIGHT_PAREN); break;
        case '{': this->addToken(LEFT_BRACE); break;
        case '}': this->addToken(RIGHT_BRACE); break;
        case ',': this->addToken(COMMA); break;
        case '.': this->addToken(DOT); break;
        case '-': this->addToken(MINUS); break;
        case '+': this->addToken(PLUS); break;
        case ';': this->addToken(SEMICOLEN); break;
        case ':': this->addToken(COLEN); break;
        case '?': this->addToken(TERNARY); break;
        case '*': this->addToken(STAR); break;
        case '%': this->addToken(MOD); break;
        case '!': 
            addToken(match('=') ? BAND_EQUAL : BANG);
            break;
        case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;           
        case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            if(match('/')){ // we have a comment
                this->skipLine();
            }else if(match('*')){
                this->skipToComment(); 
            }else{
                addToken(SLASH);
            }
            break;
        case ' ' :
        case '\t':
        case '\r':
            break; 
        case '\n':
            ++this->line;
            this->start_line = this->current;
            break;
        case '"':
            this->addToken(STRING,this->getStringLiteral());
            break;
        default:
            if(std::isdigit(c)){
                double val = this->getNumberLiteral();
                this->addToken(
                    NUMBER,val
                );
            }else if(std::isalpha(c)){
                std::string val = this->getIdentifierAndKeyword();
                TokenType type = Keywords[val];
                if(type == TokenType::NONE) type = TokenType::IDENTIFIER;
                this->addToken(type,val);
            }else{
                std::string msg = "unexpected char ";  // note trailing space
                msg += '\'';
                msg += c;
                msg += "\'."; 
                Logger::error(this->line,this->current-this->start_line,msg);
            }
            break;
    }
}


void Lexer::scanSource(){
    while(!this->isAtEnd()){
        this->start = current;
        this->scanToken();
    }
    this->tokens.push_back(
        Token(EOFF,"",nullptr,this->line,this->current-this->start)
    );
}
