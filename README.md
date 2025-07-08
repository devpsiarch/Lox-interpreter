# Lox Interpreter

This is my implementation of the Lox programming language in C++.

The goal of this project is to build a full interpreter for the Lox language from scratch, learning how programming languages work under the hood in the process.

## 🚧 Project Status

> **In Progress**

Currently, I’ve completed the **lexer (scanner)**, which is responsible for turning raw source code into a stream of tokens.

Next steps will include:
- Parser
- AST generation
- Interpreter
- (and possibly later) a bytecode VM

## 🔧 What’s Implemented

- [x] **Lexer (Scanner)** — Converts source code into tokens, handling keywords, literals, operators, and more.

## Additional features
- [ ] bitwise operations.
- [ ] display of faulty line of code.

## 📁 Project Structure
```
├── include
│ └── lexer.h
├── lox_examples
│ ├── main.lox
│ └── test.lox
├── Makefile
└── src
├── lexer.cpp
└── main.cpp
```

## 🧠 Motivation

I'm building this interpreter to deeply understand how programming languages work, from source code to execution. The *Crafting Interpreters* book has been a fantastic guide, and this repo is both a learning exercise and a stepping stone toward more advanced language and interpreter design.

## 📚 Reference

- [Crafting Interpreters by Bob Nystrom](https://craftinginterpreters.com/)

## ✍️ Author

Made with ❤️ by devpsiarch

---

Feel free to follow this project or fork it if you’re also learning from the book!
