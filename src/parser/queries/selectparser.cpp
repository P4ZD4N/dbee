#include "selectparser.h"

#include <set>

auto SelectParser::parse_select_query(const std::vector<std::string>& query_elements) const -> void {

    if (!parser.is_database_selected()) return;

    auto database = parser.database;

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto left_clause_index = find_index(query_elements, "LEFT");
    const auto right_clause_index = find_index(query_elements, "RIGHT");
    const auto full_clause_index = find_index(query_elements, "FULL");

    if (from_clause_index == -1) {
        fmt::println("Query with SELECT clause should contain FROM clause!");
        return;
    }

    const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);

    if (inner_clause_index != -1 || left_clause_index != -1 || right_clause_index != -1 || full_clause_index != -1) {
        print_appropriate_columns_with_joins(query_elements, column_names);
        return;
    }

    print_appropriate_columns_without_joins(query_elements);
}

auto SelectParser::get_select_inner_join_result(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(inner_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and INNER clause should contain JOIN and ON clauses!");
        return {};
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  inner_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return {};

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        return get_all_columns_for_join(left, right, JoinType::INNER);
    }

    return get_specific_columns_for_inner_join(left, right, column_names);
}

auto SelectParser::get_specific_columns_for_inner_join(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {
    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& left_table_data : left_table.get_all_data()) {
        for (const auto& right_table_data : right_table.get_all_data()) {
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
                            return {};
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    }
                }
                data.push_back(combined_data);
            }
        }
    }

    return data;
}

auto SelectParser::get_select_left_join_result(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto left_clause_index = find_index(query_elements, "LEFT");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(left_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and LEFT clause should contain JOIN and ON clauses!");
        return {};
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  left_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return {};

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        return get_all_columns_for_join(left, right, JoinType::LEFT);
    }

    return get_specific_columns_for_left_join(left, right, column_names);
}

auto SelectParser::get_specific_columns_for_left_join(
    const std::string &left, const std::string &right, const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {

    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& left_table_data : left_table.get_all_data()) {

        auto match_found = false;

        for (const auto& right_table_data : right_table.get_all_data()) {
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
                            return {};
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
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
                    else combined_data.emplace_back("");
                } else {
                    const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                    const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                    if (c_left_index != -1 && c_right_index != -1) {
                        fmt::println("Column with name '{}' exists in both tables!", column_name);
                        return {};
                    }

                    if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            data.push_back(combined_data);
        }
    }

    return data;
}

auto SelectParser::get_select_right_join_result(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto right_clause_index = find_index(query_elements, "RIGHT");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(right_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and RIGHT clause should contain JOIN and ON clauses!");
        return {};
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  right_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return {};

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        return get_all_columns_for_join(left, right, JoinType::RIGHT);
    }

    return get_specific_columns_for_right_join(left, right, column_names);
}

auto SelectParser::get_specific_columns_for_right_join(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &column_names
) const -> std::vector<std::vector<std::string>> {

    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& right_table_data : right_table.get_all_data()) {

        auto match_found = false;

        for (const auto& left_table_data : left_table.get_all_data()) {
            if (right_table_data[right_column_id] == left_table_data[left_column_id]) {
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
                            return {};
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
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
                    if (t_name == right_table_name) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, c_name)]);
                    else combined_data.emplace_back("");
                } else {
                    const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                    const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                    if (c_left_index != -1 && c_right_index != -1) {
                        fmt::println("Column with name '{}' exists in both tables!", column_name);
                        return {};
                    }

                    if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            data.push_back(combined_data);
        }
    }

    return data;
}

auto SelectParser::get_select_full_join_result(
    const std::vector<std::string>& query_elements,
    const std::vector<std::string>& column_names
) const -> std::vector<std::vector<std::string>> {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto full_clause_index = find_index(query_elements, "FULL");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(full_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and FULL clause should contain JOIN and ON clauses!");
        return {};
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  full_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return {};

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        return get_all_columns_for_join(left, right, JoinType::FULL);
    }

    return get_specific_columns_for_full_join(left, right, column_names);
}

