#include <iostream>
#include <fmt/ranges.h>

#include "database/database.h"
#include "parser/parser.h"

int main() {
    auto parser = Parser();
    auto query = std::string();
    while (true) {
        fmt::print("Query: ");
        std::getline(std::cin, query);

        if (query.empty()) break;

        parser.parse_query(query);
    }

    return 0;
}
