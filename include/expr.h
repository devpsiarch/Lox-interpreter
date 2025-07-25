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
class Variable;
class Grouping;
class Comma;
class Assign;
class Conditional;
class Logical;
class Call;

class Expression {
public:
    // Generic visitor interface

    class Visitor {
    public:
        virtual std::any visitLiteralExpression(Literal*lit);
        virtual std::any visitUnaryExpression(Unary*una);
        virtual std::any visitBinaryExpression(Binary*bin);
        virtual std::any visitGroupingExpression(Grouping*gro);
        virtual std::any visitCommaExpression(Comma*com);
        virtual std::any visitVariableExpression(Variable*var);
        virtual std::any visitAssignExpression(Assign*ass);
        virtual std::any visitConditionalExpression(Conditional*con);
        virtual std::any visitLogicalExpression(Logical* lor);
        virtual std::any visitCallExpression(Call* callme);

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
   
    // this is a challenge class , it will parse a expression to RPN
    class VisitorRPN {
    public:
        std::any visitLiteralExpression(Literal*lit);
        std::any visitUnaryExpression(Unary*una);
        std::any visitBinaryExpression(Binary*bin);
        std::any visitGroupingExpression(Grouping*gro);

        std::string astprinter(Expression*exp) {
            return stdany_to_string(exp->acceptRPN(*this));
        }
        // its returns a funky string , having more spaces but it wont bother our evaluator 
        // "  1 2+" instead of "1 2 +" am too lazy to fix it , maybe later
        private:template <typename ...Exprs> std::string parenthensize(std::string name,Exprs*... exprs){
            std::ostringstream builder;
            ((builder << " " << stdany_to_string(exprs->acceptRPN(*this))), ...); 
            if(name != "group") 
                builder << ' ' <<  name;
            return builder.str();
        }
        public:static double evaluator(std::string&input){
            // this code if form a exam in my college 
            // never thought id ever use this shit.
            std::stack<double> operands;
            for(char c:input){
                if(isdigit(c)){
                    operands.push(c-'0');
                }
                else if (c == ' ') continue;
                else{
                    if(operands.size() < 2) return -1;
                    int op1 = operands.top();
                    operands.pop();
                    int op2 = operands.top();
                    operands.pop();
                    switch (c) {
                        case '+':
                            operands.push(op2+op1);
                            break;
                        case '-':
                            operands.push(op2-op1);
                            break;
                        case '/':
                            operands.push(op2/op1);
                            break;
                        case '*':
                            operands.push(op2*op1);
                            break;
                        default:
                            break;
                    }
                }
            }
            return operands.top();
        }
    };
    

    // here we choose to use the first visitor class
    virtual std::any accept(Visitor& visitor) = 0;
    virtual std::any acceptRPN(VisitorRPN&visitor) = 0;

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
    virtual std::any acceptRPN(VisitorRPN&visitor){
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
    virtual std::any acceptRPN(VisitorRPN&visitor){
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
    explicit Binary(Token op,Expression*l,Expression*r) : op(op){
        this->left = l;
        this->right = r;
    }
    virtual std::any accept(Visitor&visitor) override final {
        return visitor.visitBinaryExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
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
    virtual std::any acceptRPN(VisitorRPN&visitor){
        return visitor.visitGroupingExpression(this);
    }
    virtual ~Grouping() final {
        delete expression;
    }
};

class Comma : public Expression {
public:
    std::vector<Expression*> list;
    explicit Comma(std::vector<Expression*>&other) : list(other){
        for(size_t i = 0 ; i < (size_t)other.size() ; ++i){
            // we take responsibility for them from now on
            other[i] = nullptr;
        } 
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitCommaExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Comma() final {
        for(Expression*expr:list) delete expr; 
    }
};

class Variable : public Expression {
public:
    Token name;
    explicit Variable(Token name) : name(name){}
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitVariableExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Variable() final  = default;
};

class Assign : public Expression {
public:
    Token op;
    Expression* expr;
    explicit Assign(Token op,Expression* expr) : op(op) {
        this->expr = expr;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitAssignExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Assign() final {
        delete expr;
    }
};

class Conditional : public Expression {
public:
    Expression* left;
    Expression* right;
    Expression* middle;
    Token op;
    explicit Conditional(Token op,Expression* l,Expression* m,Expression* r) : op(op) {
        this->left    = l;
        this->right   = r;
        this->middle  = m;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitConditionalExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Conditional() final {
        delete middle;
        delete right;
        delete left;
    }
};

class Logical : public Expression {
public:
    Expression* left;
    Expression* right;
    Token op;
    explicit Logical(Token op,Expression*l,Expression*r) : op(op) {
        this->left = l;
        this->right = r;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitLogicalExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Logical() final {
        delete right;
        delete left;
    }
};

class Call : public Expression {
public:
    Expression* callee;
    Token paren;
    std::vector<Expression*> args;
    explicit Call(Token op,std::vector<Expression*>&a,Expression* callee) : paren(op) , args(a){
        this->callee = callee;
    }
    virtual std::any accept(Visitor&visitor) override final{
        return visitor.visitCallExpression(this);
    }
    virtual std::any acceptRPN(VisitorRPN&visitor){
        (void)visitor;
        return nullptr;
    }
    virtual ~Call() final {
        delete callee;
        for(Expression* expr:args) delete expr;
    }   
};
