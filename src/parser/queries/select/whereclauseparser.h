#pragma once

#include <string>
#include <vector>

#include "../../parser.h"

struct WhereClauseParser {

    Parser& parser;

    explicit WhereClauseParser(Parser& parser) : parser(parser) {}

    [[nodiscard]] auto get_data_filtered_by_equality(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_inequality(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_greater_than(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_greater_than_or_equal(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_less_than(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_less_than_or_equal(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

    [[nodiscard]] auto get_data_filtered_by_like(
        const std::vector<std::string>& table_names,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value,
        bool is_select_with_join,
        const std::vector<std::vector<std::string>>& select_results
    ) const -> std::vector<std::vector<std::string>>;

private:
    [[nodiscard]] auto get_column_names_from_both(
        const std::vector<std::string>& table_names
    ) const -> std::vector<std::string>;
};
