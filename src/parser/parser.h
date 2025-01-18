#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../database/database.h"
#include "../serializer/serializer.h"

class Parser {

public:
    Database* database = nullptr;

    Parser() = default;

    explicit Parser(Database* database) : database(database) {}

    ~Parser() {
        Serializer::save_databases_to_file();
    }

    auto parse_query(const std::string& query) -> void;
    [[nodiscard]] auto is_database_selected() const -> bool;
};

