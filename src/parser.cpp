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

void parser::rewind(){
    if(this->current != 0) --this->current;
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
    std::unique_ptr<Expression> lv(this->logic_or());
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

Expression* parser::logic_or(){
    std::unique_ptr<Expression> left(this->logic_and());
    while(this->match(TokenType::OR)){
        Token op = this->previous();
        std::unique_ptr<Expression> right(this->logic_and());
        left.reset(new Logical(op
                               ,left.release()
                               ,right.release()));
    }
    return left.release();
}
Expression* parser::logic_and(){
    std::unique_ptr<Expression> left(this->equality());
    while(this->match(TokenType::AND)){
        Token op = this->previous();
        std::unique_ptr<Expression> right(this->equality());
        left.reset(new Logical(op,left.release(),right.release()));
    }
    return left.release();
}

Expression* parser::a_fun(){
    // taken from the function parsing , lol am getting sloppy
    std::vector<Token> params;
    // we get the parameters
    this->consume(TokenType::LEFT_PAREN,"Expected \'(\' after the function name");
    if(!this->check(TokenType::RIGHT_PAREN)){
        do {
            if(params.size() >= 255){
                std::string err_msg = 
                    "Exceeded max number of arguments <255>.";
                Logger::error(this->previous().line,
                              this->previous().col,err_msg);
                break;
            }
            params.push_back(this->consume(TokenType::IDENTIFIER,
                "Expected parameter in function definition.")); 
        } while(this->match(TokenType::COMMA));
    }
    this->consume(TokenType::RIGHT_PAREN,
            "Expected \')\' after function parameters.");
    this->consume(TokenType::LEFT_BRACE,
            "Expected \'{\' before function block.");   
    std::vector<Statement*> result = this->block_statement();
    return new AFun(this->previous(),params,result);        
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
    if(this->match(TokenType::SLASH,TokenType::STAR,TokenType::MOD)){
        Token e = this->peek();
        std::string err_msg = "Missing left hand side of binary operation.";
        Logger::error(e.line,e.col,err_msg);
        this->synchronize();
        return this->expression();
    }
    std::unique_ptr<Expression>left(this->unary());
    while(this->match(
        TokenType::STAR,
        TokenType::SLASH,
        TokenType::MOD
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
        return this->call();
    }
}

Expression* parser::call(){
    std::unique_ptr<Expression> pri(this->primary());
    std::unique_ptr<Expression> calling;
    while(true){
        if(this->match(TokenType::LEFT_PAREN)){
            calling.reset(this->get_call(pri));
        }else{
            break;
        }
    }
    if(calling){
        return calling.release();
    }else{
        return pri.release();
    }
}

Expression* parser::get_call(std::unique_ptr<Expression>&caller){
    std::vector<Expression*> args;
    if(!this->check(TokenType::RIGHT_PAREN)){
        do {
            // we impose a max number of arguments passed to a function let it be 255 
            if(args.size() >= 255){
                std::string err_msg = "Exceeded max number of arguments <255>.";
                Logger::error(this->previous().line,this->previous().col,err_msg);
                break;
            }
            try{
                args.push_back(this->expression()); 
            }catch(...){
                for(Expression* expr:args){
                    delete expr;
                    expr = nullptr;
                }
                throw;
            }
        } while(this->match(TokenType::COMMA));
    }
    this->consume(TokenType::RIGHT_PAREN,"Expected \')\' after function call.");
    return new Call(this->previous(),args,caller.release());
}

Expression* parser::primary(){
    if(this->match(TokenType::FUN)) return a_fun();
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
                std::vector<Expression*> list;
                list.push_back(expr.release());
                while(!this->check(TokenType::RIGHT_PAREN)){
                    this->consume(TokenType::COMMA,"Missing \',\' in comma expression.");
                    list.push_back(this->expression());
                }
                this->consume(TokenType::RIGHT_PAREN,"expcted \')\' before end of expression.");
                return new Comma(list);
            }else{
                this->consume(TokenType::RIGHT_PAREN,"expcted \')\' before end of expression.");
                return new Grouping(expr.release());
            }
            
        }catch(const parserError&e){
            std::string err_msg = e.what();
            Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        }
    }
    throw parserError(this->peek(),"expected expression.");
}

