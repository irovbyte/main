#include "web.h"
#include "format.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* output = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    output->append(static_cast<const char*>(contents), totalSize);
    return totalSize;
}

static std::string urlEncode(const std::string& value) {
    std::string encoded;
    CURL *curl = curl_easy_init();
    if (!curl) return value;
    char *out = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    if (out) {
        encoded = out;
        curl_free(out);
    } else {
        encoded = value;
    }
    curl_easy_cleanup(curl);
    return encoded;
}

std::string fetchFromWeb(const std::string& query) {
    CURL* curl = nullptr;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Вынеси ключи в ENV: GOOGLE_API_KEY и GOOGLE_CX
        const char* apiKeyEnv = std::getenv("GOOGLE_API_KEY");
        const char* cxEnv     = std::getenv("GOOGLE_CX");
        std::string apiKey = apiKeyEnv ? apiKeyEnv : "YOUR_API_KEY";
        std::string cx     = cxEnv ? cxEnv : "YOUR_CX";

        std::string encodedQuery = urlEncode(query);
        std::string url = "https://www.googleapis.com/customsearch/v1?q=" + encodedQuery +
                          "&key=" + apiKey + "&cx=" + cx;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "cpp-bot/1.0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            response = std::string("curl error: ") + curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
    } else {
        curl_global_cleanup();
        return formatError("curl init failed");
    }

    curl_global_cleanup();

    try {
        json j = json::parse(response);
        if (j.contains("items") && j["items"].is_array() && !j["items"].empty()) {
            std::string result = "Вот несколько подробных ответов:\n\n";
            int count = std::min<int>(j["items"].size(), 3);
            for (int i = 0; i < count; i++) {
                std::string title   = j["items"][i].value("title", "");
                std::string snippet = j["items"][i].value("snippet", "");
                std::string link    = j["items"][i].value("link", "");
                result += formatResult(i+1, title, snippet, link);
            }
            return result;
        } else {
            return "Нет результатов.";
        }
    } catch (const std::exception& e) {
        return formatError(std::string("Ответ не в формате JSON: ") +
                           response.substr(0, 200));
    }
}