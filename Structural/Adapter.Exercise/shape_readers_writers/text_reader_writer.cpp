#include "text_reader_writer.hpp"

#include "../shape_factories.hpp"
#include "../text.hpp"

using namespace std;
using namespace Drawing;
using namespace IO;

// TODO: Register creator for a TextReaderWriter class
namespace
{
    bool is_registered = 
        SingletonShapeRWFactory::instance()
            .register_creator(
                Factories::make_type_index<Text>(),
                [](){ return std::make_unique<TextReaderWriter>(); });
}

void TextReaderWriter::read(Shape& shp, istream& in)
{
    Text& text = static_cast<Text&>(shp);

    Point pt;
    in >> pt;
    text.set_coord(pt);

    string str;
    in >> str;
    text.set_text(str);
}

void TextReaderWriter::write(const Shape& shp, ostream& out)
{
    const Text& text = static_cast<const Text&>(shp);
    out << text.coord() << " " << text.text();
}
