#include "report_builder.hpp"
#include "data_parser.hpp"
#include <fstream>
#include <iostream>

using namespace std;

constexpr auto txt_file_name = "data_builder.txt";
constexpr auto json_file_name = "data_builder.json";

using namespace Reports;

HtmlDocument build_html_document()
{
    HtmlReportBuilder html_builder;

    DataParser parser(html_builder);
    parser.parse(txt_file_name);

    return html_builder.get_report();
}

MarkdownDocument build_markdown_document()
{
    MarkdownReportBuilder md_builder;

    JsonDataParser parser(md_builder);
    parser.parse(json_file_name);

    // DataParser parser(md_builder);
    // parser.parse(txt_file_name);

    return md_builder.get_report();
}

CsvDocument build_csv_document()
{
    CsvReportBuilder csv_builder;

    DataParser parser(csv_builder);
    parser.parse(txt_file_name);

    return csv_builder.get_report();
}

int main()
{
    HtmlDocument doc_html = build_html_document();

    cout << doc_html << endl;

    cout << "\n\n///////////////////////////////////////////////////////////\n";

    MarkdownDocument md_doc = build_markdown_document();
    for (const auto& line : md_doc)
        cout << line << endl;

    cout << "\n\n///////////////////////////////////////////////////////////\n";

    CsvDocument csv_doc = build_csv_document();

    for (const auto& line : csv_doc)
        cout << line << endl;

    cout << "\n\n///////////////////////////////////////////////////////////\n";

    MarkdownReportBuilder md_builder;

    md_builder
        .add_title("TITLE")
            .add_headers({"Header 1", "Header 2", "Header 3"})
            .begin_data()
                .add_row({"Row1Col1", "Row1Col2", "Row1Col3"})
                .add_row({"Row2Col1", "Row2Col2", "Row2Col3"})
            .end_data()
            .add_footer("Table 1.1");

    MarkdownDocument md_doc2 = md_builder.get_report();
}
