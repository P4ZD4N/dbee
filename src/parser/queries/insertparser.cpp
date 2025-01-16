#include "insertparser.h"

auto InsertParser::parse_insert_query(const std::vector<std::string>& query_elements) const -> void {

    if (!parser.is_database_selected()) return;

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

    parser.database->insert_data(table_name, cleaned_values);
}

auto InsertParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}