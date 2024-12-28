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
    const auto where_clause_index = find_index(query_elements, "WHERE");

    if (from_clause_index == -1) {
        fmt::println("Query with SELECT clause should contain FROM clause!");
        return;
    }

    const auto column_names = std::vector(query_elements.begin() + 1, query_elements.begin() + from_clause_index);

    if (inner_clause_index != -1) {
        process_select_inner_join_query(query_elements, column_names);
        return;
    }

    if (left_clause_index != -1) {
        process_select_left_join_query(query_elements, column_names);
        return;
    }

    if (right_clause_index != -1) {
        process_select_right_join_query(query_elements, column_names);
        return;
    }

    if (full_clause_index != -1) {
        process_select_full_join_query(query_elements, column_names);
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1,
        where_clause_index == -1 ? query_elements.end() : query_elements.begin() + where_clause_index);

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        auto flattened_results = std::vector<std::vector<std::string>>{};

        if (where_clause_index == -1) {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto table = database.value().tables.find(table_name)->second;
                auto data = table.get_all_data();
                flattened_results.insert(flattened_results.end(), data.begin(), data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        const auto& condition_column_name = query_elements.at(where_clause_index + 1);
        const auto& condition_column_value = query_elements.at(where_clause_index + 3);

        if (query_elements.at(where_clause_index + 2) == "=") {

            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_equality(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == "<>" || query_elements.at(where_clause_index + 2) == "!=") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_inequality(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == ">") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_greater_than(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == ">=") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_greater_than_or_equal(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == "<") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_less_than(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == "<=") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_less_than_or_equal(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        if (query_elements.at(where_clause_index + 2) == "LIKE") {
            for (auto& table_name : table_names) {
                std::erase(table_name, ',');
                auto filtered_data = parser.database.value().get_table_by_name(table_name).get_data_filtered_by_like(
                        database.value().tables.find(table_name)->second.get_all_data(), condition_column_name, condition_column_value);
                flattened_results.insert(flattened_results.end(), filtered_data.begin(), filtered_data.end());
            }
            fmt::println("{}", flattened_results);

            return;
        }

        return;
    }

    for (auto column_name : column_names) {
        std::erase(column_name, ',');

        for (auto table_name : table_names) {
            std::erase(table_name, ',');

            auto table = database.value().tables.find(table_name)->second;
            fmt::println("{}", table.get_all_data_from(column_name));
        }
    }
}

auto SelectParser::process_select_inner_join_query(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> void {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto inner_clause_index = find_index(query_elements, "INNER");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(inner_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and INNER clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  inner_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_inner_join(left, right);
        return;
    }

    print_specific_columns_for_inner_join(left, right, column_names);
}

auto SelectParser::print_all_columns_for_inner_join(const std::string &left, const std::string &right) const -> void {
    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (auto left_table_data : left_table.get_all_data()) {

        auto combined_data = std::vector<std::string>{};

        for (auto right_table_data : right_table.get_all_data()) {

            if (left_table_data[left_column_id] == right_table_data[right_column_id]) {
                for (const auto& element : left_table_data) combined_data.push_back(element);
                for (const auto& element : right_table_data) combined_data.push_back(element);
                data.push_back(combined_data);
                combined_data.clear();
            }
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::print_specific_columns_for_inner_join(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &column_names
) const -> void {
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
                            return;
                        }

                        if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                        if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    }
                }
                data.push_back(combined_data);
            }
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::process_select_left_join_query(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> void {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto left_clause_index = find_index(query_elements, "LEFT");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(left_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and LEFT clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  left_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_left_join(left, right);
        return;
    }

    print_specific_columns_for_left_join(left, right, column_names);
}

auto SelectParser::print_all_columns_for_left_join(const std::string &left, const std::string &right) const -> void {
    auto data = std::vector<std::vector<std::string>>{};

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
                data.push_back(combined_data);
                combined_data.clear();
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (const auto& element : left_table_data) combined_data.push_back(element);
            for (auto i = 0; i < right_table.column_names.size(); i++) combined_data.emplace_back("");
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::print_specific_columns_for_left_join(const std::string &left, const std::string &right, const std::vector<std::string> &column_names) const -> void {

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
                            return;
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
                        return;
                    }

                    if (c_left_index != -1) combined_data.push_back(left_table_data[Table::find_index(left_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::process_select_right_join_query(
    const std::vector<std::string> &query_elements,
    const std::vector<std::string> &column_names
) const -> void {
    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto right_clause_index = find_index(query_elements, "RIGHT");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(right_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and RIGHT clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  right_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_right_join(left, right);
        return;
    }

    print_specific_columns_for_right_join(left, right, column_names);
}

auto SelectParser::print_all_columns_for_right_join(const std::string &left, const std::string &right) const -> void {

    auto data = std::vector<std::vector<std::string>>{};

    const auto [left_table_name, left_column_name] = split_string_with_dot(left);
    const auto [right_table_name, right_column_name] = split_string_with_dot(right);

    auto left_table = parser.database.value().tables.find(left_table_name)->second;
    auto right_table = parser.database.value().tables.find(right_table_name)->second;

    const auto left_column_id = Table::find_index(left_table.column_names, left_column_name);
    const auto right_column_id = Table::find_index(right_table.column_names, right_column_name);

    for (auto right_table_data : right_table.get_all_data()) {

        auto match_found = false;

        for (auto left_table_data : left_table.get_all_data()) {
            if (right_table_data[right_column_id] == left_table_data[left_column_id]) {
                auto combined_data = std::vector<std::string>{};
                for (const auto& element : left_table_data) combined_data.push_back(element);
                for (const auto& element : right_table_data) combined_data.push_back(element);
                data.push_back(combined_data);
                combined_data.clear();
                match_found = true;
            }
        }

        if (!match_found) {
            auto combined_data = std::vector<std::string>{};
            for (auto i = 0; i < left_table.column_names.size(); i++) combined_data.emplace_back("");
            for (const auto& element : right_table_data) combined_data.push_back(element);
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::print_specific_columns_for_right_join(
    const std::string &left,
    const std::string &right,
    const std::vector<std::string> &column_names
) const -> void {

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
                            return;
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
                        return;
                    }

                    if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
}

auto SelectParser::process_select_full_join_query(
    const std::vector<std::string>& query_elements,
    const std::vector<std::string>& column_names
) const -> void {

    const auto from_clause_index = find_index(query_elements, "FROM");
    const auto full_clause_index = find_index(query_elements, "FULL");
    const auto join_clause_index = find_index(query_elements, "JOIN");
    const auto on_clause_index = find_index(query_elements, "ON");

    if (!is_valid_select_join_query(full_clause_index, join_clause_index, on_clause_index)) {
        fmt::println("Query with SELECT and FULL clause should contain JOIN and ON clauses!");
        return;
    }

    auto table_names = std::vector(query_elements.begin() + from_clause_index + 1, query_elements.begin() +  full_clause_index);
    table_names.emplace_back(query_elements.at(join_clause_index + 1));

    const auto& left = query_elements.at(on_clause_index + 1);
    const auto& right = query_elements.at(on_clause_index + 3);

    if (!validate_tables_and_columns(left, right, query_elements)) return;

    if (column_names.size() == 1 and column_names.at(0) == "*") {
        print_all_columns_for_full_join(left, right);
        return;
    }

    print_specific_columns_for_full_join(left, right, column_names);
}

auto SelectParser::print_all_columns_for_full_join(
    const std::string& left,
    const std::string& right
) const -> void {

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
            for (auto i = 0; i < right_table.column_names.size(); i++) combined_data.emplace_back("");
            data.push_back(combined_data);
        }
    }

    for (auto right_table_data : right_table.get_all_data()) {

        auto match_found = false;

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

    fmt::println("{}", data);
}

auto SelectParser::print_specific_columns_for_full_join(
    const std::string& left,
    const std::string& right,
    const std::vector<std::string>& column_names
) const -> void {

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
                            return;
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
                        return;
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
                            return;
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
                        return;
                    }

                    if (c_right_index != -1) combined_data.push_back(right_table_data[Table::find_index(right_table.column_names, column_name)]);
                    else combined_data.emplace_back("");
                }
            }
            if (seen_rows.insert(combined_data).second) data.push_back(combined_data);
        }
    }

    fmt::println("{}", data);
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



