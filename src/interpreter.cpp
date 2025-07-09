#include "../include/interpreter.h"

/* stopped at handling the checking for types when evaluating*/


bool Interpreter::checkType(std::type_info&expected,std::any obj1,std::any obj2){
    const std::type_info& l = obj1.type();
    const std::type_info& r = obj2.type();
    if(l != expected || r != expected) return false;
    return true;
}

void Interpreter::assertTypeBinary(std::type_info&expected,std::any obj1,std::any obj2,Token op){
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
void Interpreter::assertTypeUnary(std::type_info&expected,std::any obj,Token op){
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
            // type check here
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
    

    const std::type_info& tl = l.type();
    const std::type_info& tr = r.type();

    switch (bin->op.type) {
        case MINUS:
            if(tl == tr  && tr == typeid(double))
                return std::any_cast<double>(l) - std::any_cast<double>(r);
            break;
        case PLUS:
            if(tl == tr  && tr == typeid(double))
                return std::any_cast<double>(l) + std::any_cast<double>(r);
            if(tl == tr  && tr == typeid(std::string))
                return std::any_cast<std::string>(l) + std::any_cast<std::string>(r);           
            break;
        case SLASH:
            if(tl == tr  && tr == typeid(double))
                return std::any_cast<double>(l) / std::any_cast<double>(r);
            break;
        case STAR:
            if(tl == tr  && tr == typeid(double))
                return std::any_cast<double>(l) * std::any_cast<double>(r);
            break;
        case GREATER_EQUAL:
            return std::any_cast<bool>(l) >= std::any_cast<bool>(r); 
            break;
        case GREATER:
            return std::any_cast<bool>(l) > std::any_cast<bool>(r); 
            break;
        case LESS_EQUAL:
            return std::any_cast<bool>(l) <= std::any_cast<bool>(r); 
            break;
        case LESS:
            return std::any_cast<bool>(l) < std::any_cast<bool>(r); 
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
