#include "tableparser.h"

auto TableParser::parse_table_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

        if (query_elements.at(1) == "CREATE") {
            if (query_elements.at(3) == "WITH" and query_elements.at(4) == "COLUMNS") {

                const auto column_details = std::vector(query_elements.begin() + 5, query_elements.end());

                const auto& table_name = query_elements.at(2);
                auto column_names = std::vector<std::string>{};
                auto column_types = std::vector<std::string>{};
                auto column_constraints = std::vector<std::vector<Constraint>>{};
                auto column_foreign_keys = std::vector<std::pair<Table*, std::string>>{};

                for (const auto& detail : column_details) {
                    auto column_name = std::string(detail.begin(), std::ranges::find(detail, '('));

                    if (std::ranges::find(column_names, column_name) != column_names.end()) {
                        fmt::println("Can't add two columns with name '{}'!", column_name);
                        return;
                    }

                    auto column_type = std::string(std::ranges::find(detail, '(') + 1, std::ranges::find(detail, ')'));
                    auto column_constraints_string = std::string(std::ranges::find(detail, '[') + 1, std::ranges::find(detail, ']'));
                    auto column_foreign_keys_string = get_foreign_key_from_curly_braces(detail);

                    if (!column_foreign_keys_string.empty()) {

                        const auto [foreign_table_name, foreign_column_name] = split_string_with_dot(column_foreign_keys_string);

                        if (!is_foreign_key_valid(foreign_table_name, foreign_column_name, column_type)) return;

                        auto& foreign_table = parser.database.value().get_table_by_name(foreign_table_name);
                        column_foreign_keys.emplace_back(&foreign_table, foreign_column_name);
                    } else column_foreign_keys.emplace_back(nullptr, "");

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

                    column_names.push_back(column_name);
                    column_types.push_back(column_type);
                    column_constraints.push_back(strings_to_constraints(constraints));
                }

                parser.database.value().create_table(
                    table_name,
                    column_names,
                    strings_to_column_types(column_types),
                    column_constraints,
                    column_foreign_keys);
            } else fmt::println("Query with TABLE CREATE clauses should contain WITH COLUMNS clauses after table name!");
        } else if (query_elements.at(1) == "DROP") {
            const auto& table_to_drop_name = query_elements.at(2);

            for (const auto& [table_from_db_name, table_from_db] : parser.database.value().tables) {
                for (const auto& table_from_foreign_key : table_from_db.column_foreign_keys) {
                    if (table_from_foreign_key.first != nullptr && table_from_foreign_key.first->name == table_to_drop_name) {
                        fmt::println("Can't drop table '{}' because it contains column, which is foreign key of different column!", table_to_drop_name);
                        return;
                    }
                }
            }

            parser.database.value().drop_table(table_to_drop_name);
        } else fmt::println("Query with TABLE clause should contain correct operation clause after TABLE clause!");
}

auto TableParser::get_foreign_key_from_curly_braces(const std::string& detail) -> std::string {
    const auto& start_pos = detail.find('{');
    const auto& end_pos = detail.find('}');
    if (start_pos != std::string::npos && end_pos != std::string::npos)
        return detail.substr(start_pos + 1, end_pos - start_pos - 1);
    return {};
}

auto TableParser::is_foreign_key_valid(
    const std::string& foreign_table_name,
    const std::string& foreign_column_name,
    const std::string& column_type
) const -> bool {
    if (!parser.database.value().tables.contains(foreign_table_name)) {
        fmt::println("Enter valid table name in a foreign key!");
        return false;
    }

    const auto foreign_table = parser.database.value().tables.at(foreign_table_name);
    const auto column_found_in_foreign_table = std::ranges::find(foreign_table.column_names, foreign_column_name);

    if (column_found_in_foreign_table == foreign_table.column_names.end()) {
        fmt::println("Enter valid column name in a foreign key!");
        return false;
    }

    const auto column_found_index = Table::find_index(parser.database.value().tables.at(foreign_table_name).column_names, foreign_column_name);
    const auto column_found_type = parser.database.value().tables.at(foreign_table_name).column_types.at(column_found_index);

    if (column_found_type != string_to_column_type(column_type)) {
        fmt::println("Enter valid column type, which match foreign column type in a foreign key!");
        return false;
    }

    const auto column_found_constraints = parser.database.value().tables.at(foreign_table_name).column_constraints.at(column_found_index);

    if (std::ranges::find(column_found_constraints, Constraint::PRIMARY_KEY) == column_found_constraints.end() &&
        std::ranges::find(column_found_constraints, Constraint::UNIQUE) == column_found_constraints.end()) {
        fmt::println("Column with 'FOREIGN_KEY' constraint must refer to column with 'UNIQUE' or 'PRIMARY_KEY' constraint!");
        return false;
        }

    return true;
}

auto TableParser::split_string_with_dot(const std::string &str) -> std::pair<std::string, std::string> {
    const auto dot_pos = str.find('.');
    auto left = str.substr(0, dot_pos);
    auto right = str.substr(dot_pos + 1);

    return std::pair{left, right};
}


