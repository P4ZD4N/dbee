#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include "enums/columntype.h"

struct Table {
    std::string name;
    std::vector<std::string> column_names;
    std::vector<ColumnType> column_types;

    Table(const std::string& name,
          const std::vector<std::string>& column_names,
          const std::vector<ColumnType>& column_types)
        : name(name), column_names(column_names), column_types(column_types) {

        if (column_names.size() != column_types.size())
            throw std::invalid_argument("Number of column names and column types must be equal!");
    }
};

