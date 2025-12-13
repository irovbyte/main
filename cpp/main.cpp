#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using namespace std;
using json = nlohmann::json;

// ===== Нормализация =====
string normalize(const string& s) {
    string res;
    bool lastSpace = false;
    for(char c : s) {
        if(isspace(c)) {
            if(!lastSpace) {
                res += ' ';
                lastSpace = true;
            }
        } else {
            res += tolower(c);
            lastSpace = false;
        }
    }
    if(!res.empty() && res.front() == ' ') res.erase(res.begin());
    if(!res.empty() && res.back() == ' ') res.pop_back();
    return res;
}

// ===== Работа с памятью =====
map<string,string> loadMemory(const string& filename) {
    map<string,string> memory;
    ifstream in(filename);
    string key, arrow, value;
    while(in >> key >> arrow) {
        getline(in, value);
        if(!value.empty() && value[0] == ' ') value.erase(0,1);
        memory[normalize(key)] = value;
    }
    return memory;
}

void saveMemory(const string& filename, const map<string,string>& memory) {
    ofstream out(filename);
    for(auto& pair : memory) {
        out << pair.first << " >> " << pair.second << endl;
    }
}

// ===== Curl callback =====
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// ===== URL-encode =====
string urlEncode(const string &value) {
    CURL *curl = curl_easy_init();
    char *output = curl_easy_escape(curl, value.c_str(), value.length());
    string encoded = output;
    curl_free(output);
    curl_easy_cleanup(curl);
    return encoded;
}

// ===== Запрос в интернет (Google Custom Search) =====
string fetchFromWeb(const string& query) {
    CURL* curl;
    CURLcode res;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        string apiKey = // твой ключ
        string cx = 

        string encodedQuery = urlEncode(query);
        string url = "https://www.googleapis.com/customsearch/v1?q=" + encodedQuery +
                     "&key=" + apiKey + "&cx=" + cx;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            response = string("Ошибка curl: ") + curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    try {
        json j = json::parse(response);
        if(j.contains("items") && !j["items"].empty()) {
            string result;
            int count = min((int)j["items"].size(), 3); // максимум 3 результата
            for(int i = 0; i < count; i++) {
                string title   = j["items"][i]["title"];
                string snippet = j["items"][i]["snippet"];
                string link    = j["items"][i]["link"];

                result += "Результат " + to_string(i+1) + ":\n";
                result += "Заголовок: " + title + "\n";
                result += "Описание: " + snippet + "\n";
                result += "Источник: " + link + "\n\n";
            }
            return result;
        } else {
            return "Нет результатов.";
        }
    } catch(...) {
        return "Ответ не в формате JSON: " + response.substr(0, 200);
    }
}

// ===== Основной код =====
int main() {
    string filename = "memory.txt";
    map<string,string> memory = loadMemory(filename);

    cout << "ИИ готов к диалогу! Напиши что-нибудь (exit для выхода)." << endl;

    string userInput;
    while(true) {
        cout << "Ты: ";
        getline(cin, userInput);

        if(userInput == "exit") break;

        string normInput = normalize(userInput);

        // Проверяем точное совпадение
        if(memory.find(normInput) != memory.end()) {
            cout << "ИИ: У меня уже есть ответ: \"" << memory[normInput] << "\"" << endl;
            cout << "ИИ: Хочешь использовать его или добавить новый вариант? (использовать/новый)" << endl;

            string choice;
            getline(cin, choice);
            choice = normalize(choice);

            if(choice == "использовать") {
                cout << "ИИ: " << memory[normInput] << endl;
            } else if(choice == "новый") {
                cout << "ИИ: Напиши новый правильный ответ:" << endl;
                string correctAnswer;
                getline(cin, correctAnswer);

                memory[normInput] = correctAnswer;

                // сохраняем ключевые слова
                istringstream iss(normInput);
                string word;
                while(iss >> word) {
                    if(memory.find(word) == memory.end()) {
                        memory[word] = correctAnswer;
                    }
                }

                saveMemory(filename, memory);
                cout << "ИИ: Запомнил новый ответ!" << endl;
            } else {
                cout << "ИИ: Я не понял твой выбор." << endl;
            }
        } else {
            // Поиск похожих ключей
            vector<string> candidates;
            for(auto& pair : memory) {
                if(pair.first.find(normInput) != string::npos) {
                    candidates.push_back(pair.first);
                }
            }

            if(candidates.empty()) {
                // если ничего нет — пробуем интернет
                string webResult = fetchFromWeb(normInput);
                cout << "ИИ (из интернета): \n\n" << webResult << endl;

                cout << "ИИ: Хочешь сохранить этот ответ в память? (да/нет)" << endl;
                string choice;
                getline(cin, choice);
                choice = normalize(choice);

                if(choice == "да") {
                    memory[normInput] = webResult;
                    saveMemory(filename, memory);
                    cout << "ИИ: Запомнил!" << endl;
                }
            } else {
                cout << "ИИ: Я нашёл похожие ответы:" << endl;
                for(size_t i=0; i<candidates.size(); i++) {
                    cout << i+1 << ". " << candidates[i] << " >> " << memory[candidates[i]] << endl;
                }

                cout << "ИИ: Хочешь использовать один из них или добавить новый? (использовать/новый)" << endl;
                string choice;
                getline(cin, choice);
                choice = normalize(choice);

                if(choice == "использовать") {
                    cout << "ИИ: Введи номер варианта:" << endl;
                    string num;
                    getline(cin, num);
                    int idx = stoi(num) - 1;
                    if(idx >= 0 && idx < (int)candidates.size()) {
                        cout << "ИИ: " << memory[candidates[idx]] << endl;
                    } else {
                        cout << "ИИ: Неверный номер." << endl;
                    }
                } else if(choice == "новый") {
                    cout << "ИИ: Напиши новый правильный ответ:" << endl;
                    string correctAnswer;
                    getline(cin, correctAnswer);

                    memory[normInput] = correctAnswer;

                    istringstream iss(normInput);
                    string word;
                    while(iss >> word) {
                        if(memory.find(word) == memory.end()) {
                            memory[word] = correctAnswer;
                        }
                    }

                    saveMemory(filename, memory);
                    cout << "ИИ: Запомнил новый ответ!" << endl;
                } else {
                    cout << "ИИ: Я не понял твой выбор." << endl;
                }
            }
        }
    }

    return 0;
}