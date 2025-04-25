#pragma once

#include <sqlite3.h>
#include <deque>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

#include "hwt.hpp"
#include "visual.hpp"
#include "driver.hpp"

using KeyT = int;

template <typename FileType>
auto get_data_from_file(std::string input_file) {
    FileType file(input_file);
    std::pair<std::deque<char>, std::deque<int>> data = {};

    char type = 0;
    while (file >> type) {
        if (!file.good()) {
            throw std::runtime_error("Invalid type");
        }

        if (type == 'k') {
            KeyT key;
            file >> key;
            if (!file.good()) {
                throw std::runtime_error("Invalid key");
            }

            data.first.push_back(type);
            data.second.push_back(key);
        } else if (type == 'q') {
            KeyT key1, key2;
            file >> key1;
            if (!file.good()) {
                throw std::runtime_error("Invalid key");
            }
            file >> key2;
            if (!file.good()) {
                throw std::runtime_error("Invalid key");
            }

            data.first.push_back(type);
            data.second.push_back(key1);
            data.second.push_back(key2);
        } else {
            throw std::runtime_error("Invalid type");
        }
    }

    return data;
}

std::pair<std::deque<char>, std::deque<int>> get_data_from_db(const std::string& test_name) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc = sqlite3_open("../test_bd.bd", &db);  // Убедитесь, что путь правильный
    if (rc) {
        throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* sql = "SELECT type, key1, key2 FROM test_data WHERE test_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare SQL statement: " + std::string(sqlite3_errmsg(db)));
    }

    rc = sqlite3_bind_text(stmt, 1, test_name.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(db)));
    }

    std::pair<std::deque<char>, std::deque<int>> data;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* type_text = sqlite3_column_text(stmt, 0);
        if (type_text) {
            char type = static_cast<char>(type_text[0]);
            data.first.push_back(type);
        } else {
            std::cerr << "Warning: 'type' column is NULL for test_name: " << test_name << std::endl;
        }

        int key1 = sqlite3_column_int(stmt, 1);
        data.second.push_back(key1);

        int key2 = sqlite3_column_int(stmt, 2);
        data.second.push_back(key2);
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error("Failed to read data from database: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return data;
}

// Обработка логики работы с данными
auto processData(Mode mode, std::pair<std::deque<char>, std::deque<int>>& inputData) {
    std::string answer = {};

    switch (mode) {
        case OPTIMIZED_TREE:
            answer = get_answer<avl_tree::SearchTree<int>>(inputData);
            break;
        case STD_SET:
            answer = get_answer<std::set<int>>(inputData);
            break;
        case BOTH:
            std::string treeResult, setResult;
            std::chrono::duration<double> treeTime, setTime;
            std::mutex resultMutex;

            auto treeTask = [&]() {
                auto start = std::chrono::high_resolution_clock::now();
                auto result = get_answer<avl_tree::SearchTree<int>>(inputData);
                auto end = std::chrono::high_resolution_clock::now();
                std::lock_guard<std::mutex> lock(resultMutex);
                treeResult = result;
                treeTime = end - start;
            };

            auto setTask = [&]() {
                auto start = std::chrono::high_resolution_clock::now();
                auto result = get_answer<std::set<int>>(inputData);
                auto end = std::chrono::high_resolution_clock::now();
                std::lock_guard<std::mutex> lock(resultMutex);
                setResult = result;
                setTime = end - start;
            };

            std::thread t1(treeTask);
            std::thread t2(setTask);
            t1.join();
            t2.join();

            std::ostringstream oss;
            oss << "Optimized Tree Result (" << treeTime.count() << "s):\n" << treeResult << "\n\n";
            oss << "std::set Result (" << setTime.count() << "s):\n" << setResult << "\n\n";
            oss << "Optimised tree is " << setTime / treeTime << " times faster than std::set\n";

            answer = oss.str();
            break;
    }

    return answer;
}
