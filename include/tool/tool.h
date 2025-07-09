#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <any>


std::string type_to_string(const std::type_info& t);
// returns a std::string for an std::any 
std::string stdany_to_string(const std::any& a);
// reads the file content to std::string
std::string read_file(const std::string&filename);
