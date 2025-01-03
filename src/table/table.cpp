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

    for (auto column_name : column_names) {
        std::erase(column_name, ',');
        const auto column_index = find_index(this->column_names, column_name);

        if (column_index == -1) {
            fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
            return {};
        }
    }

    auto data = std::vector<std::vector<std::string>>{};

    for (auto row : rows) {
        auto data_from_row = std::vector<std::string>{};
        for (auto column_name : column_names) {
            std::erase(column_name, ',');
            data_from_row.push_back(row.at(find_index(this->column_names, column_name)));
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
) -> std::vector<std::vector<std::string>> {

    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (row[index] == condition_column_value) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_inequality(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (row[index] != condition_column_value) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_greater_than(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> std::vector<std::vector<std::string> > {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (compare_values(row[index], condition_column_value, column_types.at(index)) > 0) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_greater_than_or_equal(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (compare_values(row[index], condition_column_value, column_types.at(index)) >= 0) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_less_than(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (compare_values(row[index], condition_column_value, column_types.at(index)) < 0) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_less_than_or_equal(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (compare_values(row[index], condition_column_value, column_types.at(index)) <= 0) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
}

auto Table::get_data_filtered_by_like(
    const std::vector<std::vector<std::string>>& data,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& pattern
) -> std::vector<std::vector<std::string>> {
    auto filtered_data = std::vector<std::vector<std::string>>{};
    const auto condition_column_indices = find_indices(column_names, condition_column_name);

    if (condition_column_indices.empty()) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return data;
    }

    for (auto& row : data) {
        for (const auto& index : condition_column_indices) {
            if (matches_pattern(row[index], pattern)) {
                filtered_data.push_back(row);
                break;
            }
        }
    }

    return filtered_data;
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

auto Table::update_specific_rows_by_equality(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (row[condition_column_index] == condition_column_value) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully deleted rows where column '{}' equals '{}'", condition_column_name, condition_column_value);
}

auto Table::update_specific_rows_by_inequality(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (row[condition_column_index] != condition_column_value) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully deleted rows where column '{}' not equals '{}'", condition_column_name, condition_column_value);
}

auto Table::update_specific_rows_by_greater_than(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (compare_values(row[condition_column_index], condition_column_value, column_types.at(condition_column_index)) > 0) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully deleted rows where column '{}' greater than '{}'", condition_column_name, condition_column_value);
}

auto Table::update_specific_rows_by_greater_than_or_equal(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (compare_values(row[condition_column_index], condition_column_value, column_types.at(condition_column_index)) >= 0) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated rows where column '{}' greater than or equals '{}'", condition_column_name, condition_column_value);
}


auto Table::update_specific_rows_by_less_than(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {

        if (compare_values(row[condition_column_index], condition_column_value, column_types.at(condition_column_index)) < 0) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated rows where column '{}' less than '{}'", condition_column_name, condition_column_value);
}

auto Table::update_specific_rows_by_less_than_or_equal(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (compare_values(row[condition_column_index], condition_column_value, column_types.at(condition_column_index)) <= 0) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated rows where column '{}' less than or equals '{}'", condition_column_name, condition_column_value);
}

auto Table::update_specific_rows_by_like(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& pattern
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (!validate_column_index_and_value(column_name, condition_column_name, new_value)) return;

    for (auto& row : rows) {
        if (matches_pattern(row[condition_column_index], pattern)) {
            if (!validate_constraints(column_name, column_index, row, new_value)) continue;
            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated rows where column '{}' matches '{}' pattern", condition_column_name, pattern);
}

auto Table::delete_all_rows() -> void {

    rows.clear();

    fmt::println("Successfully deleted all rows from table '{}'", name);
}

auto Table::delete_specific_rows_by_equality(
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if ((*it)[condition_column_index] == condition_column_value) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' equals '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_inequality(
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if ((*it)[condition_column_index] != condition_column_value) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' not equals '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_greater_than(
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if (compare_values((*it)[condition_column_index], condition_column_value, column_types.at(condition_column_index)) > 0) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' greater than '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_greater_than_or_equal(
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if (compare_values((*it)[condition_column_index], condition_column_value, column_types.at(condition_column_index)) >= 0) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' greater than or equals '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_less_than(const std::string &condition_column_name, const std::string &condition_column_value) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if (compare_values((*it)[condition_column_index], condition_column_value, column_types.at(condition_column_index)) < 0) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' less than '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_less_than_or_equal(
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if (compare_values((*it)[condition_column_index], condition_column_value, column_types.at(condition_column_index)) <= 0) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' less than or equals '{}'", condition_column_name, condition_column_value);
}

auto Table::delete_specific_rows_by_like(
    const std::string& condition_column_name,
    const std::string& pattern
) -> void {
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", condition_column_name, name);
        return;
    }

    for (auto it = rows.begin(); it != rows.end(); ) {
        if (matches_pattern((*it)[condition_column_index], pattern)) {
            it = rows.erase(it);
        } else ++it;
    }

    fmt::println("Successfully deleted rows where column '{}' matches '{}' pattern", condition_column_name, pattern);
}

auto Table::compare_values(
    const std::string& value1,
    const std::string& value2,
    const ColumnType& type) -> int {

    if (type == ColumnType::INTEGER || type == ColumnType::FLOAT) {

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

auto Table::find_indices(const std::vector<std::string>& vec, const std::string& value) -> std::vector<int> {
    auto indices = std::vector<int>{};

    for (int i = 0; i < vec.size(); ++i) {
        auto element = vec[i];
        std::erase(element, ',');

        if (const auto dot_pos = value.find('.'); dot_pos != std::string::npos) {
            if (element == value) indices.push_back(i);
            continue;
        }

        const auto dot_pos = element.find('.');
        auto left = element.substr(0, dot_pos);
        auto right = element.substr(dot_pos + 1);

        if (right == value) indices.push_back(i);
    }

    return indices;
}