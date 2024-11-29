#include "database.h"

#include <stdexcept>
#include <vector>

std::vector<Database*> Database::databases;

auto Database::create_table(
    const std::string& name,
    const std::vector<std::string>& column_names,
    const std::vector<ColumnType>& column_types,
    const std::vector<std::vector<Constraint>>& column_constraints
) -> void {

    if (tables.contains(name)) throw std::runtime_error("Table with name '" + name + "' already exists!");

    const auto new_table = Table(name, column_names, column_types, column_constraints);
    tables.insert({name, new_table});

    fmt::println("Successfully created table with name: '{}' in database with name: '{}'", name, this->name);
}

auto Database::insert_data(const std::string& table_name, const std::vector<std::string>& data) -> void {

    if (!tables.contains(table_name)) throw std::runtime_error(
        "Table with name '" + table_name + "' does not exist in database with name: '" + this->name + "!");

    tables.at(table_name).insert_row(data);

    fmt::println("Successfully inserted data into table: '{}' in database with name: '{}'", table_name, this->name);
}

auto Database::get_table_by_name(const std::string &table_name) -> Table& {

    if (!tables.contains(table_name)) throw std::runtime_error(
        "Table with name '" + table_name + "' does not exist in database with name: '" + this->name + "!");


    return tables.at(table_name);
}



