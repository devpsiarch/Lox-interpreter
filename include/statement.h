#pragma once
#include "./expr.h"
#include "./lexer.h"

class ExpressionStatement;
class PrintStatement;
class DeclareStatement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class ForStatement;
class BreakStatement;
class ContinueStatement;
class FunStatement;
class ReturnStatement;

class Statement {
public:
    class Visitor {
    public:
        virtual std::any visitExpressionStatement(ExpressionStatement* estmt);
        virtual std::any visitPrintStatement(PrintStatement* pstmt);
        virtual std::any visitDeclareStatement(DeclareStatement* dstmt);
        virtual std::any visitBlockStatement(BlockStatement* bstmt);
        virtual std::any visitIfStatement(IfStatement* ifstmt);
        virtual std::any visitWhileStatement(WhileStatement* wstmt);
        virtual std::any visitForStatement(ForStatement* fstmt);
        virtual std::any visitBreakStatement(BreakStatement* brstmt);
        virtual std::any visitContinueStatement(ContinueStatement* cstmt);
        virtual std::any visitFunStatement(FunStatement* funstmt);
        virtual std::any visitReturnStatement(ReturnStatement* retstmt);

    };
    virtual std::any accept(Visitor& visitor) = 0;
    virtual ~Statement() = default;
    virtual void debug() {}
};

class ExpressionStatement : public Statement {
public:
    Expression* expr;
    explicit ExpressionStatement(Expression* expr){
        this->expr = expr;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitExpressionStatement(this);
    }
    virtual ~ExpressionStatement() {
        delete this->expr;
    }
};


class PrintStatement : public Statement {
public:
    Expression* expr;
    Token op;
    explicit PrintStatement(Expression* expr,Token op) : op(op){
        this->expr = expr;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitPrintStatement(this);
    }
    virtual ~PrintStatement() {
        delete this->expr;
    }
};

class DeclareStatement : public Statement {
public:
    Token name;
    Expression* init; // optional else nullpltr
    explicit DeclareStatement(Token name,Expression* expr = nullptr) : name(name) {
        this->init = expr;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitDeclareStatement(this);
    }
    virtual ~DeclareStatement() {
        delete this->init;
    }
};

class BlockStatement : public Statement {
public:
    std::vector<Statement*> stmts;
    explicit BlockStatement(std::vector<Statement*>&stmnts) : stmts(stmnts){}
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitBlockStatement(this);
    }
    virtual ~BlockStatement() {
        for(Statement* st:this->stmts){
            delete st;
            st = nullptr;
        } 
    }
};

class IfStatement : public Statement {
public:
    Expression* Texpr;
    Statement* thenStmt;
    Statement* elseStmt;
    explicit IfStatement(Expression* expr,Statement* ts,Statement* es = nullptr){
        this->Texpr = expr;
        this->thenStmt = ts;
        this->elseStmt = es;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitIfStatement(this);
    }
    virtual ~IfStatement() {
        delete this->Texpr;
        delete this->thenStmt;
        delete this->elseStmt;
    }
};

class WhileStatement : public Statement {
public:
    Expression* Texpr;
    Statement* stmt;
    explicit WhileStatement(Expression* expr,Statement* stmt){
        this->Texpr = expr;
        this->stmt = stmt;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitWhileStatement(this);
    }
    virtual ~WhileStatement() {
        delete this->Texpr;
        delete this->stmt;
    }
};

class ForStatement : public Statement {
public:
    Statement* init;
    Expression* incr;
    Expression* cond;
    Statement* stmts;
    explicit ForStatement(Statement* init,Expression*incr,
                          Expression* expr,Statement* stmts) {
        this->init = init;
        this->incr = incr;
        this->cond = expr;
        this->stmts = stmts;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitForStatement(this);
    }
    virtual ~ForStatement() {
        delete this->init;
        delete this->incr;
        delete this->cond;
        delete this->stmts;
    }
};

class BreakStatement : public Statement {
public:
    Token op;
    explicit BreakStatement(Token op) : op(op){}
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitBreakStatement(this);
    }   
    virtual ~BreakStatement() = default;
};

class ContinueStatement : public Statement {
public:
    Token op;
    explicit ContinueStatement(Token op) : op(op){}
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitContinueStatement(this);
    }   
    virtual ~ContinueStatement() = default;
};

class FunStatement : public Statement {
public:
    Token name;
    std::vector<Token> params;
    std::vector<Statement*> body;
    explicit FunStatement(Token op,std::vector<Token>&pars,std::vector<Statement*>&body) : name(op) 
        , params(pars) , body(body){}
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitFunStatement(this);
    }   
    virtual ~FunStatement(){
        for(Statement*st:this->body){
            delete st;
            st = nullptr;
        }
    }
};

class ReturnStatement : public Statement {
public:
    Expression* retvalue;
    Token op;
    explicit ReturnStatement(Token op,Expression* expr = nullptr) : op(op){
        this->retvalue = expr;
    }
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitReturnStatement(this);
    }   
    virtual ~ReturnStatement(){
        delete this->retvalue;
    }
};
