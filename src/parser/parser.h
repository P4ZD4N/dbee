#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../database/database.h"

class Parser {

public:
    std::optional<Database> database;

    Parser() = default;

    explicit Parser(const Database& database) : database(database) {}

    auto parse_query(const std::string& query) -> void;
    [[nodiscard]] auto is_database_selected() const -> bool;
};

