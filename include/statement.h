#pragma once
#include "./expr.h"
#include "./lexer.h"

class ExpressionStatement;
class PrintStatement;
class DeclareStatement;
class BlockStatement;

class Statement {
public:
    class Visitor {
    public:
        virtual std::any visitExpressionStatement(ExpressionStatement* estmt);
        virtual std::any visitPrintStatement(PrintStatement* pstmt);
        virtual std::any visitDeclareStatement(DeclareStatement* dstmt);
        virtual std::any visitBlockStatement(BlockStatement* bstmt);
    };
    virtual std::any accept(Visitor& visitor) = 0;
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    Expression* expr;
    ExpressionStatement(Expression* expr){
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
    PrintStatement(Expression* expr){
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
    DeclareStatement(Token name,Expression* expr = nullptr) : name(name) {
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
    BlockStatement(std::vector<Statement*>&stmnts) : stmts(stmnts){}
    virtual std::any accept(Visitor& visitor) override final {
        return visitor.visitBlockStatement(this);
    }
    virtual ~BlockStatement() {
        for(Statement* st:this->stmts){
            delete st;
        } 
    }
};
