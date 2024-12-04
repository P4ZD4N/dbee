#pragma once

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "../enums/columntype.h"
#include "../enums/constraint.h"

class Table {
public:
    std::string name;
    std::vector<std::string> column_names;
    std::vector<ColumnType> column_types;
    std::vector<std::vector<Constraint>> column_constraints;
    std::vector<std::vector<std::string>> rows;

    Table(const std::string& name,
          const std::vector<std::string>& column_names,
          const std::vector<ColumnType>& column_types,
          const std::vector<std::vector<Constraint>>& column_constraints)
        : name(name), column_names(column_names), column_types(column_types), column_constraints(column_constraints) {

        if (column_names.size() != column_types.size())
            throw std::invalid_argument("Number of column names and column types must be equal!");

        if (column_names.size() != column_constraints.size())
            throw std::invalid_argument("Number of column names and column constraints must be equal!");
    }

    auto insert_row(const std::vector<std::string>& data) -> void;
    [[nodiscard]] auto get_data() const -> std::vector<std::vector<std::string>>;
    [[nodiscard]] auto get_data_from(const std::string& column_name) const -> std::vector<std::string>;
    auto add_column(const std::string& column_name, const ColumnType& column_type, const std::vector<Constraint>& new_column_constraints) -> void;
    auto remove_column(const std::string& column_name) -> void;

private:
    static auto find_index(const std::vector<std::string>& vec, const std::string& value) -> int;
};

