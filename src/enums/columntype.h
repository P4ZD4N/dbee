#pragma once

enum class ColumnType {
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

    throw std::invalid_argument("Invalid column type: " + str);
}