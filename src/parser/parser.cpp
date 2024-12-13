#include "../enums/columntype.h"
#include "../enums/constraint.h"
#include "parser.h"

#include <sstream>
#include <fmt/ranges.h>
#include <vector>
#include <algorithm>

auto Parser::parse_query(const std::string& query) -> void {

    auto ss = std::stringstream(query);
    auto query_element = std::string();
    auto query_elements = std::vector<std::string>{};

    while (ss >> query_element) query_elements.push_back(query_element);

    if (query_elements.at(0) == "DATABASE") process_database_query(query_elements);
    else if (query_elements.at(0) == "SELECT") process_select_query(query_elements);
    else if (query_elements.at(0) == "INSERT") process_insert_query(query_elements);
    else if (query_elements.at(0) == "ALTER") process_alter_query(query_elements);
    else if (query_elements.at(0) == "TABLE") process_table_query(query_elements);
    else fmt::println("Unknown command: {}", query_elements.at(0));
}

auto Parser::process_database_query(const std::vector<std::string>& query_elements) -> void {

    if (query_elements.at(1) == "CREATE") {
        const auto& database_name = query_elements.at(2);
        Database::create_database(database_name);
    } else if (query_elements.at(1) == "USE") {
        const auto& database_name = query_elements.at(2);
        database = Database::get_database(database_name);
    } else if (query_elements.at(1) == "DROP") {
        const auto& database_name = query_elements.at(2);
        Database::drop_database(database_name);
    } else fmt::println("Query with DATABASE clause should contain correct operation clause after DATABASE clause!");
}

auto Parser::process_select_query(const std::vector<std::string> &query_elements) const -> void {

    if (!is_database_selected()) return;

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto left_clause_index = find_index(query_elements, "LEFT");

    if (from_clause_index == -1) {
        fmt::println("Query with SELECT clause should contain FROM clause!");
        return;
    }

    const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);

    if (inner_clause_index != -1) {
        process_select_inner_join_query(query_elements, column_names);
        return;
    }

    if (left_clause_index != -1) {
        process_select_left_join_query(query_elements, column_names);
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.end());

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        for (const auto& table_name : table_names) {
            auto table = database.value().tables.find(table_name)->second;
            fmt::println("{}", table.get_data());
        }
        return;
    }

    for (auto column_name : column_names) {
        std::erase(column_name, ',');

        for (auto table_name : table_names) {
            std::erase(table_name, ',');

            auto table = database.value().tables.find(table_name)->second;
            fmt::println("{}", table.get_data_from(column_name));
        }
    }
}

auto Parser::process_select_inner_join_query(
    const std::vector<std::string>& query_elements,
    const std::vector<std::string>& column_names
) const -> void {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(inner_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and INNER clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  inner_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_inner_join(left, right);
        return;
    }

    print_specific_columns_for_inner_join(left, right, column_names);
}

auto Parser::is_valid_select_join_query(
    const int join_type_clause_index,
    const int join_clause_index,
    const int on_clause_index
) const -> bool {
    return join_clause_index == join_type_clause_index + 1 && on_clause_index == join_clause_index + 2;
}

auto Parser::validate_tables_and_columns(
    const std::string& left,
    const std::string& right,
    const std::vector<std::string>& query_elements
) const -> bool {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto join_clause_index = find_index(query_elements, "JOIN");

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    if (!database.value().tables.contains(left_table_name)) {
        fmt::println("Table with name '{}' not exists in database '{}'!", left_table_name, database.value().name);
        return false;
    }

    if (!database.value().tables.contains(right_table_name)) {
        fmt::println("Table with name '{}' not exists in database '{}'!", right_table_name, database.value().name);
    }

    if (left_table_name != query_elements.at(from_clause_index + 1)) {
        fmt::println("Left table name after ON clause not match table name after FROM clause!");
        return false;
    }

    if (right_table_name != query_elements.at(join_clause_index + 1)) {
        fmt::println("Right table name after ON clause not match table name after JOIN clause!");
        return false;
    }

    auto left_table = database.value().tables.find(left_table_name)->second;
    auto right_table = database.value().tables.find(right_table_name)->second;

    if (std::ranges::find(left_table.column_names, left_column_name) == left_table.column_names.end()) {
        fmt::println("'{}' column not exists in table '{}'!", left_column_name, left_table_name);
        return false;
    }

    if (std::ranges::find(right_table.column_names, right_column_name) == right_table.column_names.end()) {
        fmt::println("'{}' column not exists in table '{}'!", right_column_name, right_table_name);
        return false;
    }

    return true;
}

