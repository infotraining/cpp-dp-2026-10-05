#ifndef SHAPE_PARSERS_HPP
#define SHAPE_PARSERS_HPP

#include "shape_reader_factory.hpp"

namespace Drawing::IO
{
    class ShapesTextParser
    {
        ShapeReaderFactory& factory_;

    private:
        std::string peek_shape_type(std::istream& in) const
        {
            std::streampos pos = in.tellg();
            std::string id;
            in >> id;
            in.seekg(pos);
            return id;
        }

        void skip_whitespace(std::istream& in) const
        {
            while (std::isspace(in.peek()))
                in.get();
        }

    public:
        explicit ShapesTextParser(ShapeReaderFactory& factory)
            : factory_(factory)
        { }

        std::vector<std::unique_ptr<Shape>> parse(std::istream& in) const
        {
            std::vector<std::unique_ptr<Shape>> shapes;

            while (in)
            {
                skip_whitespace(in);
                if (in.eof())
                    break;

                std::string shape_type = peek_shape_type(in);

                if (auto reader = factory_.create(shape_type))
                {
                    shapes.push_back(reader->read(in));
                }
                else
                {
                    throw std::runtime_error("Unknown shape type: " + shape_type);
                }
            }

            return shapes;
        }
    };

} // namespace Drawing::IO

#endif // SHAPE_PARSERS_HPP