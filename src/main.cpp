#include <fmt/ranges.h>
#include "database.h"

int main() {
    auto db1 = Database("db1");
    auto db2 = Database("db2");

    for (const auto& [str, db] : Database::databases) {
        fmt::println("Name: {}", str);
    }

    db1.create_table(
        "Users",
        std::vector<std::string>{"name", "age", "height"},
        std::vector{ColumnType::TEXT, ColumnType::INTEGER, ColumnType::FLOAT}
    );

    db1.insert_data("Users", std::vector<std::string>{"name", "15", "1.8"});

    return 0;
}
