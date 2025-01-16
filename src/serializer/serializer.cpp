#include "serializer.h"

#include <fstream>
#include <fmt/ostream.h>

auto format_as(const ColumnType column_type) {
    switch (column_type) {
        case ColumnType::INTEGER:
            return "INTEGER";
        case ColumnType::FLOAT:
            return "FLOAT";
        case ColumnType::TEXT:
            return "TEXT";
        default:
            return "UNKNOWN";
    }
}

auto format_as(const Constraint constraint) {
    switch (constraint) {
        case Constraint::PRIMARY_KEY:
            return "PRIMARY_KEY";
        case Constraint::FOREIGN_KEY:
            return "FOREIGN_KEY";
        case Constraint::UNIQUE:
            return "UNIQUE";
        case Constraint::NOT_NULL:
            return "NOT_NULL";
        default:
            return "UNKNOWN";
    }
}

auto format_as(const std::pair<Table*, std::string>& pair) -> std::string {
    if (pair.first == nullptr) return "";

    return "(" + pair.first->name + ", " + pair.second + ")";
}


auto Serializer::serialize_database(const std::unordered_map<std::string, Table>& tables) -> void {


    auto file = std::fstream("../../data.txt", std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: data.txt");
    }

    for (const auto& [name, table] : tables) {
        fmt::println(file, "{}", name);
        fmt::println(file, "{}", "");

        fmt::println(file, "{}", table.column_names);

        fmt::println(file, "{}", "");

        fmt::println(file, "{}", table.column_types);

        fmt::println(file, "{}", "");

        fmt::println(file, "{}", table.column_constraints);

        fmt::println(file, "{}", "");

        fmt::println(file, "{}", table.column_foreign_keys);

        fmt::println(file, "{}", "");

        for (const auto& row : table.rows) {
            fmt::println(file, "{}", row);
        }

        fmt::println(file, "{}", "");

        fmt::println(file, "{}", "-");
    }
}
