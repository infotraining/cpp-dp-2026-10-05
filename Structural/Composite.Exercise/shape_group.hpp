#ifndef SHAPE_GROUP_HPP
#define SHAPE_GROUP_HPP

#include "shape.hpp"

#include <generator>
#include <memory>
#include <vector>

namespace Drawing
{
    // TODO: implement a composite for shapes
    // TODO: implement Iterator pattern for ShapeGroup composite that allows iterating over aggregated shapes
    class ShapeGroup : public CloneableShape<ShapeGroup>
    {
        using PTR = std::shared_ptr<Shape>;
        std::vector<PTR> _shapes;

    public:
        constexpr static auto id = "ShapeGroup";

        ShapeGroup() = default;

        ShapeGroup(const ShapeGroup& other)
        {
            for (const auto& shape : other._shapes)
            {
                _shapes.emplace_back(shape->clone());
            }
        }

        ShapeGroup& operator=(const ShapeGroup& other)
        {
            if (this != &other)
            {
                _shapes.clear();
                for (const auto& shape : other._shapes)
                {
                    _shapes.emplace_back(shape->clone());
                }
            }
            return *this;
        }

        ShapeGroup(ShapeGroup&& other) = default;
        ShapeGroup& operator=(ShapeGroup&& other) = default;

        void add(PTR shape)
        {
            _shapes.emplace_back(shape);
        }

        void remove(PTR shape)
        {
            std::erase(_shapes, shape);
        }

        size_t size() const
        {
            return _shapes.size();
        }

        void clear()
        {
            _shapes.clear();
        }

        void draw() const override
        {
            for (const auto& shape : _shapes)
            {
                shape->draw();
            }
        }

        void move(int x, int y) override
        {
            for (const auto& shape : _shapes)
            {
                shape->move(x, y);
            }
        }

        std::generator<std::shared_ptr<Shape>> shapes() const
        {
            for (const auto& shape : _shapes)
            {
                co_yield shape;
            }
        }
    };
} // namespace Drawing

#endif // SHAPE_GROUP_HPP
