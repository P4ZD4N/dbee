#include <fmt/ranges.h>

#include "table.h"
#include "../constraintchecker/constraintchecker.h"

auto Table::insert_row(const std::vector<std::string>& data) -> void {

    if (data.size() != column_names.size()) {
        fmt::println("Row size does not match the number of columns!");
        return;
    }

    if (!ConstraintChecker::check_data(data, *this)) return;

    for (int i = 0; i < column_names.size(); i++) {
        if (!validate_value(data.at(i), column_types.at(i), column_names.at(i))) return;
    }

    rows.push_back(data);

    fmt::println("Successfully inserted data into table: '{}'", name);
}

auto Table::get_all_data() const -> std::vector<std::vector<std::string> > {
    return rows;
}

auto Table::get_all_data_from(const std::vector<std::string>& column_names) const -> std::vector<std::vector<std::string>> {

    auto data = std::vector<std::vector<std::string>>{};

    for (auto row : rows) {
        auto data_from_row = std::vector<std::string>{};
        for (auto column_name : column_names) {
            std::erase(column_name, ',');

            if (const auto dot_pos = column_name.find('.'); dot_pos != std::string::npos) {
                const auto [tab, col] = split_string_with_dot(column_name);
                const auto column_index = find_index(this->column_names, col);

                if (tab != this->name || column_index == -1) {
                    data_from_row.emplace_back("");
                    continue;
                }

                data_from_row.push_back(row.at(column_index));
                continue;
            }

            const auto column_index = find_index(this->column_names, column_name);
            if (column_index == -1) {
                data_from_row.emplace_back("");
                continue;
            }

            data_from_row.push_back(row.at(column_index));
        }
        data.push_back(data_from_row);
    }

    return data;
}

auto Table::get_data_filtered_by_equality(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, int) {
        return value == condition_column_value;
    });
}

auto Table::get_data_filtered_by_inequality(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, int) {
        return value != condition_column_value;
    });
}

auto Table::get_data_filtered_by_greater_than(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string> > {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, const int index) {
        if (column_types.size() - 1 >= index) return compare_values(value, condition_column_value, column_types.at(index)) > 0;
        return false;
    });
}

auto Table::get_data_filtered_by_greater_than_or_equal(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, const int index) {
        if (column_types.size() - 1 >= index) return compare_values(value, condition_column_value, column_types.at(index)) >= 0;
        return false;
    });
}

auto Table::get_data_filtered_by_less_than(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, const int index) {
        if (column_types.size() - 1 >= index) return compare_values(value, condition_column_value, column_types.at(index)) < 0;
        return false;
    });
}

auto Table::get_data_filtered_by_less_than_or_equal(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, const int index) {
        if (column_types.size() - 1 >= index) return compare_values(value, condition_column_value, column_types.at(index)) <= 0;
        return false;
    });
}

auto Table::get_data_filtered_by_like(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& pattern
) const -> std::vector<std::vector<std::string>> {
    const auto& effective_column_names = column_names.size() == 1 && column_names.at(0) == "*" ?
        this->column_names :
        column_names;

    return filter_data(data, effective_column_names, condition_column_name, [&](const std::string& value, int) {
        return matches_pattern(value, pattern);
    });
}

auto Table::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }

    return -1;
}

auto Table::add_column(
    const std::string& column_name,
    const ColumnType& column_type,
    const std::vector<Constraint>& new_column_constraints
) -> void {

    const auto column_index = find_index(column_names, column_name);

    if (column_index != -1) {
        fmt::println("Column with name '{}' already exists in table with name: '{}'", column_name, name);
        return;
    }

    column_names.push_back(column_name);
    column_types.push_back(column_type);
    column_constraints.push_back(new_column_constraints);

    for (auto& row : rows) row.emplace_back("");

    fmt::println("Successfully added column with name: '{}' to table with name name: '{}'", column_name, name);
}

auto Table::remove_column(const std::string &column_name) -> void {

    const auto column_index = find_index(column_names, column_name);

    if (column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return;
    }

    column_names.erase(column_names.begin() + column_index);
    column_types.erase(column_types.begin() + column_index);
    column_constraints.erase(column_constraints.begin() + column_index);
    column_foreign_keys.erase(column_foreign_keys.begin() + column_index);

    for (auto& row : rows) row.erase(row.begin() + column_index);

    fmt::println("Successfully removed column with name: '{}' from table with name name: '{}'", column_name, name);
}

auto Table::update_all_rows(const std::string& column_name, const std::string& new_value) -> void {

    const auto column_index = find_index(column_names, column_name);

    if (column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return;
    }

    if (!validate_value(new_value, column_types.at(column_index), column_names.at(column_index))) {
        return;
    }

    if (std::ranges::find(column_constraints.at(column_index), Constraint::PRIMARY_KEY) != column_constraints.at(column_index).end() ||
        std::ranges::find(column_constraints.at(column_index), Constraint::UNIQUE) != column_constraints.at(column_index).end()) {
        fmt::println("Column with name '{}' has constraints, which protects against this operation!", column_name);
        return;
    }

    for (auto& row : rows) row[column_index] = new_value;

    fmt::println("Successfully updated all rows in table '{}'", name);
}

