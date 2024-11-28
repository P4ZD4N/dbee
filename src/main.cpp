#include <iostream>
#include <fmt/ranges.h>
#include "database.h"
#include "parser.h"

int main() {
    auto db1 = Database("db1");
    auto db2 = Database("db2");

    db1.create_table(
        "users",
        std::vector<std::string>{"name", "age", "height", "weight"},
        std::vector{ColumnType::TEXT, ColumnType::INTEGER, ColumnType::FLOAT, ColumnType::FLOAT}
    );

    db1.create_table(
        "pets",
        std::vector<std::string>{"name", "weight"},
        std::vector{ColumnType::TEXT, ColumnType::FLOAT}
    );

    db1.insert_data("users", std::vector<std::string>{"Michal", "18", "1.83", "83"});
    db1.insert_data("users", std::vector<std::string>{"Pablo", "16", "1.71", "75"});
    db1.insert_data("users", std::vector<std::string>{"Antonio", "20", "1.95", "79"});
    db1.insert_data("pets", std::vector<std::string>{"Pusia", "25.5"});

    auto desired_db_name = std::string();
    for (const auto& db : Database::databases) {
        fmt::println("- {}", db->name);
    }
    fmt::println("Choose database: ");
    std::cin >> desired_db_name;
    std::cin.ignore();

    auto parser = Parser();
    for (const auto& db : Database::databases) {
        if (db->name == desired_db_name) {
            parser = Parser(*db);
            break;
        }
    }

    auto query = std::string();
    while (true) {
        std::getline(std::cin, query);

        if (query.empty()) break;

        parser.parse_query(query);
    }

    return 0;
}
