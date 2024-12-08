#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../database/database.h"

class Parser {

public:
    std::optional<Database> database;

    Parser() = default;

    explicit Parser(const Database& database) : database(database) {}

    auto parse_query(const std::string& query) -> void;

private:
    auto process_database_query(const std::vector<std::string>& query_elements) -> void;
    auto process_select_query(const std::vector<std::string>& query_elements) const -> void;
    auto process_insert_query(const std::vector<std::string>& query_elements) -> void;
    auto process_alter_query(const std::vector<std::string>& query_elements) -> void;
    auto process_table_query(const std::vector<std::string>& query_elements) -> void;
    [[nodiscard]] auto is_database_selected() const -> bool;
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
    static auto get_foreign_key_from_curly_braces(const std::string& detail) -> std::string;
    auto is_foreign_key_valid(
        const std::string& foreign_table_name,
        const std::string& foreign_column_name,
        const std::string& column_type
    ) const -> bool;
};

