#ifndef GRAPHICS_DOC_HPP
#define GRAPHICS_DOC_HPP

#include "shape.hpp"
#include "shape_readers_writers/rectangle_reader_writer.hpp"
#include "shape_readers_writers/square_reader_writer.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Drawing
{
    template <typename TProduct, typename TId = std::string, typename TCreator = std::function<std::unique_ptr<TProduct>()>>
    class GenericFactory
    {
        std::unordered_map<TId, TCreator> map_;

    public:
        bool register_factory(const TId& id, TCreator factory)
        {
            return map_.emplace(id, std::move(factory)).second;
        }

        std::unique_ptr<TProduct> create(const TId& id)
        {
            auto it = map_.find(id);
            if (it != map_.end())
            {
                return it->second();
            }
            throw std::runtime_error("Unknown type id");
        }
    };

    using ShapeFactory = GenericFactory<Shape>;
    using ShapeRWFactory = GenericFactory<IO::ShapeReaderWriter, std::type_index>;

    template <typename TShape>
    std::unique_ptr<Shape> make_shape()
    {
        return std::make_unique<TShape>();
    }

    inline ShapeFactory shape_factory;
    inline ShapeRWFactory shape_rw_factory;

    // Static factories for creating shapes and their corresponding reader/writer objects
    // std::unique_ptr<Shape> create_shape(const std::string& id)
    // {
    //     if (id == Rectangle::id)
    //         return std::make_unique<Rectangle>();
    //     else if (id == Square::id)
    //         return std::make_unique<Square>();

    //     throw std::runtime_error("Unknown shape id");
    // }

    // std::unique_ptr<IO::ShapeReaderWriter> create_shape_rw(Shape& shape)
    // {
    //     using namespace IO;

    //     if (typeid(shape) == typeid(Rectangle))
    //         return std::make_unique<RectangleReaderWriter>();
    //     else if (typeid(shape) == typeid(Square))
    //         return std::make_unique<SquareReaderWriter>();

    //     throw std::runtime_error("Unknown shape id");
    // }

    class GraphicsDoc
    {
        std::vector<std::unique_ptr<Shape>> shapes_;
        ShapeFactory& shape_factory_;
        ShapeRWFactory& shape_rw_factory_;

    public:
        GraphicsDoc(ShapeFactory& shape_factory, ShapeRWFactory& shape_rw_factory)
            : shape_factory_(shape_factory)
            , shape_rw_factory_(shape_rw_factory)
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

            while (in_stream)
            {
                std::string shape_id;
                in_stream >> shape_id;

                if (!in_stream)
                    return;

                std::cout << "Loading " << shape_id << "..." << std::endl;

                auto shape = shape_factory_.create(shape_id);
                auto shape_rw = shape_rw_factory_.create(typeid(*shape));

                shape_rw->read(*shape, in_stream);

                shapes_.push_back(std::move(shape));
            }
        }

        void save_to_stream(std::ostream& out_stream)
        {
            for (const auto& shape : shapes_)
            {
                auto shape_rw = shape_rw_factory_.create(typeid(*shape));
                shape_rw->write(*shape, out_stream);
            }
        }

        void render()
        {
            for (const auto& shape : shapes_)
            {
                shape->draw();
            }
        }
    };

} // namespace Drawing

#endif // GRAPHICS_DOC_HPP