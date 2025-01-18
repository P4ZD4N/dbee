#include "deleteparser.h"

#include "select/whereclauseparser.h"

auto DeleteParser::parse_delete_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    const auto where_clause_parser = WhereClauseParser(parser);
    const auto where_clause_index = find_index(query_elements, "WHERE");

    if (query_elements.at(1) != "FROM") {
        fmt::println("Query with DELETE clause should contain FROM clause!");
        return;
    }

    const auto& table_name = query_elements.at(2);

    if (where_clause_index == -1) {
        if (!parser.database->tables.contains(table_name)) {
            fmt::println("Table with name '{}' does not exist in database with name: '{}'!", table_name, parser.database->name);
            return;
        }

        parser.database->get_table_by_name(table_name).delete_all_rows();
        return;
    }

    if (where_clause_index != 3) {
        fmt::println("Query with DELETE clause should contain WHERE clause on proper position!");
        return;
    }

    auto results_and_comparison_operators = std::vector<std::vector<std::vector<std::string>>>{};
    for (auto it = query_elements.begin() + where_clause_index + 1; it + 2 < query_elements.end(); it += 4) {
        const auto& condition_column_name = *it;
        const auto& comparison_operator = *(it + 1);
        const auto& condition_column_value = *(it + 2);

        if (comparison_operator == "=") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_equality(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<>" || comparison_operator == "!=") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_inequality(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == ">") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_greater_than(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == ">=") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_greater_than_or_equal(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_less_than(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<=") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_less_than_or_equal(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "LIKE") {
            results_and_comparison_operators.push_back(
                where_clause_parser.get_data_filtered_by_like(
                    {table_name}, {"*"}, condition_column_name, condition_column_value, false, {}));
        }

        if (it + 3 < query_elements.end()) results_and_comparison_operators.push_back({{*(it + 3)}});
    }

    if (results_and_comparison_operators.size() == 1 && results_and_comparison_operators.at(0).empty()) {
        fmt::println("No rows were deleted because no records met the specified criteria");
        return;
    }

    auto results = std::vector<std::vector<std::string>>{};
    auto current_comparison_operator = std::string("");
    for (auto part : results_and_comparison_operators) {

        if (part.empty() && (current_comparison_operator == "&&" || current_comparison_operator == "AND")) {
            results.clear();
            current_comparison_operator.clear();
            continue;
        }

        if (part.empty() && (current_comparison_operator == "||" || current_comparison_operator == "OR")) continue;

        if (part.at(0).size() == 1 && (
                part.at(0).at(0) == "&&" ||
                part.at(0).at(0) == "||" ||
                part.at(0).at(0) == "AND" ||
                part.at(0).at(0) == "OR")
            ) {
            current_comparison_operator = part.at(0).at(0);
            continue;
        }

        for (auto& row : part) {
            if (current_comparison_operator.empty()) results.push_back(row);
            else {
                if (current_comparison_operator == "&&" || current_comparison_operator == "AND") {
                    auto temp_results = std::vector<std::vector<std::string>>{};

                    for (const auto& existing_row : results) {
                        if (std::ranges::find(part, existing_row) != part.end()) temp_results.push_back(existing_row);
                    }

                    results = temp_results;

                    if (results.empty()) break;
                }

                if (current_comparison_operator == "||" || current_comparison_operator == "OR") {
                    if (std::ranges::find(results, row) == results.end()) results.push_back(row);
                }
            }
        }

        current_comparison_operator.clear();
    }

    parser.database->get_table_by_name(table_name).delete_specific_rows(results);
}

auto DeleteParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}
