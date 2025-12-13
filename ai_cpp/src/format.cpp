#include "format.h"
#include <string>

std::string formatResult(int index, const std::string& title,
                         const std::string& snippet, const std::string& link) {
    std::string result;
    result += "🔎 Результат " + std::to_string(index) + "\n";
    result += "Заголовок: " + title + "\n";
    result += "Описание: " + snippet + "\n";
    result += "Источник: " + link + "\n";
    result += "———————————————\n\n";
    return result;
}

std::string formatError(const std::string& message) {
    return "⚠️ Ошибка: " + message + "\n";
}