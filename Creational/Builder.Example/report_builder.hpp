#ifndef RAPORT_BUILDER_HPP
#define RAPORT_BUILDER_HPP

#include <memory>
#include <string>
#include <vector>

using DataRow = std::vector<std::string>;
using HtmlDocument = std::string;
using MarkdownDocument = std::vector<std::string>;
using CsvDocument = std::vector<std::string>;

namespace Reports
{
    class ReportBuilder
    {
    public:
        virtual ~ReportBuilder() = default;
        virtual ReportBuilder& reset() = 0;
        virtual ReportBuilder& add_title(const std::string& header_text) = 0;
        virtual ReportBuilder& add_headers(const DataRow& headers) = 0;
        virtual ReportBuilder& begin_data() = 0;
        virtual ReportBuilder& add_row(const DataRow& data_row) = 0;
        virtual ReportBuilder& end_data() = 0;
        virtual ReportBuilder& add_footer(const std::string& footer) = 0;
    };

    class HtmlReportBuilder : public ReportBuilder
    {
    public:
        ReportBuilder& reset() override { doc_.clear(); return *this; }
        ReportBuilder& add_title(const std::string& header_text) override;
        ReportBuilder& add_headers(const DataRow& headers) override;
        ReportBuilder& begin_data() override;
        ReportBuilder& add_row(const DataRow& data_row) override;
        ReportBuilder& end_data() override;
        ReportBuilder& add_footer(const std::string& footer) override;

        HtmlDocument get_report();

    private:
        HtmlDocument doc_;
    };

    class MarkdownReportBuilder : public ReportBuilder
    {
    public:
        ReportBuilder& reset() override { doc_.clear(); return *this; }
        ReportBuilder& add_title(const std::string& header_text) override;
        ReportBuilder& add_headers(const DataRow& headers) override;
        ReportBuilder& begin_data() override;
        ReportBuilder& add_row(const DataRow& data_row) override;
        ReportBuilder& end_data() override;
        ReportBuilder& add_footer(const std::string& footer) override;

        MarkdownDocument get_report();

    private:
        MarkdownDocument doc_;
        int column_count_;
    };

    class CsvReportBuilder : public ReportBuilder
    {
    public:
        ReportBuilder& reset() override { doc_.clear(); return *this; }
        ReportBuilder& add_title(const std::string& header_text) override;
        ReportBuilder& add_headers(const DataRow& headers) override;
        ReportBuilder& begin_data() override;
        ReportBuilder& add_row(const DataRow& data_row) override;
        ReportBuilder& end_data() override;
        ReportBuilder& add_footer(const std::string& footer) override;

        CsvDocument get_report();

    private:
        CsvDocument doc_;
    };

} // namespace Reports

#endif // RAPORT_BUILDER_HPP
