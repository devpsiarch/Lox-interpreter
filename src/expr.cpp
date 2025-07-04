#include "../include/expr.h"

std::any Expression::Visitor::visitLiteralExpression(Literal*lit){
    if(!lit->value.has_value()) return "nil";
    return stdany_to_string(lit->value);
}
std::any Expression::Visitor::visitUnaryExpression(Unary*uni){
   return this->parenthensize(uni->op.lexeme,uni->right); 
}
std::any Expression::Visitor::visitBinaryExpression(Binary*bin){
   return this->parenthensize(bin->op.lexeme,bin->right,bin->left); 
}
std::any Expression::Visitor::visitGroupingExpression(Grouping*gro){
   return this->parenthensize("group",gro->expression); 
}
