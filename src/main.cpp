#include <iostream>
#include <fmt/ranges.h>

#include "database/database.h"
#include "parser/parser.h"

int main() {

    auto parser = Parser();

    parser.parse_query("DATABASE CREATE db1");
    parser.parse_query("DATABASE USE db1");
    parser.parse_query("TABLE CREATE users WITH COLUMNS id(INTEGER)[PRIMARY_KEY], name(TEXT)[NOT_NULL], weight(INTEGER)[NOT_NULL]");
    parser.parse_query("TABLE CREATE pets WITH COLUMNS id(INTEGER)[PRIMARY_KEY], name(TEXT)[NOT_NULL], height(FLOAT)[NOT_NULL]");
    parser.parse_query("INSERT INTO users VALUES 1, Wiktor, 80");
    parser.parse_query("INSERT INTO users VALUES 2, Mateusz, 76");
    parser.parse_query("INSERT INTO pets VALUES 2, Dog, 1.80");
    parser.parse_query("INSERT INTO pets VALUES 3, Cat, 1.90");
    parser.parse_query("SELECT users.id, users.name, pets.name, weight, height FROM users LEFT JOIN pets ON users.id = pets.id");
//users.id, users.name, pets.name, height, weight
    auto query = std::string();
    while (true) {
        fmt::print("Query: ");
        std::getline(std::cin, query);

        if (query.empty()) break;

        parser.parse_query(query);
    }

    return 0;
}
