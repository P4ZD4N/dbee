#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "fmt/ranges.h"

enum class ColumnType {
    INVALID,
    INTEGER,
    TEXT,
    FLOAT
};

inline auto string_to_column_type(const std::string& str) -> ColumnType {

    static const auto column_type_map = std::unordered_map<std::string, ColumnType>{
        {"INTEGER", ColumnType::INTEGER},
        {"TEXT", ColumnType::TEXT},
        {"FLOAT", ColumnType::FLOAT}
    };

    auto it = column_type_map.find(str);

    if (it != column_type_map.end()) return it->second;

    fmt::println("Invalid column type: {}", str);
    return {};
}

inline auto strings_to_column_types(const std::vector<std::string> vec) -> std::vector<ColumnType> {

    static const auto column_type_map = std::unordered_map<std::string, ColumnType>{
            {"INTEGER", ColumnType::INTEGER},
            {"TEXT", ColumnType::TEXT},
            {"FLOAT", ColumnType::FLOAT}
    };

    auto newVec = std::vector<ColumnType>{};

    for (const auto& element: vec) {
        auto it = column_type_map.find(element);
        if (it == column_type_map.end()) {
            fmt::println("Invalid column type: {}", element);
            return {};
        }
        newVec.push_back(it->second);
    }

     return newVec;
}

inline auto column_type_to_string(const ColumnType column_type) -> std::string {
    switch (column_type) {
        case ColumnType::INTEGER: return "INTEGER";
        case ColumnType::TEXT: return "TEXT";
        case ColumnType::FLOAT: return "FLOAT";
        default: return "INVALID";
    }
}