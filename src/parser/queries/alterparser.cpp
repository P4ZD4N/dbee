#include "alterparser.h"

auto AlterParser::parse_alter_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    if (query_elements.at(1) != "TABLE") {
        fmt::println("Query with ALTER clause should contain TABLE clause!");
        return;
    }

    const auto column_clause_index = find_index(query_elements, "COLUMN");
    const auto operation_clause_index = column_clause_index - 1;
    const auto& table_name = query_elements.at(2);

    if (!parser.database->tables.contains(table_name)) {
        fmt::println("Table with name '{}' does not exist in database with name: '{}'!", table_name, parser.database->name);
        return;
    }

    if (column_clause_index == -1 || column_clause_index != 4) {
        fmt::println("Query with ALTER clause should contain COLUMN clause on proper position!");
        return;
    }

    if (query_elements.at(operation_clause_index) == "ADD") {
        if (query_elements.size() != 6) {
            fmt::println("Query with ALTER clause and ADD operation should contain proper new column details!");
            return;
        }

        const auto& new_column_details = query_elements.at(5);

        if (new_column_details.find('(') == std::string::npos &&
            new_column_details.find(')') == std::string::npos &&
            new_column_details.find('[') == std::string::npos &&
            new_column_details.find(']') == std::string::npos
        ) {
            fmt::println("Invalid format of new column: {}", new_column_details);
            return;
        }

        const auto column_name = std::string(new_column_details.begin(), std::ranges::find(new_column_details, '('));
        const auto column_type = std::string(std::ranges::find(new_column_details, '(') + 1, std::ranges::find(new_column_details, ')'));
        auto column_constraints = std::vector<std::vector<Constraint>>{};
        auto column_foreign_key = std::pair<Table*, std::string>{};
        auto column_constraints_string = std::string(std::ranges::find(new_column_details, '[') + 1, std::ranges::find(new_column_details, ']'));
        auto column_foreign_key_string = get_foreign_key_from_curly_braces(new_column_details);

        if (!column_foreign_key_string.empty()) {
            const auto [foreign_table_name, foreign_column_name] = split_string_with_dot(column_foreign_key_string);

            if (!is_foreign_key_valid(foreign_table_name, foreign_column_name, column_type)) return;

            auto& foreign_table = parser.database->get_table_by_name(foreign_table_name);
            column_foreign_key = std::pair(&foreign_table, foreign_column_name);
        } else column_foreign_key = std::pair<Table*, std::string>(nullptr, "");

        auto constraints = std::vector<std::string>{};

        auto pos = static_cast<size_t>(0);
        while ((pos = column_constraints_string.find(',')) != std::string::npos) {
            constraints.push_back(column_constraints_string.substr(0, pos));
            column_constraints_string.erase(0, pos + 1);
        }

        if (!column_constraints_string.empty()) constraints.push_back(column_constraints_string);

        for (auto& constraint : constraints) {
            auto open_brace_pos = constraint.find('{');
            if (open_brace_pos != std::string::npos) constraint.erase(open_brace_pos, constraint.back());
        }

        column_constraints.push_back(strings_to_constraints(constraints));

        if (!constraints.empty() && strings_to_constraints(constraints).empty()) return;
        if (!constraints.empty() && string_to_column_type(column_type) == ColumnType::INVALID) return;

        parser.database->get_table_by_name(table_name).add_column(
            column_name,
            string_to_column_type(column_type),
            strings_to_constraints(constraints),
            column_foreign_key);
    } else if (query_elements.at(operation_clause_index) == "DROP") {
        const auto& column_to_remove_name = query_elements.at(column_clause_index + 1);
        parser.database->get_table_by_name(table_name).remove_column(column_to_remove_name);
    } else fmt::println("Query with ALTER clause should contain operation clause before COLUMN clause!");
}

auto AlterParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}

auto AlterParser::get_foreign_key_from_curly_braces(const std::string &detail) -> std::string {
    const auto& start_pos = detail.find('{');
    const auto& end_pos = detail.find('}');
    if (start_pos != std::string::npos && end_pos != std::string::npos)
        return detail.substr(start_pos + 1, end_pos - start_pos - 1);
    return {};
}

auto AlterParser::split_string_with_dot(const std::string &str) -> std::pair<std::string, std::string> {
    const auto dot_pos = str.find('.');
    auto left = str.substr(0, dot_pos);
    auto right = str.substr(dot_pos + 1);

    return std::pair{left, right};
}

auto AlterParser::is_foreign_key_valid(
    const std::string &foreign_table_name,
    const std::string &foreign_column_name,
    const std::string &column_type
) const -> bool {
    if (!parser.database->tables.contains(foreign_table_name)) {
        fmt::println("Enter valid table name in a foreign key!");
        return false;
    }

    const auto foreign_table = parser.database->tables.at(foreign_table_name);
    const auto column_found_in_foreign_table = std::ranges::find(foreign_table.column_names, foreign_column_name);

    if (column_found_in_foreign_table == foreign_table.column_names.end()) {
        fmt::println("Enter valid column name in a foreign key!");
        return false;
    }

    const auto column_found_index = Table::find_index(parser.database->tables.at(foreign_table_name).column_names, foreign_column_name);
    const auto column_found_type = parser.database->tables.at(foreign_table_name).column_types.at(column_found_index);

    if (column_found_type != string_to_column_type(column_type)) {
        fmt::println("Enter valid column type, which match foreign column type in a foreign key!");
        return false;
    }

    const auto column_found_constraints = parser.database->tables.at(foreign_table_name).column_constraints.at(column_found_index);

    if (std::ranges::find(column_found_constraints, Constraint::PRIMARY_KEY) == column_found_constraints.end() &&
        std::ranges::find(column_found_constraints, Constraint::UNIQUE) == column_found_constraints.end()) {
        fmt::println("Column with 'FOREIGN_KEY' constraint must refer to column with 'UNIQUE' or 'PRIMARY_KEY' constraint!");
        return false;
        }

    return true;
}

