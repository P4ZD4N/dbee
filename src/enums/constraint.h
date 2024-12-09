#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <fmt/ranges.h>

enum class Constraint {
    NOT_NULL,
    UNIQUE,
    PRIMARY_KEY,
    FOREIGN_KEY
};

inline auto strings_to_constraints(std::vector<std::string> vec) -> std::vector<Constraint> {

    static const auto constraint_map = std::unordered_map<std::string, Constraint>{
            {"NOT_NULL", Constraint::NOT_NULL},
            {"UNIQUE", Constraint::UNIQUE},
            {"PRIMARY_KEY", Constraint::PRIMARY_KEY},
            {"FOREIGN_KEY", Constraint::FOREIGN_KEY}
    };

    auto constraint_vec = std::vector<Constraint>{};

    for (const auto& element : vec) {
        auto it = constraint_map.find(element);

        if (it == constraint_map.end()) {
            fmt::println("Invalid constraint: {}", element);
            return {};
        }

        constraint_vec.push_back(it->second);
    }

    return constraint_vec;
}