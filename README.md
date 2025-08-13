# Lox Interpreter
> This project is not finished yet, still have problems and bugs.

My implementation of the lox interpreter in C++.

The goal of this project is to build a full interpreter for the Lox language from scratch, learning how programming languages work under the hood in the process.

```kotlin
fun factorial(n) {
    var result = 1;
    for (var i = 2; i <= n; i = i + 1) {
        result = result * i;
    }
    return result;
}

fun sumUpTo(limit) {
    var total = 0;
    var i = 1;
    while (i <= limit) {
        total = total + i;
        i = i + 1;
    }
    return total;
}

fun map(obj,fn){
    fn(obj);
}
var printer = fun (x) {
    print x;
};

```

You can find more code examples in `lox_examples`.

## 🚧 Project Status

> **In Progress**

Currently, the interpreter supports:
- Lexing (scanning) the source code into tokens
- Parsing arithmetic expressions into an AST
- Evaluating arithmetic and logical expressions at runtime
- declaring variables and environment management
- Supporting control flow (`if`, `while`, etc.)
- Function definitions and calls
- anonymous functions and passing them
- nested Function definitions and calls

Next steps will include:

- Better REPL features (history ...etc).
- IO / interaction with OS throught predefined functions.
- FFI to execute libc functionality.
- Multithreading feature.

## 🔧 What’s Implemented

- [x] **Lexer (Scanner)** — Converts source code into tokens
- [x] **Parser** — Converts tokens into an abstract syntax tree (AST)
- [x] **Interpreter** — Evaluates arithmetic and logical  

## Problems to fix 
- [ ] reassignement bug.
- [ ] runtime binding of functions when passed to other.

## 🧠 Motivation

I'm building this interpreter to deeply understand how programming languages work, from source code to execution.i choose to write a interpreter for a already designed programming language, while not a perfect replica but adding what i please in features and writing my own when ready. 
## 📚 Reference

- [Crafting Interpreters by Bob Nystrom](https://craftinginterpreters.com/)

## ✍️ Author

Made with ❤️ by devpsiarch
---
Feel free to follow this project or fork it if you’re also learning from the book!
