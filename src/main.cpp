#include <iostream>
#include <fmt/ranges.h>

#include "database/database.h"
#include "parser/parser.h"
#include "serializer/serializer.h"
#include <csignal>

auto handle_signal(const int signal) -> void{
    Serializer::save_databases_to_file();
    std::exit(signal);
}

int main() {
    Serializer::upload_databases_from_file();

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    auto parser = Parser();

    auto query = std::string();
    while (query != "EXIT") {
        fmt::print("Query: ");
        std::getline(std::cin, query);

        if (query == "EXIT") break;
        if (query.empty()) {
            fmt::println("Can't process empty query!");
            continue;
        }

        parser.parse_query(query);
    }

    return 0;
}
