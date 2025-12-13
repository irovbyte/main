#include "ai.h"
#include "utils.h"
#include "memory.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

void processInput(const std::string& userInput,
                  std::map<std::string,std::string>& memory,
                  const std::string& filename) {
    std::string normInput = normalize(userInput);

    // 1. Точное совпадение
    auto it = memory.find(normInput);
    if (it != memory.end()) {
        std::cout << "ИИ: У меня уже есть ответ: \"" << it->second << "\"" << std::endl;
        return;
    }

    // 2. Поиск похожих ключей (contains)
    std::vector<std::string> candidates;
    for (const auto& pair : memory) {
        if (pair.first.find(normInput) != std::string::npos) {
            candidates.push_back(pair.first);
        }
    }

    if (!candidates.empty()) {
        std::cout << "ИИ: Я нашёл похожие ответы:" << std::endl;
        for (size_t i = 0; i < candidates.size(); i++) {
            std::cout << i+1 << ". " << candidates[i]
                      << " >> " << memory[candidates[i]] << std::endl;
        }
        return;
    }

    // 3. Обучение вручную
    std::cout << "ИИ: Я не знаю ответа. Обучи меня сам!" << std::endl;
    std::cout << "Напиши правильный ответ:" << std::endl;
    std::string correctAnswer;
    std::getline(std::cin, correctAnswer);

    // сохраняем сразу
    memory[normInput] = correctAnswer;

    // сохраняем ключевые слова
    std::istringstream iss(normInput);
    std::string word;
    while (iss >> word) {
        if (memory.find(word) == memory.end()) {
            memory[word] = correctAnswer;
        }
    }

    saveMemory(filename, memory);
    std::cout << "ИИ: Запомнил новый ответ!" << std::endl;
}