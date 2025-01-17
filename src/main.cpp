#include <iostream>
#include <fmt/ranges.h>

#include "database/database.h"
#include "parser/parser.h"
#include "serializer/serializer.h"

int main() {

    Serializer::upload_databases_from_file();
    auto parser = Parser();

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
