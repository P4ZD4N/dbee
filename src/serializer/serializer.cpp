#include "serializer.h"

#include <fstream>
#include <ranges>
#include <fmt/ostream.h>

auto format_as(const ColumnType column_type) {
    switch (column_type) {
        case ColumnType::INTEGER:
            return "INTEGER";
        case ColumnType::FLOAT:
            return "FLOAT";
        case ColumnType::TEXT:
            return "TEXT";
        default:
            return "UNKNOWN";
    }
}

auto format_as(const Constraint constraint) {
    switch (constraint) {
        case Constraint::PRIMARY_KEY:
            return "PRIMARY_KEY";
        case Constraint::FOREIGN_KEY:
            return "FOREIGN_KEY";
        case Constraint::UNIQUE:
            return "UNIQUE";
        case Constraint::NOT_NULL:
            return "NOT_NULL";
        default:
            return "UNKNOWN";
    }
}

auto format_as(const std::pair<Table*, std::string>& pair) -> std::string {
    if (pair.first == nullptr) return "";

    return pair.first->name + "." + pair.second;
}


auto Serializer::save_databases_to_file() -> void {

    auto file = std::fstream("../../data.txt", std::ios::out);

    if (!file.is_open()) {
        fmt::println("Failed to open file: data.txt");
        return;
    }

    auto reversed_databases = std::vector<std::string>{};
    for (const auto& [database_name, database] : Database::databases) {
        reversed_databases.push_back(database_name);
    }

    for (const auto& database_name : std::ranges::reverse_view(reversed_databases)) {
        auto* database = Database::get_database(database_name);
        fmt::println(file, "{}", database_name);

        auto reversed_tables = std::vector<std::string>{};
        for (const auto& [name, table] : database->tables) {
            reversed_tables.push_back(name);
        }

        for (const auto& table_name : std::ranges::reverse_view(reversed_tables)) {
            const auto& table = database->tables.at(table_name);

            fmt::println(file, "{}", table_name);
            fmt::println(file, "{}", table.column_names);
            fmt::println(file, "{}", table.column_types);
            fmt::println(file, "{}", table.column_constraints);
            fmt::println(file, "{}", table.column_foreign_keys);

            for (const auto& row : table.rows) {
                fmt::println(file, "{}", row);
            }
        }
        fmt::println(file, "{}", "-");
    }

    fmt::println("Successfully saved databases data to file!");
}

auto Serializer::upload_databases_from_file() -> void {

    auto file = std::fstream("../../data.txt", std::ios::in);

    if (!file.is_open()) {
        fmt::println("Failed to open file: data.txt");
        return;
    }

    auto line = std::string("");
    auto current_database = static_cast<Database*>(nullptr);
    auto table_name = std::string("");
    auto column_names = std::vector<std::string>{};
    auto column_types = std::vector<ColumnType>{};
    auto column_constraints = std::vector<std::vector<Constraint>>{};
    auto column_foreign_keys = std::vector<std::pair<Table*, std::string>>{};
    auto rows = std::vector<std::vector<std::string>>{};
    auto line_number = 0;
    auto repeat_iteration = false;

    while (repeat_iteration || std::getline(file, line)) {
        auto handled_by_switch = false;

        switch (line_number) {
            case 0: {
                current_database = Database::get_database(line);

                if (!current_database) {
                    Database::create_database(line);
                    current_database = Database::get_database(line);
                }

                handled_by_switch = true;
                line_number++;
            } break;

            case 1: {
                table_name = line;

                column_names.clear();
                column_types.clear();
                column_constraints.clear();
                column_foreign_keys.clear();
                rows.clear();

                handled_by_switch = true;
                repeat_iteration = false;
                line_number++;
            } break;

            case 2: {
                column_names = parse_vector(line);
                handled_by_switch = true;
                line_number++;
            } break;

            case 3: {
                column_types = strings_to_column_types(parse_vector(line));
                handled_by_switch = true;
                line_number++;
            } break;

            case 4: {
                for (const auto& element : parse_vector_of_vectors(line)) {
                    column_constraints.push_back(strings_to_constraints(element));
                }

                handled_by_switch = true;
                line_number++;
            } break;

            case 5: {
                column_foreign_keys = parse_foreign_keys(line, current_database);
                handled_by_switch = true;
                line_number++;
            } break;

            default: {
                handled_by_switch = false;
            } break;
        }

        if (handled_by_switch) continue;
        if (line.starts_with("[")) rows.push_back(parse_vector(line));
        if (!line.starts_with("[") && current_database) {
            current_database->create_table(
                    table_name,
                    column_names,
                    column_types,
                    column_constraints,
                    column_foreign_keys
            );

            for (const auto& row : rows) current_database->insert_data(table_name, row);

            if (line == "-") {
                line_number = 0;
                current_database = nullptr;
                continue;
            }

            repeat_iteration = true;
            line_number = 1;
        }
    }

    fmt::println("Successfully uploaded databases data from file!");
}

auto Serializer::parse_vector(const std::string& line) -> std::vector<std::string> {
    auto result = std::vector<std::string>{};

    const auto cleaned = line.substr(1, line.size() - 2);

    auto ss = std::stringstream(cleaned);
    auto item = std::string("");

    while (std::getline(ss, item, ',')) {
        item = item.erase(0, item.find_first_not_of(" \t"));
        item = item.erase(item.find_last_not_of(" \t") + 1);

        if (item.front() == '"' && item.back() == '"') item = item.substr(1, item.size() - 2);

        result.push_back(item);
    }

    return result;
}
auto Serializer::parse_vector_of_vectors(const std::string& line) -> std::vector<std::vector<std::string>> {
    auto result = std::vector<std::vector<std::string>>{};

    const auto cleaned = line.substr(1, line.size() - 2);

    size_t start = 0;

    while (true) {
        start = cleaned.find('[', start);
        if (start == std::string::npos) break;

        const size_t end = cleaned.find(']', start);
        if (end == std::string::npos) break;

        const auto sub_vector = cleaned.substr(start + 1, end - start - 1);
        auto parsed_sub_vector = parse_vector("[" + sub_vector + "]");

        result.push_back(parsed_sub_vector);

        start = end + 1;
    }

    return result;
}

auto Serializer::parse_foreign_keys(const std::string& line, Database* current_database) -> std::vector<std::pair<Table*, std::string>> {
    auto result = std::vector<std::pair<Table*, std::string>>{};

    const auto cleaned = line.substr(1, line.size() - 2);

    auto ss = std::stringstream(cleaned);
    auto item = std::string("");
    while (std::getline(ss, item, ',')) {
        item = item.erase(0, item.find_first_not_of(" \t"));
        item = item.erase(item.find_last_not_of(" \t") + 1);

        if (item.front() == '"' && item.back() == '"') item = item.substr(1, item.size() - 2);

        if (item.empty()) result.emplace_back(nullptr, "");

        const size_t dot_pos = item.find('.');
        if (dot_pos != std::string::npos) {
            const auto table_name = item.substr(0, dot_pos);
            const auto column_name = item.substr(dot_pos + 1);

            auto* table = &current_database->get_table_by_name(table_name);

            result.emplace_back(table, column_name);
        }
    }

    return result;
}
