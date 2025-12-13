#include "ai.h"
#include "memory.h"
#include <iostream>
#include <map>

int main() {
    const std::string filename = "data/memory.db";
    std::map<std::string,std::string> memory = loadMemory(filename);

    std::cout << "🤖 ИИ готов к диалогу! Напиши что-нибудь (exit для выхода)." << std::endl;

    std::string userInput;
    while (true) {
        std::cout << "Ты: ";
        if (!std::getline(std::cin, userInput)) break;

        if (userInput == "exit") break;

        processInput(userInput, memory, filename);
    }

    return 0;
}