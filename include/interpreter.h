#pragma once
#include "./expr.h"

class Interpreter : public Expression::Visitor {
private:
    virtual std::any visitLiteralExpression(Literal*lit) override;
    virtual std::any visitUnaryExpression(Unary*una) override;
    virtual std::any visitBinaryExpression(Binary*bin) override;
    virtual std::any visitGroupingExpression(Grouping*gro) override;

    bool isEqual(std::any obj1,std::any obj2); 
    
    bool checkType(const std::type_info&expected,std::any obj1,std::any obj2);

    void assertTypeBinary(const std::type_info&expected,std::any obj1,std::any obj2,Token op);
    void assertTypeUnary(const std::type_info&expected,std::any obj,Token op);
        


    class RunTimeError : public std::exception {
    public:
        std::string msg;
        Token faulty_op;
        RunTimeError() = delete;
        RunTimeError(Token t,const char*reason) : faulty_op(t) {
            this->msg = reason;
        }
        const char* what() const noexcept override{
            return msg.c_str();
        }
    };

public:
    void Interpret(Expression* expr);
    static bool isTruthy(std::any obj);
    std::any evaluate(Expression* expr);
};

bool operator==(std::any obj1,std::any obj2);
