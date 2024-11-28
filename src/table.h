#pragma once

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "enums/columntype.h"

class Table {
public:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<ColumnType> column_types;
    std::vector<std::vector<std::string>> rows;

    Table(const std::string& name,
          const std::vector<std::string>& column_names,
          const std::vector<ColumnType>& column_types)
        : name(name), column_names(column_names), column_types(column_types) {

        if (column_names.size() != column_types.size())
            throw std::invalid_argument("Number of column names and column types must be equal!");
    }

    auto insert_row(const std::vector<std::string>& data) -> void;
    auto get_data() const -> std::vector<std::vector<std::string>>;
};

