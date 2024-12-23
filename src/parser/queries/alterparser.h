#pragma once

#include "../parser.h"

struct AlterParser {

    Parser& parser;

    explicit AlterParser(Parser& parser) : parser(parser) {}

    auto parse_alter_query(const std::vector<std::string>& query_elements) const -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};