auto Parser::print_all_columns_for_inner_join(
    const std::string& left,
    const std::string& right
) const -> void {
    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = database.value().tables.find(left_table_name)->second;
    auto right_table = database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (auto left_table_data : left_table.get_data()) {

        auto combined_data = std::vector<std::string>{};

        for (auto right_table_data : right_table.get_data()) {

            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                for (const auto& element : left_table_data) combined_data.push_back(element);
                for (const auto& element : right_table_data) combined_data.push_back(element);
                data.push_back(combined_data);
                combined_data.clear();
            }
        }
    }

    fmt::println("{}", data);
}

auto Parser::print_specific_columns_for_inner_join(
    const std::string& left,
    const std::string& right,
    const std::vector<std::string>& column_names
) const -> void {
    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = database.value().tables.find(left_table_name)->second;
    auto right_table = database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& left_table_data : left_table.get_data()) {
        for (const auto& right_table_data : right_table.get_data()) {
            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                auto combined_data = std::vector<std::string>{};
                for (auto column_name : column_names) {
                    std::erase(column_name, ',');
                    if (const auto dot_pos = column_name.find('.'); dot_pos != std::string::npos) {
                        const auto [t_name, c_name] = split_string_with_dot(column_name);
                        if (t_name == left_table_name) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, c_name)]);
                        else if (t_name == right_table_name) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, c_name)]);
                    } else {
                        const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                        const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                        if (c_left_index != -1 && c_right_index != -1) {
                            fmt::println("Column with name '{}' exists in both tables!", column_name);
                            return;
                        }

                        if (c_left_index != -1) for (const auto& element : left_table_data) combined_data.push_back(element);
                        if (c_right_index != -1) for (const auto& element : right_table_data) combined_data.push_back(element);
                    }
                }
                data.push_back(combined_data);
            }
        }
    }

    fmt::println("{}", data);
}

