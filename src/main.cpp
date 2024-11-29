#include <iostream>
#include <fmt/ranges.h>
#include "database.h"
#include "parser.h"

int main() {
    auto db1 = Database("db1");
    auto db2 = Database("db2");

    db1.create_table(
        "users",
        std::vector<std::string>{"id", "name", "age", "height", "weight"},
        std::vector{ColumnType::INTEGER, ColumnType::TEXT, ColumnType::INTEGER, ColumnType::FLOAT, ColumnType::FLOAT},
        std::vector<std::vector<Constraint>>{{Constraint::PRIMARY_KEY}, {Constraint::NOT_NULL}, {}, {}, {}}
    );

    db1.create_table(
        "pets",
        std::vector<std::string>{"id", "name", "weight"},
        std::vector{ColumnType::INTEGER, ColumnType::TEXT, ColumnType::FLOAT},
        std::vector<std::vector<Constraint>>{{Constraint::PRIMARY_KEY}, {Constraint::NOT_NULL}, {}}
    );

    db1.insert_data("users", std::vector<std::string>{"1", "Michal", "18", "1.83", "83"});
    db1.insert_data("users", std::vector<std::string>{"2", "Pablo", "16", "1.71", "75"});
    db1.insert_data("users", std::vector<std::string>{"3", "Antonio", "20", "1.95", "79"});
    db1.insert_data("pets", std::vector<std::string>{"1", "Pusia", "25.5"});

    db1.get_table_by_name("users").add_column("city", ColumnType::TEXT, std::vector{Constraint::NOT_NULL});

    db1.insert_data("users", std::vector<std::string>{"4", "Antonio", "20", "1.95", "79", "Warsaw"});

    db1.get_table_by_name("users").remove_column("name");

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
