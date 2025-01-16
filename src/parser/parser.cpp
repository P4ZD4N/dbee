#include "../enums/columntype.h"
#include "../enums/constraint.h"
#include "parser.h"
#include "./queries/databaseparser.h"

#include <sstream>
#include <fmt/ranges.h>
#include <vector>

#include "../serializer/serializer.h"
#include "queries/alterparser.h"
#include "queries/deleteparser.h"
#include "queries/insertparser.h"
#include "queries/select/selectparser.h"
#include "queries/tableparser.h"
#include "queries/updateparser.h"

auto Parser::parse_query(const std::string& query) -> void {

    auto ss = std::stringstream(query);
    auto query_element = std::string();
    auto query_elements = std::vector<std::string>{};

    while (ss >> query_element) query_elements.push_back(query_element);

    auto databaseParser = DatabaseParser(*this);
    const auto selectParser = SelectParser(*this);
    const auto insertParser = InsertParser(*this);
    const auto alterParser = AlterParser(*this);
    const auto tableParser = TableParser(*this);
    const auto updateParser = UpdateParser(*this);
    const auto deleteParser = DeleteParser(*this);

    if (query_elements.at(0) == "DATABASE") databaseParser.parse_database_query(query_elements);
    else if (query_elements.at(0) == "SELECT") selectParser.parse_select_query(query_elements);
    else if (query_elements.at(0) == "INSERT") insertParser.parse_insert_query(query_elements);
    else if (query_elements.at(0) == "ALTER") alterParser.parse_alter_query(query_elements);
    else if (query_elements.at(0) == "TABLE") tableParser.parse_table_query(query_elements);
    else if (query_elements.at(0) == "UPDATE") updateParser.parse_update_query(query_elements);
    else if (query_elements.at(0) == "DELETE") deleteParser.parse_delete_query(query_elements);
    else if (query_elements.at(0) == "SAVE") {
        Serializer::serialize_database(database->tables);
    }
    else fmt::println("Unknown command: {}", query_elements.at(0));
}

auto Parser::is_database_selected() const -> bool {
    if (!database.has_value()) {
        fmt::println("No database selected. Please select it with 'DATABASE USE [...]'!");
        return false;
    }
    return true;
}
