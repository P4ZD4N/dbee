#pragma once

#include <string>
#include <unordered_map>
#include <fmt/ranges.h>

#include "../table/table.h"

class Database {

public:
    std::string name;
    std::unordered_map<std::string, Table> tables;
    static std::unordered_map<std::string, Database> databases;

    Database() = default;

    explicit Database(const std::string& name) : name(name) {
        databases.insert({name, *this});
        fmt::println("Successfully created database with name: '{}'", name);
    }

    auto create_table(
        const std::string& name,
        const std::vector<std::string>& column_names,
        const std::vector<ColumnType>& column_types,
        const std::vector<std::vector<Constraint>>& column_constraints,
        const std::vector<std::pair<Table, std::string>>& column_foreign_keys
    ) -> void;

    auto drop_table(const std::string& name) -> void;

    auto insert_data(
        const std::string& table_name,
        const std::vector<std::string>& data
    ) -> void;

    auto get_table_by_name(
        const std::string& table_name
    ) -> Table&;

    static auto create_database(const std::string& database_name) -> void;

    static auto get_database(const std::string& database_name) -> Database&;

    static auto drop_database(const std::string& database_name) -> void;
};

