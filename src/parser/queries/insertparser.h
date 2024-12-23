#pragma once

#include "../parser.h"

struct InsertParser {

    Parser& parser;

    explicit InsertParser(Parser& parser) : parser(parser) {}

    auto parse_insert_query(const std::vector<std::string>& query_elements) const -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};