auto Parser::process_select_left_join_query(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> void {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto left_clause_index = find_index(query_elements, "LEFT");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(left_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and LEFT clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  left_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_left_join(left, right);
        return;
    }

    print_specific_columns_for_left_join(left, right, column_names);
}

auto Parser::print_all_columns_for_left_join(
    const std::string& left,
    const std::string& right
) const -> void {
    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = database.value().tables.find(left_table_name)->second;
    auto right_table = database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (auto left_table_data : left_table.get_data()) {

        auto match_found = false;

        for (auto right_table_data : right_table.get_data()) {
            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                auto combined_data = std::vector<std::string>{};
                for (const auto& element : left_table_data) combined_data.push_back(element);
                for (const auto& element : right_table_data) combined_data.push_back(element);
                data.push_back(combined_data);
                combined_data.clear();
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (const auto& element : left_table_data) combined_data.push_back(element);
            for (auto i = 0; i < right_table.column_names.size(); i++) combined_data.emplace_back("");
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}

auto Parser::print_specific_columns_for_left_join(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &column_names
) const -> void {

    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = database.value().tables.find(left_table_name)->second;
    auto right_table = database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& left_table_data : left_table.get_data()) {

        auto match_found = false;

        for (const auto& right_table_data : right_table.get_data()) {
            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                auto combined_data = std::vector<std::string>{};
                for (auto column_name : column_names) {
                    std::erase(column_name, ',');
                    if (const auto dot_pos = column_name.find('.'); dot_pos != std::string::npos) {
                        const auto [t_name, c_name] = split_string_with_dot(column_name);
                        if (t_name == left_table_name) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, c_name)]);
                        else if (t_name == right_table_name) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, c_name)]);
                    } else {
                        const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                        const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                        if (c_left_index != -1 && c_right_index != -1) {
                            fmt::println("Column with name '{}' exists in both tables!", column_name);
                            return;
                        }

                        if (c_left_index != -1) for (const auto& element : left_table_data) combined_data.push_back(element);
                        if (c_right_index != -1) for (const auto& element : right_table_data) combined_data.push_back(element);
                    }
                }
                data.push_back(combined_data);
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (auto column_name : column_names) {
                std::erase(column_name, ',');
                if (const auto dot_pos = column_name.find('.'); dot_pos != std::string::npos) {
                    const auto [t_name, c_name] = split_string_with_dot(column_name);
                    if (t_name == left_table_name) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, c_name)]);
                } else {
                    const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                    const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                    if (c_left_index != -1 && c_right_index != -1) {
                        fmt::println("Column with name '{}' exists in both tables!", column_name);
                        return;
                    }

                    if (c_left_index != -1) for (const auto& element : left_table_data) combined_data.push_back(element);
                }
            }
            for (auto i = 0; i < column_names.size() - data.size(); i++) combined_data.emplace_back("");
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}


auto Parser::process_insert_query(const std::vector<std::string> &query_elements) -> void {

    if (!is_database_selected()) return;

    if (query_elements.at(1) != "INTO") {
        fmt::println("Query with INSERT clause should contain INTO clause!");
        return;
    }

    const auto values_clause_index = find_index(query_elements, "VALUES");

    if (values_clause_index == -1) {
        fmt::println("Query with INSERT clause should contain VALUES clause!");
        return;
    }

    const auto& table_name = query_elements.at(2);
    auto values = std::vector(query_elements.begin() + values_clause_index + 1, query_elements.end());
    auto cleaned_values = std::vector<std::string>{};

    auto value_with_more_parts = std::string();

    for (auto& value : values) {
        if (!value_with_more_parts.empty()) {
            value_with_more_parts.append(" " + value);

            if (value.back() == '\'') {
                value_with_more_parts = value_with_more_parts.substr(1, value_with_more_parts.length() - 2);
                cleaned_values.push_back(value_with_more_parts);
                value_with_more_parts.clear();
            }

            continue;
        }

        if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.length() - 2);
            cleaned_values.push_back(value);
        } else if (value.find('\'') != std::string::npos) {
            value_with_more_parts = value;
        } else {
            std::erase(value, ',');
            cleaned_values.push_back(value);
        }
    }

    database.value().insert_data(table_name, cleaned_values);
}


auto Parser::process_alter_query(const std::vector<std::string>& query_elements) -> void {

    if (!is_database_selected()) return;

    if (query_elements.at(1) == "TABLE") {

        const auto column_clause_index = find_index(query_elements, "COLUMN");
        const auto operation_clause_index = column_clause_index - 1;
        const auto& table_name = query_elements.at(2);

        if (column_clause_index == -1) {
            fmt::println("Query with ALTER clause should contain COLUMN clause!");
            return;
        }

        if (query_elements.at(operation_clause_index) == "ADD") {
            const auto& new_column_name = query_elements.at(column_clause_index + 1);
            const auto& new_column_type = query_elements.at(column_clause_index + 2);
            const auto new_column_constraints = std::vector(
                query_elements.begin() + column_clause_index + 3, query_elements.end());

            database.value().get_table_by_name(table_name).add_column(
                new_column_name,
                string_to_column_type(new_column_type),
                strings_to_constraints(new_column_constraints));
        } else if (query_elements.at(operation_clause_index) == "DROP") {
            const auto& column_to_remove_name = query_elements.at(column_clause_index + 1);
            database.value().get_table_by_name(table_name).remove_column(column_to_remove_name);
        } else fmt::println("Query with ALTER clause should contain operation clause before COLUMN clause!");
    } else fmt::println("Query with ALTER clause should contain TABLE clause!");
}


