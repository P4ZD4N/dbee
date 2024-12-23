#include "alterparser.h"

auto AlterParser::parse_alter_query(const std::vector<std::string> &query_elements) const -> void {

    if (!parser.is_database_selected()) return;

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

            parser.database.value().get_table_by_name(table_name).add_column(
                new_column_name,
                string_to_column_type(new_column_type),
                strings_to_constraints(new_column_constraints));
        } else if (query_elements.at(operation_clause_index) == "DROP") {
            const auto& column_to_remove_name = query_elements.at(column_clause_index + 1);
            parser.database.value().get_table_by_name(table_name).remove_column(column_to_remove_name);
        } else fmt::println("Query with ALTER clause should contain operation clause before COLUMN clause!");
    } else fmt::println("Query with ALTER clause should contain TABLE clause!");
}

auto AlterParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}
