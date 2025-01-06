#include "whereclauseparser.h"

auto WhereClauseParser::get_data_filtered_by_equality(
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

auto WhereClauseParser::get_data_filtered_by_inequality(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string> > {
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

auto WhereClauseParser::get_data_filtered_by_greater_than(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string> > {
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

auto WhereClauseParser::get_data_filtered_by_greater_than_or_equal(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string> > &select_results
) const -> std::vector<std::vector<std::string> > {
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

auto WhereClauseParser::get_data_filtered_by_less_than(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string> > {
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

auto WhereClauseParser::get_data_filtered_by_less_than_or_equal(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string> > {
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

auto WhereClauseParser::get_data_filtered_by_like(
    const std::vector<std::string> &table_names,
    const std::vector<std::string> &column_names,
    const std::string &condition_column_name,
    const std::string &condition_column_value,
    const bool is_select_with_join,
    const std::vector<std::vector<std::string>> &select_results
) const -> std::vector<std::vector<std::string> > {
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


auto WhereClauseParser::get_column_names_from_both(const std::vector<std::string> &table_names) const -> std::vector<std::string> {
    auto column_names_from_both = std::vector<std::string>{};
    for (auto& tab_name : table_names) {
        const auto& col_names = parser.database->get_table_by_name(tab_name).column_names;

        for (const auto& col_name : col_names) {
            column_names_from_both.push_back(tab_name + "." + col_name);
        }
    }
    return column_names_from_both;
}

