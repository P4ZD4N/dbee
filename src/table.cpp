#include "table.h"

auto Table::insert_row(const std::vector<std::string>& data) -> void {
    if (data.size() != column_names.size())
        throw std::invalid_argument("Row size does not match the number of columns!");

    const std::regex integer_regex(R"(^[-+]?[0-9]+$)");
    const std::regex float_regex(R"(^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$)");

    for (int i = 0; i < column_names.size(); i++) {

        switch (column_types.at(i)) {

            case ColumnType::INTEGER: {
                if (!std::regex_match(data.at(i), integer_regex))
                    throw std::invalid_argument(
                        "Value '" + data.at(i) + "' in column '" + column_names.at(i) + "' is not a valid INTEGER!");
                break;
            }

            case ColumnType::FLOAT: {
                if (!std::regex_match(data.at(i), float_regex))
                    throw std::invalid_argument(
                        "Value '" + data.at(i) + "' in column '" + column_names.at(i) + "' is not a valid FLOAT!");
                break;
            }

            case ColumnType::TEXT: break;
        }
    }

    rows.push_back(data);
}