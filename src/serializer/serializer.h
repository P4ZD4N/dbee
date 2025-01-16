#pragma once

#include <unordered_map>
#include <string>

#include "../database/database.h"

struct Serializer {
    static auto serialize_database(const std::unordered_map<std::string, Table>& tables) -> void;
};

