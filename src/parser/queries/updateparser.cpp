#include "updateparser.h"

#include "select/whereclauseparser.h"

auto UpdateParser::parse_update_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    const auto where_clause_parser = WhereClauseParser(parser);

    const auto set_clause_index = find_index(query_elements, "SET");
    const auto where_clause_index = find_index(query_elements, "WHERE");

    if (set_clause_index == -1 || set_clause_index != 2) {
        fmt::println("Query with UPDATE clause should contain SET clause on proper position!");
        return;
    }

    const auto& table_name = query_elements.at(1);
    auto columns_and_new_values = std::vector<std::vector<std::string>>{};
    for (auto it = query_elements.begin() + set_clause_index + 1;
        it + 2 < query_elements.end() && *it != "WHERE";
        it += 3
    ) {
        const auto& column_name = *it;
        const auto& equality_operator = *(it + 1);
        auto new_value = *(it + 2);

        std::erase(new_value, ',');
        if (new_value == "WHERE") break;

        columns_and_new_values.push_back({column_name, equality_operator, new_value});
    }

    if (where_clause_index == -1) {
        for (auto column_and_new_value : columns_and_new_values) {
            parser.database.value().get_table_by_name(table_name).update_all_rows(
                column_and_new_value.at(0), column_and_new_value.at(2));
        }
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

    auto results = std::vector<std::vector<std::string>>{};
    auto current_comparison_operator = std::string("");
    for (auto part : results_and_comparison_operators) {
        if (part.empty()) continue;
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

    parser.database.value().get_table_by_name(table_name).update_specific_rows(results, columns_and_new_values);
}

auto UpdateParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}