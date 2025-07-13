#include "../include/interpreter.h"
/* stopped at handling the checking for types when evaluating*/


bool Interpreter::checkType(const std::type_info&expected,std::any obj1,std::any obj2){
    const std::type_info& l = obj1.type();
    const std::type_info& r = obj2.type();
    if(l != expected || r != expected) return false;
    return true;
}

void Interpreter::assertTypeBinary(const std::type_info&expected,std::any obj1,std::any obj2,Token op){
    if(!this->checkType(expected,obj1,obj2)){
        std::string err_msg = "TypeError: operation ";
        err_msg += op.lexeme;
        err_msg += " not supported for types ";
        err_msg += type_to_string(obj1.type());
        err_msg += " ";
        err_msg += type_to_string(obj2.type());
        err_msg += ".";
        throw RunTimeError(op,err_msg.c_str());
    }
}
void Interpreter::assertTypeUnary(const std::type_info&expected,std::any obj,Token op){
    if(obj.type() != expected){
        std::string err_msg = "TypeError: operation ";
        err_msg += op.lexeme;
        err_msg += " not supported for type ";
        err_msg += type_to_string(obj.type());
        err_msg += ".";
        throw RunTimeError(op,err_msg.c_str());
    }
}

bool operator==(std::any obj1,std::any obj2){
    const std::type_info& l = obj1.type();
    const std::type_info& r = obj2.type();
    if(l != r) return false;
    // we only suppose these types so i wont mind this code for now
    else if (r == typeid(double)) {
        return std::any_cast<double>(obj1) == std::any_cast<double>(obj2);
    }
    else if (r == typeid(bool)) {
        return std::any_cast<bool>(obj1) == std::any_cast<bool>(obj2);
    }
    else if (r == typeid(std::string)) {
        return std::any_cast<std::string>(obj1) == std::any_cast<std::string>(obj2);
    }
    return false;
}



bool Interpreter::isTruthy(std::any obj){
    // this is my considiration for an object to be truthy 
    // we can add/remove more cases as we wish ... dont make it 
    // like JS please.
    const std::type_info& tobj = obj.type();
    if(tobj == typeid(std::nullptr_t) || !obj.has_value()){
        return false;
    }
    if(tobj == typeid(bool)){
        return std::any_cast<bool>(obj);
    }
    if(tobj == typeid(double)){
        return std::any_cast<double>(obj) == 0;
    }
    return true;
}


bool Interpreter::isEqual(std::any obj1,std::any obj2){
    if(!obj1.has_value() && !obj2.has_value()) return true;
    if(!obj1.has_value()) return false;
    // overloaded operator
    return obj1==obj2;
}

std::any Interpreter::evaluate(Expression* expr){
    return expr->accept(*this);
}

std::any Interpreter::visitLiteralExpression(Literal*lit){
    return lit->value;
}
std::any Interpreter::visitUnaryExpression(Unary*una){
    std::any value = this->evaluate(una->right);
    switch(una->op.type){
        case BANG:
            return !isTruthy(value);
            break;
        case TokenType::MINUS:
            assertTypeUnary(typeid(double),value,una->op);
            return -std::any_cast<double>(value);
            break;
        default:
            break;
    }
    // considers it as nil and this will never be reached since 
    // we catch invalid types
    return nullptr;
}
std::any Interpreter::visitBinaryExpression(Binary*bin) {
    std::any l = this->evaluate(bin->left);
    std::any r = this->evaluate(bin->right);
    
    switch (bin->op.type) {
        case MINUS:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) - std::any_cast<double>(r);
            break;
        case PLUS:{
            const std::type_info& tl = l.type();
            const std::type_info& tr = r.type();
            if(tl == tr  && tr == typeid(double))
                return std::any_cast<double>(l) + std::any_cast<double>(r);
            if(tl == tr  && tr == typeid(std::string))
                return std::any_cast<std::string>(l) + std::any_cast<std::string>(r);
            // just for a challenge i will only allow String + Int , the other way around will be forbiden
            // maybe it will be confusing and ill change it later .
            if(tl == typeid(std::string) && tr == typeid(double))
                return std::any_cast<std::string>(l) + std::to_string(std::any_cast<double>(r));
            // this will always be called if we dont return from the above
            assertTypeBinary(tr,l,r,bin->op);
            break;
        }
        case SLASH:{
            assertTypeBinary(typeid(double),l,r,bin->op);
            // i wont allow infinity type for now, maybe i will but have some kind 
            // of special notation (idk how usefull it is to have a inf type)
            double nemo = std::any_cast<double>(l);
            double deno = std::any_cast<double>(r);
            if(deno == 0){
                std::string err_msg = "DivisionByZeroError: undefined to devide ";
                err_msg += std::to_string(nemo);
                err_msg += " by Zero.";
                throw RunTimeError(bin->op,err_msg.c_str());
            }
            return nemo/deno; 
            break;
        }
        case STAR:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) * std::any_cast<double>(r);
            break;
        // i probebly will never allow hetrogeous comparision
        // it does not make since to me to be present , 
        // if i change my mind it will be here.
        case GREATER_EQUAL:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) >= std::any_cast<double>(r); 
            break;
        case GREATER:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) > std::any_cast<double>(r); 
            break;
        case LESS_EQUAL:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) <= std::any_cast<double>(r); 
            break;
        case LESS:
            assertTypeBinary(typeid(double),l,r,bin->op);
            return std::any_cast<double>(l) < std::any_cast<double>(r); 
            break;
        case EQUAL_EQUAL:
            return isEqual(l,r);
            break;
        case BAND_EQUAL:
            return !isEqual(l,r);
            break;
        default:
            break;
    }
    return nullptr;
}
std::any Interpreter::visitGroupingExpression(Grouping*gro) {
    return this->evaluate(gro->expression);
}


