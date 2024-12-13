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
    auto process_select_inner_join_query(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> void;
    auto process_select_left_join_query(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> void;
    auto is_valid_select_join_query(
        int join_type_clause_index,
        int join_clause_index,
        int on_clause_index
    ) const -> bool;
    auto validate_tables_and_columns(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& query_elements
    ) const -> bool;
    auto print_all_columns_for_inner_join(
        const std::string& left,
        const std::string& right
    ) const -> void;
    auto print_specific_columns_for_inner_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> void;
    auto print_all_columns_for_left_join(
        const std::string& left,
        const std::string& right
    ) const -> void;
    auto print_specific_columns_for_left_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> void;
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
    auto split_string_with_dot(const std::string& str) const -> std::pair<std::string, std::string>;
};

