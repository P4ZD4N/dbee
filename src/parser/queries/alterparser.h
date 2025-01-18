#pragma once

#include "../parser.h"

struct AlterParser {

    Parser& parser;

    explicit AlterParser(Parser& parser) : parser(parser) {}

    auto parse_alter_query(const std::vector<std::string>& query_elements) const -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
    static auto get_foreign_key_from_curly_braces(const std::string& detail) -> std::string;
    static auto split_string_with_dot(const std::string& str) -> std::pair<std::string, std::string>;
    [[nodiscard]] auto is_foreign_key_valid(
        const std::string& foreign_table_name,
        const std::string& foreign_column_name,
        const std::string& column_type
    ) const -> bool;
};