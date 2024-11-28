#pragma once

#include <string>
#include <unordered_map>
#include <fmt/ranges.h>

#include "table.h"

class Database {

public:
    std::string name;
    std::unordered_map<std::string, Table> tables;
    static std::vector<Database*> databases;

    Database() = default;

    explicit Database(const std::string& name) : name(name) {
        databases.push_back(this);
        fmt::println("Successfully created database with name: '{}'", name);
    }

    auto create_table(
        const std::string& name,
        const std::vector<std::string>& column_names,
        const std::vector<ColumnType>& column_types
    ) -> void;

    auto insert_data(
        const std::string& table_name,
        const std::vector<std::string>& data
    ) -> void;
};