std::any Interpreter::visitVariableExpression(Variable*var){
    try {
        return this->env->get(var->name.lexeme);
    }catch(const environment::NameError&e){
        throw RunTimeError(var->name,e.what()); 
    }
}


std::any Interpreter::visitAssignExpression(Assign*ass){
    try {
        std::any value = this->evaluate(ass->expr);
        this->env->assign(ass->op.lexeme,value);
        return value;
    }catch(const environment::NameError&e){
        throw RunTimeError(ass->op,e.what()); 
    }
}


std::any Interpreter::visitConditionalExpression(Conditional*con){
    try {
        std::any value = this->evaluate(con->left);
        if(isTruthy(value)){
            return this->evaluate(con->middle);
        }else{
            return this->evaluate(con->right);
        }
    }catch(const environment::NameError&e){
        throw RunTimeError(con->op,e.what()); 
    } 
}

std::any Interpreter::visitExpressionStatement(ExpressionStatement* estmt){
    std::any value = this->evaluate(estmt->expr);
    if(this->REPL)
        std::cout << stdany_to_string(value) << '\n';
    return nullptr;
} 
std::any Interpreter::visitPrintStatement(PrintStatement* pstmt){
    std::any value = this->evaluate(pstmt->expr);
    if(stdany_to_string(value) == "nil")
        throw RunTimeError(pstmt->op,"Can't print uninitilized value , maybe replace it will nil ?");
    std::cout << stdany_to_string(value) << '\n';
    return nullptr;   
}

std::any Interpreter::visitDeclareStatement(DeclareStatement* dstmt){
    try {
        std::any value = nullptr;
        if(dstmt->init != nullptr){
            value = this->evaluate(dstmt->init);
        }
        this->env->define(dstmt->name.lexeme,value);
        return nullptr;
    }catch(const environment::NameError&e){
       throw RunTimeError(dstmt->name,e.what());
    }
}

void Interpreter::executeBlock(std::vector<Statement*>&stmts,environment&env){
    environment saved = *this->env;
    try {
        *this->env = env;
        for(Statement* st:stmts){
            this->execute(st);
        }
    }catch(const RunTimeError&e){
        // in case of a error
        *this->env = saved;
        std::string err_msg = e.what();
        Logger::error(e.faulty_op.line,e.faulty_op.col,err_msg);
    }
    *this->env = saved;
}

std::any Interpreter::visitBlockStatement(BlockStatement* bstmt){
    environment copy = *this->env;
    this->executeBlock(bstmt->stmts,copy);
    return nullptr;
}

std::any Interpreter::visitIfStatement(IfStatement* ifstmt){
    if(this->isTruthy(this->evaluate(ifstmt->Texpr))){
        this->execute(ifstmt->thenStmt);
    }else if(ifstmt->elseStmt != nullptr){
        this->execute(ifstmt->elseStmt);
    }
    return nullptr;
}

void Interpreter::Interpret(Expression* expr){
    try {
        std::any result = this->evaluate(expr);
        std::cout << stdany_to_string(result) << '\n' << "ended interpretation.\n";
    }catch(const RunTimeError&e){
        std::string err_msg = e.what();
        Logger::error(e.faulty_op.line,e.faulty_op.col,err_msg);
    }
}

void Interpreter::execute(Statement* st){
    st->accept(*this);
}


void Interpreter::InterpretProgram(std::vector<Statement*>& stmt){
    try {
        // NOTE: for a statement to be nullptr that means that that statement is faulty 
        // because of any kind of error no matter what kind , in that case a we should stop execution
        for(Statement* st:stmt){
            if(st == nullptr){
                break;
            }
            this->execute(st);
        }
        for(Statement* st:stmt){
            delete st; 
        }
    }catch(const RunTimeError&e){
        for(Statement* st:stmt){
            delete st; 
        }
        std::string err_msg = e.what();
        Logger::error(e.faulty_op.line,e.faulty_op.col,err_msg);
    }
}