auto SelectParser::get_specific_columns_for_full_join(
    const std::string& left,
    const std::string& right,
    const std::vector<std::string>& column_names
) const -> std::vector<std::vector<std::string>> {

    auto data = std::vector<std::vector<std::string>>{};
    auto seen_rows = std::set<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (const auto& left_table_data : left_table.get_all_data()) {

        auto match_found = false;

        for (const auto& right_table_data : right_table.get_all_data()) {
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
                            return {};
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    }
                }
                if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
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
                    else combined_data.emplace_back("");
                } else {
                    const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                    const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                    if (c_left_index != -1 && c_right_index != -1) {
                        fmt::println("Column with name '{}' exists in both tables!", column_name);
                        return {};
                    }

                    if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            if (seen_rows.insert(combined_data).second)data.push_back(combined_data);
        }
    }

    for (const auto& right_table_data : right_table.get_all_data()) {

        auto match_found = false;

        for (const auto& left_table_data : left_table.get_all_data()) {
            if (right_table_data[right_column_id] == left_table_data[left_column_id]) {
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
                            return {};
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    }
                }
                if (seen_rows.insert(combined_data).second)data.push_back(combined_data);
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (auto column_name : column_names) {
                std::erase(column_name, ',');
                if (const auto dot_pos = column_name.find('.'); dot_pos != std::string::npos) {
                    const auto [t_name, c_name] = split_string_with_dot(column_name);
                    if (t_name == right_table_name) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, c_name)]);
                    else combined_data.emplace_back("");
                } else {
                    const auto c_left_index = Table::find_index(left_table.column_names, column_name);
                    const auto c_right_index = Table::find_index(right_table.column_names, column_name);

                    if (c_left_index != -1 && c_right_index != -1) {
                        fmt::println("Column with name '{}' exists in both tables!", column_name);
                        return {};
                    }

                    if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
        }
    }

    return data;
}

auto SelectParser::get_all_columns_for_join(
    const std::string &left,
    const std::string &right,
    const JoinType& join_type
) const -> std::vector<std::vector<std::string>> {
    auto data = std::vector<std::vector<std::string>>{};
    auto seen_rows = std::set<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (auto left_table_data : left_table.get_all_data()) {
        auto match_found = false;

        for (auto right_table_data : right_table.get_all_data()) {
            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                auto combined_data = std::vector<std::string>{};
                for (const auto& element : left_table_data) combined_data.push_back(element);
                for (const auto& element : right_table_data) combined_data.push_back(element);
                if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (const auto& element : left_table_data) combined_data.push_back(element);

            if (join_type == JoinType::LEFT || join_type == JoinType::FULL) {
                for (auto i = 0; i < right_table.column_names.size(); i++) combined_data.emplace_back("");
            }

            if (join_type == JoinType::FULL) {
                if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
            } else if (join_type == JoinType::LEFT) data.push_back(combined_data);
        }
    }

    if (join_type == JoinType::RIGHT || join_type == JoinType::FULL) {
        for (auto right_table_data : right_table.get_all_data()) {
            bool match_found = false;
            for (auto& left_table_data : left_table.get_all_data()) {
                if (right_table_data[right_column_id] == left_table_data[left_column_id]) {
                    match_found = true;
                    break;
                }
            }

            if (!match_found) {
                auto combined_data = std::vector<std::string>{};
                for (auto i = 0; i < left_table.column_names.size(); i++) combined_data.emplace_back("");
                for (const auto& element : right_table_data) combined_data.push_back(element);

                if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
            }
        }
    }

    return data;
}

