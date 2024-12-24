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

auto Table::get_data() const -> std::vector<std::vector<std::string> > {
    return rows;
}

auto Table::get_data_from(const std::string& column_name) const -> std::vector<std::string> {

    const auto column_index = find_index(column_names, column_name);

    if (column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return {};
    }

    auto data = std::vector<std::string>{};

    for (auto row : rows) {
        data.push_back(row.at(column_index));
    }

    return data;
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

    fmt::println("Successfully updated column with name: '{}' (swapped all values with '{}')", column_name, new_value);
}

auto Table::update_specific_rows_by_equality(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {

    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (column_index == -1 || condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return;
    }

    if (!validate_value(new_value, column_types.at(column_index), column_names.at(column_index))) {
        return;
    }

    for (auto& row : rows) {
        if (row[condition_column_index] == condition_column_value) {
            if (std::ranges::find(column_constraints.at(column_index), Constraint::PRIMARY_KEY) != column_constraints.at(column_index).end()) {
                const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
                    return other_row[column_index] == new_value;
                });

                if (duplicate_found) {
                    fmt::println("Cannot update column '{}'! Value '{}' violates PRIMARY_KEY constraint!", column_name, new_value);
                    return;
                }
            }

            if (std::ranges::find(column_constraints.at(column_index), Constraint::UNIQUE) != column_constraints.at(column_index).end()) {
                const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
                    return &other_row != &row && other_row[column_index] == new_value;
                });

                if (duplicate_found) {
                    fmt::println("Cannot update column '{}'! Value '{}' violates UNIQUE constraint!", column_name, new_value);
                    return;
                }
            }

            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated column with name: '{}' (swapped matching values with '{}')", column_name, new_value);
}

auto Table::update_specific_rows_by_inequality(
    const std::string& column_name,
    const std::string& new_value,
    const std::string& condition_column_name,
    const std::string& condition_column_value
) -> void {
    const auto column_index = find_index(column_names, column_name);
    const auto condition_column_index = find_index(column_names, condition_column_name);

    if (column_index == -1 || condition_column_index == -1) {
        fmt::println("Column with name '{}' not found in table with name: '{}'", column_name, name);
        return;
    }

    if (!validate_value(new_value, column_types.at(column_index), column_names.at(column_index))) {
        return;
    }

    for (auto& row : rows) {
        if (row[condition_column_index] != condition_column_value) {
            if (std::ranges::find(column_constraints.at(column_index), Constraint::PRIMARY_KEY) != column_constraints.at(column_index).end()) {
                const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
                    return other_row[column_index] == new_value;
                });

                if (duplicate_found) {
                    fmt::println("Cannot update column '{}'! Value '{}' violates PRIMARY_KEY constraint!", column_name, new_value);
                    return;
                }
            }

            if (std::ranges::find(column_constraints.at(column_index), Constraint::UNIQUE) != column_constraints.at(column_index).end()) {
                const auto duplicate_found = std::ranges::any_of(rows, [&](const auto& other_row) {
                    return &other_row != &row && other_row[column_index] == new_value;
                });

                if (duplicate_found) {
                    fmt::println("Cannot update column '{}'! Value '{}' violates UNIQUE constraint!", column_name, new_value);
                    return;
                }
            }

            row[column_index] = new_value;
        }
    }

    fmt::println("Successfully updated column with name: '{}' (swapped matching values with '{}')", column_name, new_value);
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
