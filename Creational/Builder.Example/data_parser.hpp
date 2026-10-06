#ifndef DATA_PARSER_HPP
#define DATA_PARSER_HPP

#include "report_builder.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <optional>
#include <ranges>

namespace Reports
{
    class DataParser
    {
    public:
        explicit DataParser(ReportBuilder& report_builder)
            : report_builder_(report_builder)
        {
        }

        virtual void parse(const std::string& file_name)
        {
            report_builder_.reset();

            report_builder_.add_title(std::string("Raport from file: ") + file_name);

            std::ifstream fin(file_name.c_str());

            auto headers = parse_row(fin);
            if (!headers)
            {
                std::print("No headers found in file: {}\n", file_name);
                throw std::runtime_error("No headers found in file: " + file_name);
            }
            report_builder_.begin_data();

            report_builder_.add_headers(*headers);

            while (std::optional<DataRow> data_row = parse_row(fin))
            {
                report_builder_.add_row(*data_row);
            }

            report_builder_.end_data();

            report_builder_.add_footer("Copyright RaportBuilder 2013");
        }

        virtual ~DataParser() = default;

    protected:
        ReportBuilder& report_builder() { return report_builder_; }
    private:
        ReportBuilder& report_builder_;

        std::optional<DataRow> parse_row(std::istream& in)
        {   
            if (in.eof())
            {
                return std::nullopt;
            }

            // skip lines starting with a dash
            if (in.peek() == '-')
            {
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            std::string row;
            std::getline(in, row);

            std::istringstream iss(row);

            return DataRow(std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>());
        }
    };

    class JsonDataParser : public DataParser
    {
    public:
        explicit JsonDataParser(ReportBuilder& report_builder)
            : DataParser(report_builder)
        {
        }

        void parse(const std::string& file_name) override
        {
            std::ifstream fin(file_name);
            if (!fin.is_open())
            {
                std::print("Could not open file: {}\n", file_name);
                throw std::runtime_error("Could not open file: " + file_name);
            }

            nlohmann::json json_data = nlohmann::json::parse(fin);

            report_builder().add_title(std::string("Raport from file: ") + file_name);

            report_builder().begin_data();

            const auto& headers = json_data.front()["headers"].get<std::vector<std::string>>();
            report_builder().add_headers(headers);

            report_builder().begin_data();

            for (const auto& row_item : json_data | std::views::drop(1))
            {
                DataRow row_values{
                    row_item["firstName"].get<std::string>(),
                    row_item["lastName"].get<std::string>(),
                    row_item["gender"].get<std::string>(),
                    std::to_string(row_item["age"].get<int>()),
                };

                report_builder().add_row(row_values);
            }

            report_builder().end_data();
            report_builder().add_footer("Parsed using nlohmann::json library");
        }
    };

} // namespace Reports

#endif