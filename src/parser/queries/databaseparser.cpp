#include "databaseparser.h"

auto DatabaseParser::parse_database_query(const std::vector<std::string>& query_elements) -> void {

    if (query_elements.at(1) == "CREATE") {
        const auto& database_name = query_elements.at(2);
        Database::create_database(database_name);
    } else if (query_elements.at(1) == "USE") {
        const auto& database_name = query_elements.at(2);
        parser.database = Database::get_database(database_name);
        fmt::println("Selected database with name: {}", parser.database->name);
    } else if (query_elements.at(1) == "DROP") {
        const auto& database_name = query_elements.at(2);
        Database::drop_database(database_name);
    } else fmt::println("Query with DATABASE clause should contain correct operation clause after DATABASE clause!");
}