Statement* parser::statement(){
    if(this->match(TokenType::RETURN)) return this->return_statement();
    if(this->match(TokenType::FUN)) return this->function_statement();
    if(this->match(TokenType::BREAK)) return this->break_statement();
    if(this->match(TokenType::CONTINUE)) return this->continue_statement();
    if(this->match(TokenType::FOR)) return this->for_statement();
    if(this->match(TokenType::WHILE)) return this->while_statement();
    if(this->match(TokenType::IF)) return this->if_statement();
    if(this->match(TokenType::PRINT)) return this->print_statement();
    if(this->match(TokenType::LEFT_BRACE)){
        std::vector<Statement*> result = this->block_statement();
        return new BlockStatement(result);
    }
    return expression_statement();
}

Statement* parser::declaration(){
    Statement* st;
    try {
        if(this->match(TokenType::VAR)) st = declare_statement();
        else st = statement();
    }catch(const parserError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
        this->synchronize();
        return nullptr;
    }
    return st;
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
    // here we check the possibility of the value expression being a 
    // anonymous function
    AFun* anony = dynamic_cast<AFun*>(value.get());
    // if it isnt , we return a normal DeclareStatement
    if(anony == nullptr){
        return new DeclareStatement(name,value.release());
    }else{
        // else , we consider it as a function declaration
        // we tell the AFun API that this anonymous function has found its bind 
        // therefore , when you clean yourself , dont kill the statements because the 
        // FunStatement will own them later
        anony->setbind();
        // get the guts of the anonymous function
        std::vector<Token> params = anony->params;
        std::vector<Statement*> body = anony->body;
        // give them to the bounded function
        return new FunStatement(name,params,body);
    }
}

Statement* parser::if_statement(){
    this->consume(TokenType::LEFT_PAREN,"Expected a \'(\' after if statement.");
    std::unique_ptr<Expression> expr(this->expression());
    this->consume(TokenType::RIGHT_PAREN,"Expected a \')\' after if statement.");
    std::unique_ptr<Statement> thenstmt(this->statement());
    std::unique_ptr<Statement> elsestmt(nullptr);
    if(this->match(TokenType::ELSE)){
        elsestmt.reset(this->statement()); 
    }
    return new IfStatement(expr.release()
                           ,thenstmt.release()
                           ,elsestmt.release());
}

Statement* parser::while_statement(){
    this->consume(TokenType::LEFT_PAREN,"Expected a \'(\' after while statement.");
    std::unique_ptr<Expression> cond(this->expression());
    this->consume(TokenType::RIGHT_PAREN,"Expected a \')\' after while statement.");
    std::unique_ptr<Statement> body(this->statement());
    return new WhileStatement(cond.release()
                              ,body.release());
}

