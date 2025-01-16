#pragma once

#include <unordered_map>
#include <string>

#include "../database/database.h"

struct Serializer {
    static auto save_databases_to_file() -> void;
};

