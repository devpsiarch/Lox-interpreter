#pragma once
#include "./expr.h"

class ExpressionStatement;
class PrintStatement;


class Statement {
public:
    class Visitor {
    public:
        virtual std::any visitExpressionStatement(ExpressionStatement* estmt);
        virtual std::any visitPrintStatement(PrintStatement* pstmt);
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
