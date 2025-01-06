#pragma once

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "../enums/columntype.h"
#include "../enums/constraint.h"

class Table {
public:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<ColumnType> column_types;
    std::vector<std::vector<Constraint>> column_constraints;
    std::vector<std::pair<Table*, std::string>> column_foreign_keys;
    std::vector<std::vector<std::string>> rows;

    Table() = default;

    Table(const std::string& name,
          const std::vector<std::string>& column_names,
          const std::vector<ColumnType>& column_types,
          const std::vector<std::vector<Constraint>>& column_constraints,
          const std::vector<std::pair<Table*, std::string>>& column_foreign_keys)
        : name(name),
          column_names(column_names),
          column_types(column_types),
          column_constraints(column_constraints),
          column_foreign_keys(column_foreign_keys) {

        if (column_names.size() != column_types.size())
            throw std::invalid_argument("Number of column names and column types must be equal!");

        if (column_names.size() != column_constraints.size())
            throw std::invalid_argument("Number of column names and column constraints must be equal!");
    }

    auto insert_row(const std::vector<std::string>& data) -> void;
    [[nodiscard]] auto get_all_data() const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_all_data_from(
        const std::vector<std::string>& column_names
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_equality(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_inequality(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_greater_than(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_greater_than_or_equal(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_less_than(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_less_than_or_equal(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_filtered_by_like(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::string& pattern
    ) const -> std::vector<std::vector<std::string>>;
    auto add_column(const std::string& column_name, const ColumnType& column_type, const std::vector<Constraint>& new_column_constraints) -> void;
    auto remove_column(const std::string& column_name) -> void;
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
    auto update_all_rows(const std::string& column_name, const std::string& new_value) -> void;
    auto update_specific_rows(
        const std::vector<std::vector<std::string>>& specific_rows,
        const std::vector<std::vector<std::string>>& columns_and_new_values
    ) -> void;
    auto delete_all_rows() -> void;
    auto delete_specific_rows_by_equality(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_inequality(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_greater_than(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_greater_than_or_equal(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_less_than(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_less_than_or_equal(
        const std::string& condition_column_name,
        const std::string& condition_column_value
    ) -> void;
    auto delete_specific_rows_by_like(
        const std::string& condition_column_name,
        const std::string& pattern
    ) -> void;
private:
    static auto compare_values(
        const std::string& value1,
        const std::string& value2,
        const ColumnType& type
    ) -> int;
    static auto matches_pattern(
        const std::string& value,
        const std::string& pattern
    ) -> bool;
    [[nodiscard]] auto validate_column_index_and_value(
        const std::string& column_name,
        const std::string& condition_column_name,
        const std::string& new_value
    ) const -> bool;
    auto validate_constraints(
        const std::string& column_name,
        int column_index,
        const std::vector<std::string>& row,
        const std::string& new_value
    ) -> bool;
    static auto validate_value(const std::string& value, const ColumnType& column_type, const std::string& column_name) -> bool;
    [[nodiscard]] auto find_indices(const std::vector<std::string>& vec, const std::string& value) const -> std::vector<int>;
    auto filter_data(
        const std::vector<std::vector<std::string>>& data,
        const std::vector<std::string>& column_names,
        const std::string& condition_column_name,
        const std::function<bool(const std::string&, int)>& condition
    ) const -> std::vector<std::vector<std::string>>;
    auto update_rows_by_condition(
        const std::string& column_name,
        const std::string& new_value,
        const std::string& condition_column_name,
        const std::function<bool(const std::string&, int)>& condition
    ) -> void;
    auto delete_rows_by_condition(
        const std::string& condition_column_name,
        const std::function<bool(const std::string&, int)>& condition
    ) -> void;
    static auto split_string_with_dot(const std::string &str) -> std::pair<std::string, std::string>;
};

