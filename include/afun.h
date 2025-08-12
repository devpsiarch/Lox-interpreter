#pragma once
#include "expr.h"
#include "statement.h"

class AFun : public Expression {
public:
    Token paren;
    std::vector<Token> params;
    std::vector<Statement*> body;
    bool binded;
    explicit AFun(Token op,std::vector<Token> p,std::vector<Statement*> stmts)  
        : paren(op) , params(p) , body(stmts){
        this->binded = false;
    }
    void setbind(){
        this->binded = true;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitAFunExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~AFun() final {
        if(this->binded == false){
            for(Statement*st:this->body){
                delete st;
                st = nullptr;
            }
        }
    }
};
