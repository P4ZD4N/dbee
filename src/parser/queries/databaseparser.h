#pragma once

#include <vector>
#include <string>

#include "../../database/database.h"
#include "../parser.h"

struct DatabaseParser {
    Parser& parser;

    explicit DatabaseParser(Parser& parser) : parser(parser) {}

    auto parse_database_query(const std::vector<std::string>& query_elements) -> void;
};

