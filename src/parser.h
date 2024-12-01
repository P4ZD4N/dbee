#pragma once

#include <string>
#include <vector>
#include <optional>

#include "database.h"

class Parser {

public:
    std::optional<Database> database;

    Parser() = default;

    explicit Parser(const Database& database) : database(database) {}

    auto parse_query(const std::string& query) -> void;

private:
    auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};