auto Table::update_specific_rows(
    const std::vector<std::vector<std::string>> &specific_rows,
    const std::vector<std::vector<std::string>> &columns_and_new_values
) -> void {
    auto rows_to_update = std::vector<int>{};

    for (auto i = 0; i < rows.size(); ++i) {
        for (const auto& specific_row : specific_rows) {
            if (rows[i] == specific_row) {
                rows_to_update.push_back(i);
                break;
            }
        }
    }

    for (const auto& column_and_new_value : columns_and_new_values) {
        const auto& column_name = column_and_new_value.at(0);
        const auto& new_value = column_and_new_value.at(2);
        const auto column_index = find_index(column_names, column_name);

        if (column_index == -1) {
            fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
            return;
        }

        if (!validate_value(new_value, column_types.at(column_index), column_names.at(column_index))) {
            continue;
        }

        if (std::ranges::find(column_constraints.at(column_index), Constraint::PRIMARY_KEY) != column_constraints.at(column_index).end() ||
            std::ranges::find(column_constraints.at(column_index), Constraint::UNIQUE) != column_constraints.at(column_index).end()) {
            fmt::println("Column with name '{}' has constraints, which protects against this operation!", column_name);
            continue;
            }

        for (const auto& row_index : rows_to_update) rows[row_index][column_index] = new_value;
    }

    fmt::println("Successfully updated specific rows in table '{}'", name);
}

auto Table::delete_all_rows() -> void {
    rows.clear();
    fmt::println("Successfully deleted all rows from table '{}'", name);
}

auto Table::delete_specific_rows(
    const std::vector<std::vector<std::string>> &specific_rows
) -> void {
    std::erase_if(rows, [&](const std::vector<std::string>& row) {
            return std::ranges::any_of(specific_rows, [&](const std::vector<std::string>& specific_row) {
                return row == specific_row;
            });
        }
    );

    fmt::println("Successfully deleted specific rows from table '{}'", name);
}

auto Table::compare_values(
    const std::string& value1,
    const std::string& value2,
    const ColumnType& type) -> int {

    if (type == ColumnType::INTEGER || type == ColumnType::FLOAT) {
        if (value1.empty() || value2.empty()) {
            return 0;
        }

        const double num1 = std::stod(value1);
        const double num2 = std::stod(value2);

        if (num1 > num2) return 1;
        if (num1 < num2) return -1;

        return 0;
    }

    if (type == ColumnType::TEXT) {

        if (value1 > value2) return 1;
        if (value1 < value2) return -1;

        return 0;
    }

    return 0;
}

auto Table::matches_pattern(const std::string& value, const std::string& pattern) -> bool {
    auto regex_pattern = std::regex_replace(pattern, std::regex(R"(\%)"), ".*");
    regex_pattern = std::regex_replace(regex_pattern, std::regex(R"(\_)"), ".");
    const auto regex = std::regex(regex_pattern);

    return std::regex_match(value, regex);
}

auto Table::validate_column_index_and_value(
    const std::string& column_name,
    const std::string& condition_column_name,
    const std::string& new_value
) const -> bool {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (column_index == -1 || condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return false;
    }

    if (!validate_value(new_value, column_types.at(column_index), column_names.at(column_index))) {
        return false;
    }

    return true;
}

auto Table::validate_constraints(
    const std::string& column_name,
    int column_index,
    const std::vector<std::string>& row,
    const std::string& new_value
) -> bool {

    if (std::ranges::find(column_constraints.at(column_index), Constraint::PRIMARY_KEY) != column_constraints.at(column_index).end()) {
        const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
            return other_row[column_index] == new_value;
        });

        if (duplicate_found) {
            fmt::println("Cannot update column '{}'! Value '{}' violates PRIMARY_KEY constraint!", column_name, new_value);
            return false;
        }
    }

    if (std::ranges::find(column_constraints.at(column_index), Constraint::UNIQUE) != column_constraints.at(column_index).end()) {
        const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
            return &other_row != &row && other_row[column_index] == new_value;
        });

        if (duplicate_found) {
            fmt::println("Cannot update column '{}'! Value '{}' violates UNIQUE constraint!", column_name, new_value);
            return false;
        }
    }

    return true;
}

auto Table::validate_value(
    const std::string& value,
    const ColumnType& column_type,
    const std::string& column_name
) -> bool {

    const std::regex integer_regex(R"(^[-+]?[0-9]+$)");
    const std::regex float_regex(R"(^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$)");

    switch (column_type) {
        case ColumnType::INTEGER: {
            if (!std::regex_match(value, integer_regex)) {
                fmt::println("Value '{}' in column '{}' is not a valid INTEGER!", value, column_name);
                return false;
            }
        } break;

        case ColumnType::FLOAT: {
            if (!std::regex_match(value, float_regex)) {
                fmt::println("Value '{}' in column '{}' is not a valid FLOAT!", value, column_name);
                return false;
            }
        } break;

        case ColumnType::TEXT:
            break;
    }

    return true;
}

auto Table::find_indices(const std::vector<std::string>& vec, const std::string& value) const -> std::vector<int> {
    auto indices = std::vector<int>{};

    for (int i = 0; i < vec.size(); ++i) {
        auto element = vec[i];
        std::erase(element, ',');

        if (const auto dot_pos = value.find('.'); dot_pos != std::string::npos) {
            if (element.find('.') != std::string::npos && element == value) indices.push_back(i);
            if (element.find('.') == std::string::npos) {
                const auto [left, right] = split_string_with_dot(value);
                if (left == this->name && right == element) indices.push_back(i);
            }
            continue;
        }

        const auto [left, right] = split_string_with_dot(element);

        if (right == value) indices.push_back(i);
    }

    return indices;
}

auto Table::filter_data(
    const std::vector<std::vector<std::string>> &data,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::function<bool(const std::string&, int)> &condition
) const -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) return {};

    for (const auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (condition(row[index], index)) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::split_string_with_dot(const std::string &str) -> std::pair<std::string, std::string> {
    const auto dot_pos = str.find('.');
    auto left = str.substr(0, dot_pos);
    auto right = str.substr(dot_pos + 1);

    return std::pair{left, right};
}
