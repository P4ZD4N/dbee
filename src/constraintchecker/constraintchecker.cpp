#include "constraintchecker.h"
#include "../enums/constraint.h"
#include "../table/table.h"

#include <stdexcept>

#include "fmt/xchar.h"

auto ConstraintChecker::check_data(
    const std::vector<std::string> &data,
    const std::vector<std::vector<Constraint>> &constraints,
    const Table& table
) -> void {

    for (int i = 0; i < data.size(); i++) {

        const auto& element = data.at(i);

        for (const auto& constraint : constraints.at(i)) {
            switch (constraint) {
                case Constraint::NOT_NULL: {
                    if (is_null(element))
                        throw std::runtime_error("Can't insert NULL data to column with NOT_NULL constraint!");
                } break;

                case Constraint::UNIQUE: {
                    if (!is_unique(element, table, i))
                        throw std::runtime_error("Can't insert not unique data to column with UNIQUE constraint!");
                } break;

                case Constraint::PRIMARY_KEY: {
                    if (!is_primary_key(element, table, i))
                        throw std::runtime_error("Can't insert not unique or NULL data to column with PRIMARY_KEY constraint!");
                } break;

                case Constraint::FOREIGN_KEY: {
                    if (!is_foreign_key(element, table, i))
                        throw std::runtime_error("Can't insert data that violates FOREIGN KEY constraint!");
                } break;
            }
        }
    }
}

auto ConstraintChecker::is_null(const std::string &element) -> bool {
    return element.empty();
}

auto ConstraintChecker::is_unique(const std::string& searched_element, const Table& table, const int& column_number) -> bool {
    for (const auto& row : table.rows) {
        const auto& column_element = row.at(column_number);

        if (column_element == searched_element) return false;
    }

    return true;
}


auto ConstraintChecker::is_primary_key(const std::string& element, const Table& table, const int& column_number) -> bool {
    return !is_null(element) && is_unique(element, table, column_number);
}

auto ConstraintChecker::is_foreign_key(const std::string &element, const Table &table, const int &column_number) -> bool {

    return true;
}



