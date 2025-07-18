# Lox Interpreter

My implementation of the lox interpreter in C++.

The goal of this project is to build a full interpreter for the Lox language from scratch, learning how programming languages work under the hood in the process.

## 🚧 Project Status

> **In Progress**

Currently, the interpreter supports:
- Lexing (scanning) the source code into tokens
- Parsing arithmetic expressions into an AST
- Evaluating arithmetic expressions at runtime
- Adding support for grouping, comparison, and equality operators
- Implementing variables and environments
- Supporting control flow (`if`, `while`, etc.)
- Function definitions and calls

Next steps will include:

- Better REPL features (history ...etc).
- IO / interaction with OS throught predefined functions.
- FFI to execute libc functionality.
- Multithreading feature.

## 🔧 What’s Implemented

- [x] **Lexer (Scanner)** — Converts source code into tokens
- [x] **Parser** — Converts tokens into an abstract syntax tree (AST)
- [x] **Interpreter** — Evaluates arithmetic and logical  

## 🧠 Motivation

I'm building this interpreter to deeply understand how programming languages work, from source code to execution.i choose to write a interpreter for a already designed programming language, while not a perfect replica but adding what i please in features and writing my own when ready. 
## 📚 Reference

- [Crafting Interpreters by Bob Nystrom](https://craftinginterpreters.com/)

## ✍️ Author

Made with ❤️ by devpsiarch
---
Feel free to follow this project or fork it if you’re also learning from the book!
