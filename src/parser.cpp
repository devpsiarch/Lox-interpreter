#include "../include/parser.h"
#include "../include/interpreter.h"

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
    return this->assignement();
}

Expression* parser::assignement(){
    std::unique_ptr<Expression> lv(this->conditional());
    if(this->match(TokenType::EQUAL)){
        Token op = this->previous();
        std::unique_ptr<Expression> rv(this->assignement());
        // if the lv value is a variable aka a assinable entity 
        // we continue the assignement
        Variable* down = dynamic_cast<Variable*>(lv.get());
        if(down != nullptr){
            Token name = down->name; 
            return new Assign(name,rv.release());
        }
        // else we cant bind lvalue to lvalue
        std::string err_msg = "SyntaxError: Can't bind lvalue to lvalue , maybe you meant \'==\'?";
        Logger::error(op.line,op.col,err_msg);
    }
    return lv.release();
}

Expression* parser::conditional(){
    // here we either return a condional ds or a equality
    std::unique_ptr<Expression> left(this->equality());
    if(this->match(TokenType::TERNARY)){
        std::unique_ptr<Expression> middle(this->expression());
        std::unique_ptr<Expression> right;
        try{
            this->consume(TokenType::COLEN,"Missing \':\' from ternary expression.");
            right.reset(this->expression());
            // now we evalute
        }catch(const parserError&e){
            std::string err_msg = e.what();
            Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        }

        return new Conditional(this->peek(),left.release()
                               ,middle.release()
                               ,right.release());

    }else{
        return left.release();
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
    if(this->match(TokenType::PLUS)){
        Token e = this->peek();
        std::string err_msg = "Missing left hand side of binary operation.";
        Logger::error(e.line,e.col,err_msg);
        this->synchronize();
        return this->expression();
    }
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
    if(this->match(TokenType::SLASH,TokenType::STAR)){
        Token e = this->peek();
        std::string err_msg = "Missing left hand side of binary operation.";
        Logger::error(e.line,e.col,err_msg);
        this->synchronize();
        return this->expression();
    }
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
    if(this->match(TokenType::IDENTIFIER)) 
        return new Variable(this->previous());
    if(this->match(TokenType::LEFT_PAREN)){
        std::unique_ptr<Expression> expr (this->expression());
        try {
            if(this->check(TokenType::COMMA)){
                while(!this->check(TokenType::RIGHT_PAREN)){
                    this->consume(TokenType::COMMA,"Missing \',\' in comma expression.");
                    // NOTE: we are supposed to evaluate the the expressions in the 
                    // comma expression , and only return the most right one
                    expr.reset(this->expression());
                }
            }
            
            this->consume(TokenType::RIGHT_PAREN,"expcted \')\' before end of expression.");
        }catch(const parserError&e){
            std::string err_msg = e.what();
            Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        }
        return new Grouping(expr.release());
    }
    throw parserError(this->peek(),"expected expression.");
}

Statement* parser::statement(){
    if(this->match(TokenType::PRINT)) return print_statement();
    if(this->match(TokenType::LEFT_BRACE)){
        std::vector<Statement*> result = block_statement();
        return new BlockStatement(result);
    }
    return expression_statement();
}

Statement* parser::declaration(){
    try {
        if(this->match(TokenType::VAR)) return declare_statement();
        return statement();
    }catch(const parserError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        this->synchronize();
        return nullptr;
    }

}

Statement* parser::print_statement(){
    std::unique_ptr<Expression> expr(this->expression());
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after print statement.");
    return new PrintStatement(expr.release(),this->peek());
}
Statement* parser::expression_statement(){
    std::unique_ptr<Expression>expr(this->expression());
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after value of expression.");
    return new ExpressionStatement(expr.release());
}

Statement* parser::declare_statement(){
    Token name = consume(TokenType::IDENTIFIER,"Expected variable name after declaration.");
    std::unique_ptr<Expression> value;
    if(this->match(TokenType::EQUAL)){
        value.reset(this->expression()); 
    }
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after variable declaration.");
    return new DeclareStatement(name,value.release());
}

std::vector<Statement*> parser::block_statement(){
    std::vector<Statement*> result;
    while(!this->isAtEnd() && !this->check(TokenType::RIGHT_BRACE)){
        result.push_back(this->declaration());
    }
    this->consume(TokenType::RIGHT_BRACE,"Expected \'}\' at end of a block.");
    return result;
}


void parser::synchronize(void){
    while(!this->isAtEnd()){
        this->advance();
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

std::vector<Statement*> parser::parserProgram(){
    std::vector<Statement*> stmts;
    try {
        while(!this->isAtEnd()){
            stmts.push_back(this->declaration());
        }
    }catch(const parserError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
    }
    // returns empty if failed , maybe ill change this to return a pointer to a vector instead
    return stmts;
}
