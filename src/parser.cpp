#include "parser.h"

#include <sstream>
#include <fmt/ranges.h>
#include <vector>
#include <algorithm>

auto Parser::parse_query(const std::string& query) -> void {

    auto ss = std::stringstream(query);
    auto query_element = std::string();
    auto query_elements = std::vector<std::string>{};

    while (ss >> query_element) {
        query_elements.push_back(query_element);
    }

    if (query_elements.at(0) == "SELECT") {
        const auto obligatory_from_clause = std::string("FROM");
        const auto from_clause_index = find_index(query_elements, obligatory_from_clause);

        if (from_clause_index == -1)
            throw std::invalid_argument("Query with SELECT clause should contain FROM clause!");

        const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);
        const auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.end());

        if (column_names.size() == 1 and column_names.at(0) == "*") {
            for (const auto& table_name : table_names) {
                auto table = database.tables.find(table_name)->second;
                fmt::println("{}", table.get_data());
            }
            return;
        }

        for (auto column_name : column_names) {
            std::erase(column_name, ',');

            for (auto table_name : table_names) {
                std::erase(table_name, ',');

                auto table = database.tables.find(table_name)->second;
                fmt::println("{}", table.get_data_from(column_name));
            }
        }
    } else if (query_elements.at(0) == "INSERT") {
        if (query_elements.at(1) == "INTO") {

            const auto obligatory_values_clause = std::string("VALUES");
            const auto values_clause_index = find_index(query_elements, obligatory_values_clause);

            if (values_clause_index == -1)
                throw std::invalid_argument("Query with INSERT clause should contain VALUES clause!");

            const auto table_name = query_elements.at(2);
            auto values = std::vector(query_elements.begin() + values_clause_index + 1, query_elements.end());

            for (auto& value : values) std::erase(value, ',');

            database.insert_data(table_name, values);
        } else {
            fmt::println("Query with INSERT clause should contain INTO clause!");
        }
    } else if (query_elements.at(0) == "ALTER") {
        if (query_elements.at(1) == "TABLE") {

            const auto obligatory_column_clause = std::string("COLUMN");
            const auto column_clause_index = find_index(query_elements, obligatory_column_clause);
            const auto operation_clause_index = column_clause_index - 1;
            const auto table_name = query_elements.at(2);

            if (column_clause_index == -1)
                throw std::invalid_argument("Query with ALTER clause should contain COLUMN clause!");

            if (query_elements.at(operation_clause_index) == "ADD") {
                const auto new_column_name = query_elements.at(column_clause_index + 1);
                const auto new_column_type = query_elements.at(column_clause_index + 2);
                const auto new_column_constraints = std::vector(
                    query_elements.begin() + column_clause_index + 3, query_elements.end());

                database.get_table_by_name(table_name).add_column(
                    new_column_name,
                    string_to_column_type(new_column_type),
                    strings_to_constraints(new_column_constraints));
            } else throw std::invalid_argument("Query with ALTER clause should contain operation clause before COLUMN clause!");
        } else {
            fmt::println("Query with ALTER clause should contain TABLE clause!");
        }
    } else {
        fmt::println("Unknown command: {}", query_elements.at(0));
    }
}

auto Parser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {

    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }

    return -1;
}

