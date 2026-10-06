#include "report_builder.hpp"

#include <format>

using namespace std;

////////////////////////////////////////////////////////////////////////////
// HtmlReportBuilder implementation

namespace Reports
{
    ReportBuilder& HtmlReportBuilder::add_title(const std::string& header_text)
    {
        doc_.clear();
        doc_.append("<h1>" + header_text + "</h1>\n");

        return *this;
    }

    ReportBuilder& HtmlReportBuilder::add_headers(const DataRow& headers)
    {
        doc_.append("  <tr>\n");
        for (const auto& item : headers)
        {
            doc_.append("    <th>" + item + "</th>\n");
        }
        doc_.append("  </tr>\n");

        return *this;
    }

    ReportBuilder& HtmlReportBuilder::begin_data()
    {
        doc_.append("<table>\n");

        return *this;
    }

    ReportBuilder& HtmlReportBuilder::add_row(const DataRow& data_row)
    {
        doc_.append("  <tr>\n");
        for (const auto& item : data_row)
        {
            doc_.append("    <td>" + item + "</td>\n");
        }
        doc_.append("  </tr>\n");

        return *this;
    }

    ReportBuilder& HtmlReportBuilder::end_data()
    {
        doc_.append("</table>\n");

        return *this;
    }

    ReportBuilder& HtmlReportBuilder::add_footer(const std::string& footer)
    {
        doc_.append("<div class='footer'>" + footer + "</div>\n");

        return *this;
    }

    HtmlDocument HtmlReportBuilder::get_report()
    {
        return std::move(doc_);
    }

    ///////////////////////////////////////////////////////////////////////////
    // MarkdownReportBuilder implementation

    ReportBuilder& MarkdownReportBuilder::add_title(const std::string& header_text)
    {
        doc_.clear();
        doc_.push_back("# " + header_text);

        return *this;
    }

    ReportBuilder& MarkdownReportBuilder::add_headers(const DataRow& headers)
    {       
        column_count_ = headers.size();
        string md_row;
        for (const auto& item : headers)
        {
            md_row.append(std::format("{:^15}|", item));
        }
        doc_.push_back(md_row);

        // add a separator line after the headers
        string separator(column_count_ * 15 + column_count_, '-');
        doc_.push_back(separator);


        return *this;
    }

    ReportBuilder& MarkdownReportBuilder::begin_data()
    {
        return *this;
    }

    ReportBuilder& MarkdownReportBuilder::add_row(const DataRow& data_row)
    {
        string md_row;

        for (const auto& item : data_row)
        {
            md_row.append(std::format("{:^15}|", item));
        }
        doc_.push_back(md_row);

        return *this;
    }

    ReportBuilder& MarkdownReportBuilder::end_data()
    {
        doc_.push_back("\n");

        return *this;
    }

    ReportBuilder& MarkdownReportBuilder::add_footer(const std::string& footer)
    {
        doc_.push_back("**" + footer + "**");

        return *this;
    }

    MarkdownDocument MarkdownReportBuilder::get_report()
    {
        return std::move(doc_);
    }

    ///////////////////////////////////////////////////////////////////////////
    // CsvReportBuilder implementation

    ReportBuilder& CsvReportBuilder::add_title(const std::string& header_text)
    {
        doc_.clear();
        doc_.push_back("# " + header_text);

        return *this;
    }

    ReportBuilder& CsvReportBuilder::add_headers(const DataRow& headers)
    {
        string csv_row;
        for (const auto& item : headers)
        {
            csv_row.append(item + ";");
        }
        doc_.push_back(csv_row);
        doc_.push_back("---");

        return *this;
    }

    ReportBuilder& CsvReportBuilder::begin_data()
    {
        doc_.push_back("\n");

        return *this;
    }

    ReportBuilder& CsvReportBuilder::add_row(const DataRow& data_row)
    {

        string csv_row;

        for (const auto& item : data_row)
        {
            csv_row.append(item + ";");
        }
        doc_.push_back(csv_row);

        return *this;
    }

    ReportBuilder& CsvReportBuilder::end_data()
    {
        doc_.push_back("\n");

        return *this;
    }

    ReportBuilder& CsvReportBuilder::add_footer(const std::string& footer)
    {
        doc_.push_back("# Summary: " + footer);

        return *this;
    }

    CsvDocument CsvReportBuilder::get_report()
    {
        return std::move(doc_);
    }

} // namespace Reports