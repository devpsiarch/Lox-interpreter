#include "../testa.h"
#include "../include/lexer.h"
#include "../include/expr.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

#define TESTING 0
int test(void);
void runPrompt();
void runFile();

int main(int argc,char *argv[]){
    if(!TESTING){
        Expression::Visitor v;
        Expression::VisitorRPN vr;
        (void)vr;
        Lexer l = Lexer(read_file("lox_examples/expr.lox"));
        l.scanSource();
        parser p = parser(l.tokens);
        std::vector<Statement*> stmnt =  p.parserProgram();
        Interpreter inter;
        inter.InterpretProgram(stmnt);

        return 0;
    }else{
        test();
        return 0;
    }
}


void eval_expr(const std::string&line){
    Lexer l = Lexer(line);
    l.scanSource();
    parser p = parser(l.tokens);
    Expression* expr = p.parse();
    if(expr == nullptr) return ;           
    Expression::Visitor v;
    Interpreter inter;
    inter.Interpret(expr);
    std::string ast_str = v.astprinter(expr);
    std::cout << ast_str << '\n';
    delete expr;
}

void run_line(const std::string&line){
    Lexer l = Lexer(line);
    l.scanSource();
    parser p = parser(l.tokens);
    std::vector<Statement*> stmts = p.parserProgram();
    Interpreter inter;
    inter.InterpretProgram(stmts);
}

void runPrompt(){
    std::string line;
    bool shouldclose = false;
    while(!shouldclose){
        // interpritation here
        std::cout << "=> ";
        std::getline(std::cin,line);
        if(line == "exit"){
            shouldclose = true;
            continue;
        }else if(line == "clear"){
            system("clear");
            continue;
        }
        //eval_expr(line);
        run_line(line);
    }
}

