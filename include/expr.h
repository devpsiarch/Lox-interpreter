#pragma once
// abstract class that models a expression which can be : 
// expression  → Literal
//             | Unary
//             | Binary
//             | Grouping ;
#include "lexer.h"
// the book here follows a metaprogramming aproache to define all of these classes 
// although it is pretty easy to define them by hand , it still a good exercise to 
// do that metaprogramming task.
class Literal;
class Unary;
class Binary;
class Grouping;

class Expression {
public:
    // Generic visitor interface

    class Visitor {
    public:
        std::any visitLiteralExpression(Literal*lit);
        std::any visitUnaryExpression(Unary*una);
        std::any visitBinaryExpression(Binary*bin);
        std::any visitGroupingExpression(Grouping*gro);

        std::string astprinter(Expression*exp) {
            return stdany_to_string(exp->accept(*this));
        }
                
        private:template <typename ...Exprs> std::string parenthensize(std::string name,Exprs*... exprs){
            std::ostringstream builder;
            builder << "(" << name ;
            ((builder << " " << stdany_to_string(exprs->accept(*this))), ...); 
            builder << ")";
            return builder.str();
        }
    };
    
    virtual std::any accept(Visitor& visitor) = 0;

    virtual ~Expression() = default;
};

class Literal : public Expression {
public:
    std::any value;
    explicit Literal(std::any val){
        this->value = val;
    }
    virtual std::any accept(Visitor& visitor) override  final {
        return visitor.visitLiteralExpression(this);
    }
    virtual ~Literal() = default;
};

class Unary : public Expression {
public:
    Token op;
    Expression* right;
    explicit Unary(Token op,Expression*exp) : op(op) {
        this->right = exp;
    }
    virtual std::any accept(Visitor& visitor) override final{
        return visitor.visitUnaryExpression(this);
    }
    virtual ~Unary() final {
        delete right;
    }
};

class Binary : public Expression {
public:
    Expression* left;
    Expression* right;
    Token op;
    explicit Binary(Token op,Expression*r,Expression*l) : op(op){
        this->left = l;
        this->right = r;
    }
    virtual std::any accept(Visitor&visitor) override final {
        return visitor.visitBinaryExpression(this);
    }
    virtual ~Binary() final {
        delete left;
        delete right;
    }
};

class Grouping : public Expression {
public:
    Expression* expression;
    explicit Grouping(Expression*exp){
        this->expression = exp;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitGroupingExpression(this);
    }
    virtual ~Grouping() final {
        delete expression;
    }
};