auto SelectParser::print_appropriate_columns_without_joins(
    const std::vector<std::string>& query_elements
) const -> void {
    auto flattened_results = std::vector<std::vector<std::string>>{};
    const auto database = parser.database;
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto where_clause_index = find_index(query_elements, "WHERE");
    const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);
    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1,
        where_clause_index == -1 ? query_elements.end() : query_elements.begin() + where_clause_index);

    if (where_clause_index == -1) {
        for (auto table_name : table_names) {
            std::erase(table_name, ',');
            auto data_from_table = column_names.size() == 1 && column_names.at(0) == "*" ?
                database.value().tables.find(table_name)->second.get_all_data() :
                database.value().tables.find(table_name)->second.get_all_data_from(column_names);
            flattened_results.insert(flattened_results.end(), data_from_table.begin(), data_from_table.end());
        }

        fmt::println("{}", flattened_results);

        return;
    }

    auto results_and_comparison_operators = std::vector<std::vector<std::vector<std::string>>>{};
    for (auto it = query_elements.begin() + where_clause_index + 1; it + 2 < query_elements.end(); it += 4) {
        const auto& condition_column_name = *it;
        const auto& comparison_operator = *(it + 1);
        const auto& condition_column_value = *(it + 2);

        if (comparison_operator == "=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_equality(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<>" || comparison_operator == "!=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_inequality(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == ">") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_greater_than(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == ">=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_greater_than_or_equal(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_less_than(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "<=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_less_than_or_equal(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
        }

        if (comparison_operator == "LIKE") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_like(
                    table_names, column_names, condition_column_name, condition_column_value, false, {}));
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

    fmt::println("{}", results);
}

auto SelectParser::print_appropriate_columns_with_joins(
    const std::vector<std::string>& query_elements,
    const std::vector<std::string>& column_names
) const -> void {

    const auto database = parser.database;
    auto select_results = std::vector<std::vector<std::string>>{};
    auto flattened_results = std::vector<std::vector<std::string>>{};

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto left_clause_index = find_index(query_elements, "LEFT");
    const auto right_clause_index = find_index(query_elements, "RIGHT");
    const auto full_clause_index = find_index(query_elements, "FULL");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto where_clause_index = find_index(query_elements, "WHERE");

    auto table_names = std::vector<std::string>{};

    if (inner_clause_index != -1) {
        select_results = get_select_inner_join_result(query_elements, column_names);
        table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  inner_clause_index);
        table_names.emplace_back(query_elements.at(join_clause_index + 1));
    }

    if (left_clause_index != -1) {
        select_results = get_select_left_join_result(query_elements, column_names);
        table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  left_clause_index);
        table_names.emplace_back(query_elements.at(join_clause_index + 1));
    }

    if (right_clause_index != -1) {
        select_results = get_select_right_join_result(query_elements, column_names);
        table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  right_clause_index);
        table_names.emplace_back(query_elements.at(join_clause_index + 1));
    }

    if (full_clause_index != -1) {
        select_results = get_select_full_join_result(query_elements, column_names);
        table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() + full_clause_index);
        table_names.emplace_back(query_elements.at(join_clause_index + 1));
    }

    if (where_clause_index == -1) {
        fmt::println("{}", select_results);
        return;
    }

    auto results_and_comparison_operators = std::vector<std::vector<std::vector<std::string>>>{};
    for (auto it = query_elements.begin() + where_clause_index + 1; it + 2 < query_elements.end(); it += 4) {
        const auto& condition_column_name = *it;
        const auto& comparison_operator = *(it + 1);
        const auto& condition_column_value = *(it + 2);

        if (comparison_operator == "=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_equality(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == "<>" || comparison_operator == "!=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_inequality(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == ">") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_greater_than(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == ">=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_greater_than_or_equal(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == "<") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_less_than(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == "<=") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_less_than_or_equal(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
        }

        if (comparison_operator == "LIKE") {
            results_and_comparison_operators.push_back(
                get_data_filtered_by_like(
                    table_names, column_names, condition_column_name, condition_column_value, true, select_results));
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

    fmt::println("{}", results);
}

auto SelectParser::get_data_filtered_by_equality(
    const std::vector<std::string>& table_names,
    const std::vector<std::string>& column_names,
    const std::string& condition_column_name,
    const std::string& condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>>& select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                       .value()
                       .get_table_by_name(table_name)
                       .get_data_filtered_by_equality(
                           select_results,
                           column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                           condition_column_name,
                           condition_column_value);
            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_equality(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::get_data_filtered_by_inequality(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_inequality(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_inequality(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::get_data_filtered_by_greater_than(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_greater_than(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_greater_than(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

   return flattened_results;
}

auto SelectParser::get_data_filtered_by_greater_than_or_equal(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_greater_than_or_equal(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_greater_than_or_equal(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::get_data_filtered_by_less_than(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_less_than(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_less_than(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::get_data_filtered_by_less_than_or_equal(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_less_than_or_equal(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_less_than_or_equal(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::get_data_filtered_by_like(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string>> {
    const auto database = parser.database;
    auto flattened_results = std::vector<std::vector<std::string>>{};

    for (auto table_name : table_names) {
        std::erase(table_name, ',');

        auto filtered_data = std::vector<std::vector<std::string>>{};

        if (is_select_with_join) {
            auto column_names_from_both = get_column_names_from_both(table_names);

            filtered_data = parser.database
                .value()
                .get_table_by_name(table_name)
                .get_data_filtered_by_like(
                    select_results,
                    column_names.size() == 1 && column_names.at(0) == "*" ? column_names_from_both : column_names,
                    condition_column_name,
                    condition_column_value);

            for (const auto& data : filtered_data) {
                if (std::ranges::find(flattened_results, data) == flattened_results.end()) flattened_results.push_back(data);
            }

            continue;
        }

        filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_like(
                column_names.size() == 1 && column_names.at(0) == "*" ?
                    database.value().tables.find(table_name)->second.get_all_data() :
                    database.value().tables.find(table_name)->second.get_all_data_from(column_names),
                column_names,
                condition_column_name,
                condition_column_value);
        flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
    }

    return flattened_results;
}

auto SelectParser::find_index(const std::vector<std::string> &vec, const std::string &value) -> int {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return i;
    }
    return -1;
}

auto SelectParser::is_valid_select_join_query(int join_type_clause_index, int join_clause_index, int on_clause_index) -> bool {
    return join_clause_index == join_type_clause_index + 1 && on_clause_index == join_clause_index + 2;
}

auto SelectParser::validate_tables_and_columns(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &query_elements
) const -> bool {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto join_clause_index = find_index(query_elements, "JOIN");

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    if (!parser.database.value().tables.contains(left_table_name)) {
        fmt::println("Table with name '{}' not exists in database '{}'!", left_table_name, parser.database.value().name);
        return false;
    }

    if (!parser.database.value().tables.contains(right_table_name)) {
        fmt::println("Table with name '{}' not exists in database '{}'!", right_table_name, parser.database.value().name);
    }

    if (left_table_name != query_elements.at(from_clause_index + 1)) {
        fmt::println("Left table name after ON clause not match table name after FROM clause!");
        return false;
    }

    if (right_table_name != query_elements.at(join_clause_index + 1)) {
        fmt::println("Right table name after ON clause not match table name after JOIN clause!");
        return false;
    }

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

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

auto SelectParser::split_string_with_dot(const std::string &str) -> std::pair<std::string, std::string> {
    const auto dot_pos = str.find('.');
    auto left = str.substr(0, dot_pos);
    auto right = str.substr(dot_pos + 1);

    return std::pair{left, right};
}

auto SelectParser::get_column_names_from_both(const std::vector<std::string> &table_names) const -> std::vector<std::string> {
    auto column_names_from_both = std::vector<std::string>{};
    for (auto& tab_name : table_names) {
        const auto& col_names = parser.database->get_table_by_name(tab_name).column_names;

        for (const auto& col_name : col_names) {
            column_names_from_both.push_back(tab_name + "." + col_name);
        }
    }
    return column_names_from_both;
}
