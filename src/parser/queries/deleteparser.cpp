#include "deleteparser.h"

auto DeleteParser::parse_delete_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    if (query_elements.at(1) != "FROM") {
        fmt::println("Query with DELETE clause should contain FROM clause!");
        return;
    }

    const auto& table_name = query_elements.at(2);

    if (find_index(query_elements, "WHERE") == 3) {

        const auto& condition_column_name = query_elements.at(4);
        const auto& condition_column_value = query_elements.at(6);

        if (query_elements.at(5) == "=") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_equality(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == "<>" || query_elements.at(5) == "!=") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_inequality(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == ">") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_greater_than(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == ">=") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_greater_than_or_equal(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == "<") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_less_than(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == "<=") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_less_than_or_equal(
                condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(5) == "LIKE") {
            parser.database.value().get_table_by_name(table_name).delete_specific_rows_by_like(
                condition_column_name, condition_column_value);
            return;
        }

        fmt::println("Query with DELETE and WHERE clauses should contain comparison operator after column name in condition!");
        return;
    }

    parser.database.value().get_table_by_name(table_name).delete_all_rows();
}

auto DeleteParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}
