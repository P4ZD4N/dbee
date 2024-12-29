#pragma once

#include "../parser.h"

struct SelectParser {

    Parser& parser;

    explicit SelectParser(Parser& parser) : parser(parser) {}

    auto parse_select_query(const std::vector<std::string>& query_elements) const -> void;

private:
    [[nodiscard]] auto get_select_inner_join_result(
           const std::vector<std::string>& query_elements,
           const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_all_columns_for_inner_join(
        const std::string& left,
        const std::string& right
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_specific_columns_for_inner_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_select_left_join_result(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_all_columns_for_left_join(
        const std::string& left,
        const std::string& right
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_specific_columns_for_left_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_select_right_join_result(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_all_columns_for_right_join(
        const std::string& left,
        const std::string& right
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_specific_columns_for_right_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_select_full_join_result(
        const std::vector<std::string>& query_elements,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_all_columns_for_full_join(
        const std::string& left,
        const std::string& right
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_specific_columns_for_full_join(
        const std::string& left,
        const std::string& right,
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;

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

