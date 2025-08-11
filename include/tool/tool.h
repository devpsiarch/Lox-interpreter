#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <any>
#include <unordered_set>


static std::unordered_set<void*> deleted_ptrs;

std::string type_to_string(const std::type_info& t);
// returns a std::string for an std::any 
std::string stdany_to_string(const std::any& a);
// reads the file content to std::string
std::string read_file(const std::string&filename);
// logs deletion for debugging
void log_delete(const void* ptr, const char* who);
