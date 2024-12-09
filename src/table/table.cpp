#include <fmt/ranges.h>

#include "table.h"
#include "../constraintchecker/constraintchecker.h"

auto Table::insert_row(const std::vector<std::string>& data) -> void {

    if (data.size() != column_names.size()) {
        fmt::println("Row size does not match the number of columns!");
        return;
    }

    if (!ConstraintChecker::check_data(data, *this)) return;

    const std::regex integer_regex(R"(^[-+]?[0-9]+$)");
    const std::regex float_regex(R"(^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$)");

    for (int i = 0; i < column_names.size(); i++) {

        switch (column_types.at(i)) {

            case ColumnType::INTEGER: {
                if (!std::regex_match(data.at(i), integer_regex))
                    fmt::println("Value '{}' in column '{}' is not a valid INTEGER!", data.at(i), column_names.at(i));
                break;
            }

            case ColumnType::FLOAT: {
                if (!std::regex_match(data.at(i), float_regex))
                    fmt::println("Value '{}' in column '{}' is not a valid FLOAT!", data.at(i), column_names.at(i));
                break;
            }

            case ColumnType::TEXT: break;
        }
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

    for (auto &row : rows) {
        row.erase(row.begin() + column_index);
    }

    fmt::println("Successfully removed column with name: '{}' from table with name name: '{}'", column_name, name);
}