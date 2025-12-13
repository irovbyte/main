#include "memory.h"
#include "utils.h"
#include <fstream>
#include <sstream>

// Вспомогательная функция: аккуратно убрать ведущие/замыкающие пробелы
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Формат строки в файле: "key >> value"
// Читаем построчно и делим по ">>"
std::map<std::string, std::string> loadMemory(const std::string& filename) {
    std::map<std::string, std::string> memory;

    std::ifstream in(filename);
    if (!in.is_open()) {
        // файл может не существовать при первом запуске — это ок
        return memory;
    }

    std::string line;
    while (std::getline(in, line)) {
        // пропускаем пустые строки
        if (line.empty()) continue;

        // ищем разделитель ">>"
        std::size_t pos = line.find(">>");
        if (pos == std::string::npos) {
            // строка без разделителя — пропустим, чтобы не ломать парсер
            continue;
        }

        std::string keyRaw = trim(line.substr(0, pos));
        std::string valueRaw = trim(line.substr(pos + 2)); // после ">>"

        // нормализуем ключ (нижний регистр + одиночные пробелы)
        std::string key = normalize(keyRaw);
        std::string value = valueRaw; // значение не нормализуем, храним как ввёл пользователь

        if (!key.empty()) {
            memory[key] = value;
        }
    }

    return memory;
}

void saveMemory(const std::string& filename, const std::map<std::string, std::string>& memory) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        // можно добавить лог, но работу не прерываем
        return;
    }

    for (const auto& pair : memory) {
        // ключ пишем как есть (он уже нормализован при сохранении),
        // значение — оригинальное, без нормализации
        out << pair.first << " >> " << pair.second << '\n';
    }
}