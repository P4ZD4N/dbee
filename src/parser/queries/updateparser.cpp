#include "updateparser.h"

auto UpdateParser::parse_update_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    if (query_elements.at(2) != "SET") {
        fmt::println("Query with UPDATE clause should contain SET clause!");
        return;
    }

    if (query_elements.at(4) != "=") {
        fmt::println("Query with UPDATE clause should contain '=' sign after column name!");
        return;
    }

    const auto& table_name = query_elements.at(1);
    const auto& column_name = query_elements.at(3);
    const auto& new_value = query_elements.at(5);

    if (find_index(query_elements, "WHERE") == 6) {

        const auto& condition_column_name = query_elements.at(7);
        const auto& condition_column_value = query_elements.at(9);

        if (query_elements.at(8) == "=") {
            parser.database.value().get_table_by_name(table_name).update_specific_rows_by_equality(
                column_name, new_value, condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(8) == "<>" || query_elements.at(8) == "!=") {
            parser.database.value().get_table_by_name(table_name).update_specific_rows_by_inequality(
                column_name, new_value, condition_column_name, condition_column_value);
            return;
        }

        if (query_elements.at(8) != ">") {

        }

        if (query_elements.at(8) != ">=") {

        }

        if (query_elements.at(8) != "<") {

        }

        if (query_elements.at(8) != "<=") {

        }

        if (query_elements.at(8) != "LIKE") {

        }

        fmt::println("Query with UPDATE and WHERE clauses should contain comparison operator after column name in condition!");
        return;
    }

    parser.database.value().get_table_by_name(table_name).update_all_rows(column_name, new_value);
}

auto UpdateParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}