int test(void){
    Expression::Visitor v;
    InitTesta();

    // 1) Simple unary: (- 123)
    {
        Unary expr1(
            Token(MINUS, "-"),
            new Literal(123)
        );
        ASSERRT_EQ(
            "(- 123)",
            v.astprinter(&expr1),
            "Wrong on simple unary literal"
        );
    }

    // 2) Nested grouping: (group (+ 1 2))
    {
        Grouping expr2(
            new Binary(
                Token(PLUS, "+"),
                new Literal(1),
                new Literal(2)
            )
        );
        ASSERRT_EQ(
            "(group (+ 1 2))",
            v.astprinter(&expr2),
            "Wrong on nested grouping"
        );
    }

    // 3) Mix of binary, unary, and grouping: (* (- 4) (group (/ 8 2)))
    {
        Binary expr3(
            Token(STAR, "*"),
            new Unary(
                Token(MINUS, "-"),
                new Literal(4)
            ),
            new Grouping(
                new Binary(
                    Token(SLASH, "/"),
                    new Literal(8),
                    new Literal(2)
                )
            )
        );
        ASSERRT_EQ(
            "(* (- 4) (group (/ 8 2)))",
            v.astprinter(&expr3),
            "Wrong on mix of binary, unary, grouping"
        );
    }

    // 4) Deeply nested plus with a string “false”: (+ (+ 123 (* 69 100)) (! false))
    {
        Binary expr4(
            Token(PLUS, "+"),
            new Binary(
                Token(PLUS, "+"),
                new Literal(123),
                new Binary(
                    Token(STAR, "*"),
                    new Literal(69),
                    new Literal(100)
                )
            ),
            new Unary(
                Token(BANG, "!"),
                new Literal(std::string("false"))
            )
        );
        ASSERRT_EQ(
            "(+ (+ 123 (* 69 100)) (! false))",
            v.astprinter(&expr4),
            "Wrong on deeply nested plus with string literal"
        );
    }

    // 5) All‑nil edge case: nil
    {
        Literal nilLit(std::any{});
        ASSERRT_EQ(
            "nil",
            v.astprinter(&nilLit),
            "Wrong on nil literal"
        );
    }
 {
        Literal strLit(std::string("foo bar"));
        ASSERRT_EQ(
            "foo bar",
            v.astprinter(&strLit),
            "Wrong on string literal only"
        );
    }

    // 7) Grouping of a literal: (group 42)
    {
        Grouping expr6(
            new Literal(42)
        );
        ASSERRT_EQ(
            "(group 42)",
            v.astprinter(&expr6),
            "Wrong on grouping of an integer literal"
        );
    }

    // 8) Unary of a grouping of a string: (- (group test))
    {
        Unary expr7(
            Token(MINUS, "-"),
            new Grouping(
                new Literal(std::string("test"))
            )
        );
        ASSERRT_EQ(
            "(- (group test))",
            v.astprinter(&expr7),
            "Wrong on unary of grouping of a string literal"
        );
    }

    // 9) Mixed nested: (group (* (+ 2 3) (! x)))
    {
        Grouping expr8(
            new Binary(
                Token(STAR, "*"),
                new Binary(
                    Token(PLUS, "+"),
                    new Literal(2),
                    new Literal(3)
                ),
                new Unary(
                    Token(BANG, "!"),
                    new Literal(std::string("x"))
                )
            )
        );
        ASSERRT_EQ(
            "(group (* (+ 2 3) (! x)))",
            v.astprinter(&expr8),
            "Wrong on grouping of a complex nested expression"
        );
    }

    // 10) Deep three-level nesting: (+ 1 (group (- 2))) 
    {
        Binary expr9(
            Token(PLUS, "+"),
            new Literal(1),
            new Grouping(
                new Unary(
                    Token(MINUS, "-"),
                    new Literal(2)
                )
            )
        );
        ASSERRT_EQ(
            "(+ 1 (group (- 2)))",
            v.astprinter(&expr9),
            "Wrong on deep three-level nesting"
        );
    }
        // 11) Binary with two strings: (+ hello world)
    {
        Binary expr10(
            Token(PLUS, "+"),
            new Literal(std::string("hello")),
            new Literal(std::string("world"))
        );
        ASSERRT_EQ(
            "(+ hello world)",
            v.astprinter(&expr10),
            "Wrong on binary with two strings"
        );
    }

    // 12) Unary double negation: (! (! true))
    {
        Unary expr11(
            Token(BANG, "!"),
            new Unary(
                Token(BANG, "!"),
                new Literal(std::string("true"))
            )
        );
        ASSERRT_EQ(
            "(! (! true))",
            v.astprinter(&expr11),
            "Wrong on double negation"
        );
    }

    // 13) Binary left-nested chain: (+ (+ 1 2) 3)
    {
        Binary expr12(
            Token(PLUS, "+"),
            new Binary(
                Token(PLUS, "+"),
                new Literal(1),
                new Literal(2)
            ),
            new Literal(3)
        );
        ASSERRT_EQ(
            "(+ (+ 1 2) 3)",
            v.astprinter(&expr12),
            "Wrong on left-nested binary"
        );
    }

    // 14) Binary right-nested chain: (+ 1 (+ 2 3))
    {
        Binary expr13(
            Token(PLUS, "+"),
            new Literal(1),
            new Binary(
                Token(PLUS, "+"),
                new Literal(2),
                new Literal(3)
            )
        );
        ASSERRT_EQ(
            "(+ 1 (+ 2 3))",
            v.astprinter(&expr13),
            "Wrong on right-nested binary"
        );
    }

    // 15) Complex combo: (! (group (+ "x" (* 5 nil))))
    {
        Unary expr14(
            Token(BANG, "!"),
            new Grouping(
                new Binary(
                    Token(PLUS, "+"),
                    new Literal(std::string("x")),
                    new Binary(
                        Token(STAR, "*"),
                        new Literal(5),
                        new Literal(std::any{})  // nil
                    )
                )
            )
        );
        ASSERRT_EQ(
            "(! (group (+ x (* 5 nil))))",
            v.astprinter(&expr14),
            "Wrong on complex combo of all types"
        );

    }
    EndTesta();
    return 0;
}
