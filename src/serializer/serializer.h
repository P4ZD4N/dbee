#pragma once

#include <unordered_map>
#include <string>

#include "../database/database.h"

struct Serializer {
    static auto save_databases_to_file() -> void;
    static auto upload_databases_from_file() -> void;
private:
    static auto parse_vector(const std::string& line) -> std::vector<std::string>;
    static auto parse_vector_of_vectors(const std::string& line) -> std::vector<std::vector<std::string>>;
    static auto parse_foreign_keys(const std::string& line, Database* current_database) -> std::vector<std::pair<Table*, std::string>>;
};

