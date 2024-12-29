#pragma once

#include "../parser.h"

struct SelectParser {

    Parser& parser;

    explicit SelectParser(Parser& parser) : parser(parser) {}

    auto parse_select_query(const std::vector<std::string>& query_elements) const -> void;

private:
    auto process_select_inner_join_query(
           const std::vector<std::string>& query_elements,
           const std::vector<std::string>& column_names
    ) const -> void;

    auto print_all_columns_for_inner_join(
        const std::string& left,
        const std::string& right
    ) const -> void;

    auto print_specific_columns_for_inner_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> void;

    auto process_select_left_join_query(
        const std::vector<std::string>& query_elements,
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

    auto process_select_right_join_query(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> void;

    auto print_all_columns_for_right_join(
        const std::string& left,
        const std::string& right
    ) const -> void;

    auto print_specific_columns_for_right_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> void;

    auto process_select_full_join_query(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> void;

    auto print_all_columns_for_full_join(
        const std::string& left,
        const std::string& right
    ) const -> void;

    auto print_specific_columns_for_full_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> void;

    auto print_appropriate_columns_for_where(
        const std::vector<std::string>& query_elements,
        std::vector<std::vector<std::string>>& flattened_results
    ) const -> void;

    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;

    static auto is_valid_select_join_query(
        int join_type_clause_index,
        int join_clause_index,
        int on_clause_index
    ) -> bool;

    [[nodiscard]] auto validate_tables_and_columns(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& query_elements
    ) const -> bool;

    static auto split_string_with_dot(const std::string& str) -> std::pair<std::string, std::string>;
};