Statement* parser::for_statement(){
    this->consume(TokenType::LEFT_PAREN,"Expected a \'(\' after a for statement.");
    
    std::unique_ptr<Statement> init;
    if(this->match(SEMICOLEN)){
        init.reset(nullptr);
    }
    else if(this->match(TokenType::VAR)){
        init.reset(this->declare_statement());
    }else{
        init.reset(this->expression_statement());
    }
    
    std::unique_ptr<Expression> condition;
    if(!this->check(TokenType::SEMICOLEN)){
        condition.reset(this->expression());
    }
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after for loop condition.");
    
    std::unique_ptr<Expression> increm;
    if(!this->check(TokenType::RIGHT_PAREN)){
        increm.reset(this->expression());
    }
    this->consume(TokenType::RIGHT_PAREN,"Expected \')\' after for loop condition.");

    std::unique_ptr<Statement> body(this->statement());

    return new ForStatement(init.release()
                            ,increm.release()
                            ,condition.release()
                            ,body.release());

    // here we are basicly building a while loop using the 
    // infomation we parsed from a for loop. look up desugaring
    
    // i would have kept the desugaring methode if it werent foe the 
    // fact that it disallows the continue statement. 

    // if we have and inc then the body is just a block 
    // if(increm != nullptr){
    //     std::vector<Statement*> stmts = {body.release(),new ExpressionStatement(increm.release())};
    //     body.reset(new BlockStatement(stmts));
    // }
    //
    // // the condition we parserd makes it a while using that condition
    // if(condition == nullptr) condition.reset(new Literal(true));
    // body.reset(new WhileStatement(condition.release(),body.release()));
    //
    // // if we have a init then we execute it before the while loop
    // if(init != nullptr){
    //     std::vector<Statement*> stmts = {init.release(),body.release()};
    //     body.reset(new BlockStatement(stmts));
    // }
    //
    // return body.release();
}

Statement* parser::break_statement(){
    Token op = this->previous();
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after break.");
    return new BreakStatement(op);
}
Statement* parser::continue_statement(){
    Token op = this->previous();
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after continue.");
    return new ContinueStatement(op);
}

std::vector<Statement*> parser::block_statement(){
    std::vector<Statement*> result;
    while(!this->isAtEnd() && !this->check(TokenType::RIGHT_BRACE)){
        std::unique_ptr<Statement> st(this->declaration());
        // we use to stop nested functions here , if we wanna mess around.
        result.push_back(st.release());
    }
    try {
        this->consume(TokenType::RIGHT_BRACE,"Expected \'}\' at end of a block.");
    }catch(...){
        for(Statement* st:result) {
            delete st;
            st = nullptr;
        }
        throw;
    }
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

Statement* parser::function_statement(){
    // parse the identifer that the function call binds to
    if(!this->check(TokenType::IDENTIFIER)){
        // we rewind because the in the primary parsing function
        // while parsing the anonymous function , it assumes the "fun" token 
        // has not been consumed yet
        this->rewind();
        return expression_statement();
    }
    Token funName = this->consume(TokenType::IDENTIFIER,
                "Expected function name after declaration.");
    this->consume(TokenType::LEFT_PAREN,
                "Expected \'(\' after the function name");
    std::vector<Token> params;
    // we get the parameters
    if(!this->check(TokenType::RIGHT_PAREN)){
        do {
            if(params.size() >= 255){
                std::string err_msg = 
                    "Exceeded max number of arguments <255>.";
                Logger::error(this->previous().line,
                              this->previous().col,err_msg);
                break;
            }
            params.push_back(this->consume(TokenType::IDENTIFIER,
                "Expected parameter in function call.")); 
        } while(this->match(TokenType::COMMA));
    }
    this->consume(TokenType::RIGHT_PAREN,
            "Expected \')\' after function parameters.");
     this->consume(TokenType::LEFT_BRACE,
            "Expected \'{\' before function block.");   
    std::vector<Statement*> result = this->block_statement();
    return new FunStatement(funName,params,result); 
}

Statement* parser::return_statement(){
    std::unique_ptr<Expression> expr;
    if(!this->check(TokenType::SEMICOLEN)){
        expr.reset(this->expression());
    }
    this->consume(TokenType::SEMICOLEN,"Expected \';\' after a return statement.");
    return new ReturnStatement(this->peek(),expr.release());
}

// NOTE: we dont use this anymore
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
            Statement* next = this->declaration();
            stmts.push_back(next);
        }
    }catch(const parserError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_token.line,e.faulty_token.col,err_msg);
    }
    // returns empty if failed , maybe ill change this to return a pointer to a vector instead
    return stmts;
}
