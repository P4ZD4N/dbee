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

    if (from_clause_index == -1)
        throw std::invalid_argument("Query with SELECT clause should contain FROM clause!");

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

        if (values_clause_index == -1)
            throw std::invalid_argument("Query with INSERT clause should contain VALUES clause!");

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

        if (column_clause_index == -1)
            throw std::invalid_argument("Query with ALTER clause should contain COLUMN clause!");

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
        } else throw std::invalid_argument("Query with ALTER clause should contain operation clause before COLUMN clause!");
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

                for (const auto& detail : column_details) {
                    auto column_name = std::string(detail.begin(), std::ranges::find(detail, '('));
                    auto column_type = std::string(std::ranges::find(detail, '(') + 1, std::ranges::find(detail, ')'));
                    auto column_constraints_string = std::string(std::ranges::find(detail, '[') + 1, std::ranges::find(detail, ']'));
                    auto constraints = std::vector<std::string>{};

                    auto pos = static_cast<size_t>(0);
                    while ((pos = column_constraints_string.find(',')) != std::string::npos) {
                        constraints.push_back(column_constraints_string.substr(0, pos));
                        column_constraints_string.erase(0, pos + 1);
                    }

                    if (!column_constraints_string.empty()) constraints.push_back(column_constraints_string);

                    column_names.push_back(column_name);
                    column_types.push_back(column_type);
                    column_constraints.push_back(strings_to_constraints(constraints));
                }

                database.value().create_table(
                    table_name,
                    column_names,
                    strings_to_column_types(column_types),
                    column_constraints);
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

