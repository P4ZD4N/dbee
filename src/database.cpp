#include "database.h"

#include <stdexcept>
#include <vector>

std::unordered_map<std::string, Database> Database::databases;

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

auto Database::get_table_by_name(const std::string& table_name) -> Table& {

    if (!tables.contains(table_name)) throw std::runtime_error(
        "Table with name '" + table_name + "' does not exist in database with name: '" + this->name + "!");

    return tables.at(table_name);
}

auto Database::create_database(const std::string& database_name) -> void {

    if (databases.contains(database_name)) throw std::runtime_error(
        "Database with name '" + database_name + "' already exists!");

    auto new_db = Database(database_name);
}

auto Database::get_database(const std::string& database_name) -> Database& {

    if (!databases.contains(database_name)) throw std::runtime_error(
        "Database with name '" + database_name + "' does not exist!");

    return databases.at(database_name);
}

auto Database::drop_database(const std::string &database_name) -> void {

    if (!databases.contains(database_name)) throw std::runtime_error(
        "Database with name '" + database_name + "' does not exist!");

    databases.erase(database_name);

    fmt::println("Successfully dropped database with name: '{}'", database_name);
}

