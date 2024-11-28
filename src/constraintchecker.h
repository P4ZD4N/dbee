#pragma once

#include <vector>
#include <string>

#include "table.h"
#include "enums/constraint.h"

class ConstraintChecker {
public:
    static auto check_data(
        const std::vector<std::string>& data,
        const std::vector<std::vector<Constraint>>& constraints,
        const Table& table
    ) -> void;

private:
    static auto is_null(const std::string& element) -> bool;
    static auto is_unique(const std::string& searched_element, const Table& table, const int& column_number) -> bool;
    static auto is_primary_key(const std::string& element, const Table& table, const int& column_number) -> bool;
};
