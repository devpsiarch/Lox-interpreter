#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <any>
// returns a std::string for an std::any 
std::string stdany_to_string(const std::any& a);
// reads the file content to std::string
std::string read_file(const std::string&filename);
// interprits the whole file at one
void runFile(const std::string&filename);
// interprites as "REPl".
void runPrompt();
