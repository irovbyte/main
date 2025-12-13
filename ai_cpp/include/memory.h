#pragma once
#include <map>
#include <string>

std::map<std::string, std::string> loadMemory(const std::string& filename);
void saveMemory(const std::string& filename,
                const std::map<std::string, std::string>& memory);