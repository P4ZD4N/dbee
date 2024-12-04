#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

enum class Constraint {
    NOT_NULL,
    UNIQUE,
    PRIMARY_KEY
};

inline auto strings_to_constraints(std::vector<std::string> vec) -> std::vector<Constraint> {

    static const auto constraint_map = std::unordered_map<std::string, Constraint>{
            {"NOT_NULL", Constraint::NOT_NULL},
            {"UNIQUE", Constraint::UNIQUE},
            {"PRIMARY_KEY", Constraint::PRIMARY_KEY}
    };

    auto constraint_vec = std::vector<Constraint>{};

    for (const auto& element : vec) {
        auto it = constraint_map.find(element);

        if (it == constraint_map.end()) throw std::invalid_argument("Invalid constraint: " + element);

        constraint_vec.push_back(it->second);
    }

    return constraint_vec;
}