#include "memory.h"
#include <sqlite3.h>
#include <iostream>

static sqlite3* openDB(const std::string& filename) {
    sqlite3* db = nullptr;
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(db) << std::endl;
        if (db) sqlite3_close(db);
        return nullptr;
    }
    const char* createSQL =
        "CREATE TABLE IF NOT EXISTS memory ("
        "key TEXT PRIMARY KEY,"
        "value TEXT"
        ");";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, createSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQLite table create error: " << (errMsg ? errMsg : "") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
    }
    return db;
}

std::map<std::string, std::string> loadMemory(const std::string& filename) {
    std::map<std::string, std::string> memory;
    sqlite3* db = openDB(filename);
    if (!db) return memory;

    const char* selectSQL = "SELECT key, value FROM memory;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* k = sqlite3_column_text(stmt, 0);
            const unsigned char* v = sqlite3_column_text(stmt, 1);
            std::string key   = k ? reinterpret_cast<const char*>(k) : "";
            std::string value = v ? reinterpret_cast<const char*>(v) : "";
            if (!key.empty()) memory[key] = value;
        }
    } else {
        std::cerr << "SQLite prepare error (load)" << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return memory;
}

void saveMemory(const std::string& filename,
                const std::map<std::string, std::string>& memory) {
    sqlite3* db = openDB(filename);
    if (!db) return;

    const char* insertSQL =
        "INSERT OR REPLACE INTO memory (key, value) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite BEGIN failed" << std::endl;
    }

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        for (const auto& [key, value] : memory) {
            sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "SQLite insert error for key: " << key << std::endl;
            }
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
        }
    } else {
        std::cerr << "SQLite prepare error (save)" << std::endl;
    }

    sqlite3_finalize(stmt);

    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite COMMIT failed" << std::endl;
    }

    sqlite3_close(db);
}