#pragma once

#include "../parser.h"

struct UpdateParser {

    Parser& parser;

    explicit UpdateParser(Parser& parser) : parser(parser) {}

    auto parse_update_query(const std::vector<std::string>& query_elements) const -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};

