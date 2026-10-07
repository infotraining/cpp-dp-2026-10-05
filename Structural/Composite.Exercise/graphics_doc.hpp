#ifndef GRAPHICS_DOC_HPP
#define GRAPHICS_DOC_HPP

#include "shape.hpp"
#include "shape_factories.hpp"
#include "shape_readers_writers/shape_group_reader_writer.hpp"
#include "shape_group.hpp"
#include "shape_readers_writers/circle_reader_writer.hpp"
#include "shape_readers_writers/rectangle_reader_writer.hpp"
#include "shape_readers_writers/square_reader_writer.hpp"

#include <cassert>
#include <fstream>

namespace Drawing
{
    class GraphicsDoc
    {
        ShapeGroup shapes_;
        ShapeFactory& shape_factory_;
        ShapeRWFactory& shape_rw_factory_;

    public:
        GraphicsDoc(ShapeFactory& shape_factory, ShapeRWFactory& shape_rw_factory)
            : shape_factory_(shape_factory), shape_rw_factory_(shape_rw_factory)
        {
        }

        void load_from_file(const std::string& filename)
        {
            std::ifstream file_in{filename};

            if (!file_in)
            {
                std::cout << "File not found!" << std::endl;
                exit(1);
            }

            shapes_.clear();
            load_from_stream(file_in);
        }

        void save_to_file(const std::string& filename)
        {
            std::ofstream file_out{filename};
            if (!file_out)
            {
                std::cout << "Failed to open file for writing!" << std::endl;
                exit(1);
            }
            save_to_stream(file_out);
        }

        void load_from_stream(std::istream& in_stream)
        {
            assert(in_stream);

            shapes_.clear();

            std::string shape_id;
            in_stream >> shape_id;

            assert(shape_id == ShapeGroup::id);

            IO::ShapeGroupReaderWriter group_rw(shape_factory_, shape_rw_factory_);
            group_rw.read(shapes_, in_stream);
        }

        void save_to_stream(std::ostream& out_stream)
        {
            IO::ShapeGroupReaderWriter group_rw(shape_factory_, shape_rw_factory_);
            group_rw.write(shapes_, out_stream);
        }

        void render()
        {
            shapes_.draw();
        }
    };

} // namespace Drawing

#endif // GRAPHICS_DOC_HPP