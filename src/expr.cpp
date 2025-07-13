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
// place holder fr , idk what to fill these with 
// i assume they look ok in the notaion but am not sure
std::any Expression::Visitor::visitVariableExpression(Variable*var){
    (void)var;
    return this->parenthensize("variable"); 
}
std::any Expression::Visitor::visitAssignExpression(Assign*ass){
    return this->parenthensize("assignement",ass->expr); 
}
std::any Expression::Visitor::visitConditionalExpression(Conditional*con){
    return this->parenthensize("conditional",con->left
                                            ,con->middle
                                            ,con->right); 
}
std::any Expression::VisitorRPN::visitLiteralExpression(Literal*lit){
    if(!lit->value.has_value()) return "nil";
    return stdany_to_string(lit->value);
}
std::any Expression::VisitorRPN::visitUnaryExpression(Unary*uni){
   return this->parenthensize(uni->op.lexeme,uni->right); 
}
std::any Expression::VisitorRPN::visitBinaryExpression(Binary*bin){
   return this->parenthensize(bin->op.lexeme,bin->right,bin->left); 
}
std::any Expression::VisitorRPN::visitGroupingExpression(Grouping*gro){
   return this->parenthensize("group",gro->expression); 
}
std::any Expression::Visitor::visitLogicalExpression(Logical* lor){
    return this->parenthensize(lor->op.lexeme,lor->left,lor->right); 
}
