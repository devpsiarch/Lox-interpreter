#include "../include/parser.h"

parser::parser(const std::vector<Token>&toks){
    this->current = 0;
    this->tokens = toks;
}
parser::parser(const parser&p){
    this->current = p.current;
    this->tokens = p.tokens;
}
parser& parser::operator=(const parser&p){
    if(this != &p){
        this->current = p.current;
        this->tokens = p.tokens;       
    }
    return *this;
}
bool parser::isAtEnd(){
    return this->tokens[this->current].type == TokenType::EOFF;
}
Token parser::peek(){
    return this->tokens[this->current];
}
bool parser::check(TokenType type){
    if(isAtEnd()) return false;
    return this->peek().type == type;
}
Token parser::previous(){
    return this->tokens[this->current-1];
}
Token parser::advance(){
    if(!isAtEnd()) this->current++;
    return this->previous();
}
template <typename ...Token> bool parser::match(Token... tokens){
    bool did_advance = false;
    bool found = ( ( check(tokens) 
                    ? ( advance(), did_advance = true, true )
                    : false )
                || ... );
    return found;
}

Token parser::consume(TokenType type,const char*msg){
    if(check(type)) return advance();
    throw parserError(this->peek(),msg);
}

/* when you dont know where the memory leaks are comming from 
 * wrap every thing in a unique_ptr and call it a day      
 * the leaks happend when the call stack was unwinded to handle 
 * the exception.*/

Expression* parser::expression(){
    if(this->match(TokenType::LEFT_PAREN)){
        std::unique_ptr<Expression> left(this->expression());
        if(this->match(TokenType::RIGHT_PAREN)){
            if(this->match(TokenType::TERNARY)){
                // NOTE: this piece of code is handling the ?: turnary operator.
                // evaluatation for this operator should be done here to determine 
                // what expression to return , either middle or right 
                // based on the value of the left (most likely).
                std::unique_ptr<Expression> middle(this->expression());
                try {
                    this->consume(TokenType::COLEN,"Missing \':\' for turnary operator.");
                    std::unique_ptr<Expression> right(this->expression());
                    this->consume(TokenType::SEMICOLEN,"Missing \';\' for turnary operator.");
                    // for now we always return the middle expression
                    return middle.release();
                }
                catch(const parserError&e) {
                    std::string err_msg = e.what();
                    Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
                }
            }
        }
        while(!this->match(TokenType::RIGHT_PAREN)){
            try {
                // NOTE: this piece of code is handling the comma expression 
                // that is supposed to evaluate all the expressions seperated by a comma 
                // and only return the last one (done only returning).
                this->consume(TokenType::COMMA,"Missing \',\' in comma expression."); 
                left.reset(this->expression());
            }catch(const parserError&e){
                std::string err_msg = e.what();
                Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
            }
        }
        return left.release();
        
    }else{
        return this->equality();
    }
}
Expression* parser::equality(){
    std::unique_ptr<Expression> left(this->comparison());
    while(this->match(TokenType::BAND_EQUAL,TokenType::EQUAL_EQUAL)){
        Token op = this->previous();
        std::unique_ptr<Expression> right(this->comparison());
        left.reset(new Binary(op,left.release(),right.release()));
    }
    return left.release();
}
Expression* parser::comparison(){
    std::unique_ptr<Expression> left(this->term());
    while(this->match(
        TokenType::GREATER,
        TokenType::GREATER_EQUAL,
        TokenType::LESS,
        TokenType::LESS_EQUAL
    )){
        Token op = this->previous();
        std::unique_ptr<Expression> right(term());
        left.reset(new Binary(op,left.release(),right.release()));
    }
    return left.release();
}
Expression* parser::term(){
    std::unique_ptr<Expression> left(this->factor());
    while(this->match(
        TokenType::MINUS,
        TokenType::PLUS
    )){
        Token op = this->previous();
        std::unique_ptr<Expression> right (this->factor());
        left.reset(new Binary(op,left.release(),right.release()));
    }
    return left.release();
}
Expression* parser::factor(){
    std::unique_ptr<Expression>left(this->unary());
    while(this->match(
        TokenType::STAR,
        TokenType::SLASH
    )){
        Token op = this->previous();
        std::unique_ptr<Expression> right(unary());
        left.reset(new Binary(op,left.release(),right.release()));
    }
    return left.release();
}
Expression* parser::unary(){
    if(this->match(
        TokenType::BANG,
        TokenType::MINUS
    )){
        Token op = this->previous();
        return new Unary(op,this->unary());
    }else{
        return this->primary();
    }
}
Expression* parser::primary(){
    if(this->match(TokenType::FALSE)) return new Literal(false);
    if(this->match(TokenType::TRUE)) return new Literal(true);
    if(this->match(TokenType::NIL)) return new Literal(nullptr);
    if(this->match(TokenType::STRING,TokenType::NUMBER))
        return new Literal(previous().literal);
    if(this->match(TokenType::LEFT_PAREN)){
        std::unique_ptr<Expression> expr (this->expression());
        try {
            this->consume(TokenType::RIGHT_PAREN,"expcted \')\' before end of expression.");
        }catch(const parserError&e){
            std::string err_msg = e.what();
            Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        }
        return new Grouping(expr.release());
    }
    throw parserError(this->peek(),"expected expression.");
}

void parser::synchronize(void){
    advance();
    while(!this->isAtEnd()){
        if(previous().type == TokenType::SEMICOLEN) return;
        switch (peek().type) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
            default:
                continue;
        }
        advance();
    }
}

Expression* parser::parse(){
    try {
        return expression();
    }catch(const parserError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        return nullptr;
    }
}
