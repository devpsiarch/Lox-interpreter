# Lox Interpreter

My implementation of the lox interpreter in C++.

The goal of this project is to build a full interpreter for the Lox language from scratch, learning how programming languages work under the hood in the process.

## 🚧 Project Status

> **In Progress**

Currently, the interpreter supports:
- Lexing (scanning) the source code into tokens
- Parsing arithmetic expressions into an AST
- Evaluating arithmetic expressions at runtime

Next steps will include:
- Adding support for grouping, comparison, and equality operators
- Implementing variables and environments
- Supporting control flow (`if`, `while`, etc.)
- Function definitions and calls
- Better REPL features (history ...etc).
- FFI to execute libc functionality.

## 🔧 What’s Implemented

- [x] **Lexer (Scanner)** — Converts source code into tokens
- [x] **Parser** — Converts tokens into an abstract syntax tree (AST)
- [x] **Interpreter** — Evaluates arithmetic expressions like `1 + 2 * (3 - 4)`  

## 🧠 Motivation

I'm building this interpreter to deeply understand how programming languages work, from source code to execution. The *Crafting Interpreters* book has been a fantastic guide, and this repo is both a learning exercise and a stepping stone toward more advanced language and interpreter design.

## 📚 Reference

- [Crafting Interpreters by Bob Nystrom](https://craftinginterpreters.com/)

## ✍️ Author

Made with ❤️ by devpsiarch
---
Feel free to follow this project or fork it if you’re also learning from the book!
