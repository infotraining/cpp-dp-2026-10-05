#ifndef SHAPE_GROUP_HPP
#define SHAPE_GROUP_HPP

#include "shape_reader_factory.hpp"
#include "shapes.hpp"

namespace Drawing
{
    class ShapeGroup : public Shape
    {
    public:
        using iterator = std::vector<std::unique_ptr<Shape>>::iterator;
        using const_iterator = std::vector<std::unique_ptr<Shape>>::const_iterator;

        iterator begin() { return shapes_.begin(); }
        iterator end() { return shapes_.end(); }
        const_iterator begin() const { return shapes_.begin(); }
        const_iterator end() const { return shapes_.end(); }
        size_t size() const { return shapes_.size(); }

        Shape& operator[](size_t index) { return *shapes_[index]; }
        const Shape& operator[](size_t index) const { return *shapes_[index]; }

        ShapeGroup() = default;

        ShapeGroup(std::vector<std::unique_ptr<Shape>> shapes)
            : shapes_(std::move(shapes))
        { }

        void add_shape(std::unique_ptr<Shape> shape)
        {
            shapes_.push_back(std::move(shape));
        }

        void draw() const override
        {
            std::cout << "Drawing ShapeGroup with " << shapes_.size() << " shapes:" << std::endl;
            for (const auto& shape : shapes_)
                shape->draw();
        }

    private:
        std::vector<std::unique_ptr<Shape>> shapes_;
    };

} // namespace Drawing

namespace Drawing::IO
{
    class ShapeGroupTextReader : public IShapeReader
    {
    public:
        explicit ShapeGroupTextReader(ShapeReaderFactory& factory)
            : factory_(factory)
        { }

        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            std::string shapeType;
            size_t shapeCount;

            in >> shapeType >> shapeCount;

            if (shapeType != "ShapeGroup")
                throw std::runtime_error("Invalid shape type");

            std::vector<std::unique_ptr<Shape>> shapes;
            for (size_t i = 0; i < shapeCount; ++i)
            {
                std::string nestedShapeType = peek_shape_type(in);
                if (auto reader = factory_.create(nestedShapeType))
                {
                    shapes.push_back(reader->read(in));
                }
                else
                {
                    throw std::runtime_error("Unknown shape type: " + nestedShapeType);
                }
            }

            return std::make_unique<ShapeGroup>(std::move(shapes));
        }

    private:
        std::string peek_shape_type(std::istream& in) const
        {
            std::streampos pos = in.tellg();
            std::string id;
            in >> id;
            in.seekg(pos);
            return id;
        }

        ShapeReaderFactory& factory_;
    };

    class ShapeGroupJsonReader : public IShapeReader
    {
    public:
        explicit ShapeGroupJsonReader(ShapeReaderFactory& factory)
            : factory_(factory)
        { }

        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            nlohmann::json group_json;
            in >> group_json;

            const auto& groupData = group_json["ShapeGroup"];
            auto group = std::make_unique<ShapeGroup>();

            for (const auto& item : groupData)
            {
                const auto& key = item.begin().key();
                if (auto reader = factory_.create(key))
                {
                    std::stringstream ss;
                    ss << "{\"" << key << "\":" << item[key] << "}";
                    group->add_shape(reader->read(ss));
                }
            }

            return group;
        }

    private:
        ShapeReaderFactory& factory_;
    };
} // namespace Drawing::IO

#endif // SHAPE_GROUP_HPP