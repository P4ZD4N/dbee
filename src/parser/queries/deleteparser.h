#pragma once

#include "../parser.h"

struct DeleteParser {

    Parser& parser;

    explicit DeleteParser(Parser& parser) : parser(parser) {}

    auto parse_delete_query(const std::vector<std::string>& query_elements) const -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};