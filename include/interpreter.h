#pragma once
#include "./expr.h"
#include "./statement.h"
#include "./environment.h"
#include "./callable.h"
#include "./function.h"

class environment;

class Interpreter : public Expression::Visitor , public Statement::Visitor {
private:

    environment* env;
    bool REPL;

    virtual std::any visitLiteralExpression(Literal*lit) override;
    virtual std::any visitUnaryExpression(Unary*una) override;
    virtual std::any visitCallExpression(Call* callme) override;
    virtual std::any visitBinaryExpression(Binary*bin) override;
    virtual std::any visitGroupingExpression(Grouping*gro) override;
    virtual std::any visitCommaExpression(Comma*com) override;
    virtual std::any visitVariableExpression(Variable*var) override;
    virtual std::any visitAssignExpression(Assign*ass) override;
    virtual std::any visitConditionalExpression(Conditional*con) override;
    virtual std::any visitLogicalExpression(Logical* lor) override;

    virtual std::any visitExpressionStatement(ExpressionStatement* estmt) override;
    virtual std::any visitPrintStatement(PrintStatement* pstmt) override;
    virtual std::any visitDeclareStatement(DeclareStatement* dstmt) override;
    virtual std::any visitBlockStatement(BlockStatement* bstmt) override;
    virtual std::any visitIfStatement(IfStatement* ifstmt) override;
    virtual std::any visitWhileStatement(WhileStatement* wstmt) override;
    virtual std::any visitForStatement(ForStatement* fstmt) override;
    virtual std::any visitBreakStatement(BreakStatement* brstmt) override;
    virtual std::any visitContinueStatement(ContinueStatement* cstmt) override;
    virtual std::any visitFunStatement(FunStatement* funstmt) override;
    virtual std::any visitReturnStatement(ReturnStatement* retstmt) override;
    
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
    
    // the name might be missleading but this class accualy refers to 
    // the "break" and "continue" statements.
    // am too lazy to find a better name fore them then this but hey
    // just know these are expections that we throw then catch (if possible)
    // to excape from an arbirarly nested statements or else crash since only 
    // applicable in loops
    public: class ControlFlow : std::exception {
    public:
        Token op;
        ControlFlow(Token op) : op(op){}
        ~ControlFlow() = default;
        const char* what() const noexcept override{
            return "ControlFlow statement expection";
        }
    };
    
    // this does a similar job as the above , it allows us to return a value 
    // from an arbitrarly nested statements inside a function 'only'
    public: class ReturnFlow : public std::exception {
    public:   
        std::any ret_value;
        ReturnFlow(std::any value) : ret_value(value){}
        ~ReturnFlow() = default;
        const char* what() const noexcept override{
            return "ReturnFlow statement expection";
        }
    };
private:
    static bool isTruthy(std::any obj);
    std::any evaluate(Expression* expr);
    void execute(Statement* st);

public:
    void executeBlock(std::vector<Statement*>&stmts,environment* venv);   
    void Interpret(Expression* expr);
    void InterpretProgram(std::vector<Statement*>& stmt);

    Interpreter(bool repl = false);
    ~Interpreter();
};

bool operator==(std::any obj1,std::any obj2);
