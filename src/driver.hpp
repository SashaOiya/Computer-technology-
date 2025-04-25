#pragma once

#include <deque>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <sstream>
#include <sqlite3.h>

#include "hwt.hpp"

using KeyT = int;

template <typename Tree_t>
std::size_t range_query(const Tree_t& tree, const KeyT fst, const KeyT snd) {
    if (fst >= snd) {
        return 0;
    }
    if constexpr (std::is_same_v<Tree_t, avl_tree::SearchTree<KeyT>>)
        return tree.my_distance(fst, snd);
    else
        return std::distance(tree.lower_bound(fst), tree.upper_bound(snd));
}

template <typename Tree_t>
auto get_answer(std::pair<std::deque<char>, std::deque<int>> data) {
    Tree_t tree = {};
    std::vector<KeyT> answers = {};
    std::ostringstream oss;

    if (data.first.empty() || data.second.empty()) {
        oss<< "Empty data";
        return oss.str();
    }

    while (!data.first.empty()) {
        auto type = data.first.front();
        data.first.pop_front();

        if (type == 'k') {
            if (data.second.empty()) {
                oss<< "Invalid key";
                return oss.str();
            }

            KeyT key = data.second.front();
            data.second.pop_front();

            tree.insert(key);
        } else if (type == 'q') {
            if (data.second.size() < 2) {
                oss << "Invalid key size";
                return oss.str();
            }
            KeyT key1 = data.second.front();
            data.second.pop_front();
            
            KeyT key2 = data.second.front();
            data.second.pop_front();

            answers.push_back(range_query<Tree_t>(tree, key1, key2));
        } else {
            throw std::runtime_error("Invalid type");
        }
    }

    for (size_t i = 0; i < answers.size(); ++i) {
        oss << answers[i];
        if (i != answers.size() - 1) {
            oss << " ";
        }
    }

    return oss.str();
}