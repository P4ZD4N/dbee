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

    const auto obligatory_from_clause = std::string("FROM");
    const auto from_clause_index = find_index(query_elements, obligatory_from_clause);

    if (from_clause_index == -1) {
        fmt::println("Query with SELECT clause should contain FROM clause!");
        return;
    }

    const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);
    const auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.end());

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

auto Parser::process_insert_query(const std::vector<std::string> &query_elements) -> void {

    if (!is_database_selected()) return;

    if (query_elements.at(1) == "INTO") {

        const auto obligatory_values_clause = std::string("VALUES");
        const auto values_clause_index = find_index(query_elements, obligatory_values_clause);

        if (values_clause_index == -1) {
            fmt::println("Query with INSERT clause should contain VALUES clause!");
            return;
        }

        const auto& table_name = query_elements.at(2);
        auto values = std::vector(query_elements.begin() + values_clause_index + 1, query_elements.end());

        for (auto& value : values) std::erase(value, ',');

        database.value().insert_data(table_name, values);
    } else fmt::println("Query with INSERT clause should contain INTO clause!");
}


auto Parser::process_alter_query(const std::vector<std::string>& query_elements) -> void {

    if (!is_database_selected()) return;

    if (query_elements.at(1) == "TABLE") {

        const auto obligatory_column_clause = std::string("COLUMN");
        const auto column_clause_index = find_index(query_elements, obligatory_column_clause);
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

                        const auto dot_pos = column_foreign_keys_string.find('.');
                        auto foreign_table_name = column_foreign_keys_string.substr(0, dot_pos);
                        auto foreign_column_name = column_foreign_keys_string.substr(dot_pos + 1);

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
            const auto& table_name = query_elements.at(2);
            database.value().drop_table(table_name);
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