auto Parser::process_table_query(const std::vector<std::string> &query_elements) -> void {

        if (!is_database_selected()) return;

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

                        auto& foreign_table = database.value().get_table_by_name(foreign_table_name);
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

                database.value().create_table(
                    table_name,
                    column_names,
                    strings_to_column_types(column_types),
                    column_constraints,
                    column_foreign_keys);
            } else fmt::println("Query with TABLE CREATE clauses should contain WITH COLUMNS clauses after table name!");
        } else if (query_elements.at(1) == "DROP") {
            const auto& table_to_drop_name = query_elements.at(2);

            for (const auto& [table_from_db_name, table_from_db] : database.value().tables) {
                for (const auto& table_from_foreign_key : table_from_db.column_foreign_keys) {
                    if (table_from_foreign_key.first != nullptr && table_from_foreign_key.first->name == table_to_drop_name) {
                        fmt::println("Can't drop table '{}' because it contains column, which is foreign key of different column!", table_to_drop_name);
                        return;
                    }
                }
            }

            database.value().drop_table(table_to_drop_name);
        } else fmt::println("Query with TABLE clause should contain correct operation clause after TABLE clause!");
}

auto Parser::is_database_selected() const -> bool {
    if (!database.has_value()) {
        fmt::println("No database selected. Please select it with 'DATABASE USE [...]'!");
        return false;
    }
    return true;
}



auto Parser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}

auto Parser::get_foreign_key_from_curly_braces(const std::string &detail) -> std::string {
    const auto& start_pos = detail.find('{');
    const auto& end_pos = detail.find('}');
    if (start_pos != std::string::npos && end_pos != std::string::npos)
        return detail.substr(start_pos + 1, end_pos - start_pos - 1);
    return {};
}

auto Parser::is_foreign_key_valid(
    const std::string& foreign_table_name,
    const std::string& foreign_column_name,
    const std::string& column_type
) const -> bool {
    if (!database.value().tables.contains(foreign_table_name)) {
        fmt::println("Enter valid table name in a foreign key!");
        return false;
    }

    const auto foreign_table = database.value().tables.at(foreign_table_name);
    const auto column_found_in_foreign_table = std::ranges::find(foreign_table.column_names, foreign_column_name);

    if (column_found_in_foreign_table == foreign_table.column_names.end()) {
        fmt::println("Enter valid column name in a foreign key!");
        return false;
    }

    const auto column_found_index = Table::find_index(database.value().tables.at(foreign_table_name).column_names, foreign_column_name);
    const auto column_found_type = database.value().tables.at(foreign_table_name).column_types.at(column_found_index);

    if (column_found_type != string_to_column_type(column_type)) {
        fmt::println("Enter valid column type, which match foreign column type in a foreign key!");
        return false;
    }

    const auto column_found_constraints = database.value().tables.at(foreign_table_name).column_constraints.at(column_found_index);

    if (std::ranges::find(column_found_constraints, Constraint::PRIMARY_KEY) == column_found_constraints.end() &&
        std::ranges::find(column_found_constraints, Constraint::UNIQUE) == column_found_constraints.end()) {
        fmt::println("Column with 'FOREIGN_KEY' constraint must refer to column with 'UNIQUE' or 'PRIMARY_KEY' constraint!");
        return false;
    }

    return true;
}

auto Parser::split_string_with_dot(const std::string& str) const -> std::pair<std::string, std::string> {
    const auto dot_pos = str.find('.');
    auto left = str.substr(0, dot_pos);
    auto right = str.substr(dot_pos + 1);

    return std::pair{left, right};
}
