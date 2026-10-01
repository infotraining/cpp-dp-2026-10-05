#ifndef SHAPE_POLYMORPHIC_VARIANT_HPP
#define SHAPE_POLYMORPHIC_VARIANT_HPP

#include "shapes.hpp"

#include <memory>
#include <variant>

namespace PolymorphicVariant
{
    /////////////////////////////////////////////////////////////////////////////
    // Restricted polymorphism with std::variant

    template <typename T>
    concept ShapeType = requires(T a, int dx, int dy) {
        { a.draw() } -> std::same_as<void>;
        { a.move(dx, dy) } -> std::same_as<void>;
    };

    template <typename... Ts>
    class ShapeVariant
    {
        static_assert((ShapeType<Ts> && ...), "All types must satisfy the ShapeType concept");

        using ShapeVariant_t = std::variant<Ts...>;
        ShapeVariant_t shape_;

    public:
        template <typename U>
            requires(!std::is_same_v<std::decay_t<U>, ShapeVariant<Ts...>>)
        ShapeVariant(U &&shp)
            : shape_{std::forward<U>(shp)}
        {
        }

        void draw() const
        {
            std::visit([](const auto &s)
                       { s.draw(); }, shape_);
        }

        void move(int dx, int dy)
        {
            std::visit([dx, dy](auto &s)
                       { s.move(dx, dy); }, shape_);
        }
    };

    // alias for the restricted polymorphic shape variant
    using Shape = ShapeVariant<Drawing::Circle, Drawing::Triangle, Drawing::Square>;

    class GraphicsDoc
    {
        std::vector<Shape> shapes_;

    public:
        GraphicsDoc() = default;

        GraphicsDoc(const std::vector<Shape> &shapes)
            : shapes_(shapes)
        {
        }

        void add(Shape shp)
        {
            shapes_.push_back(shp);
        }

        void draw() const
        {
            std::cout << "GraphicsDoc{\n";
            for (const auto &shp : shapes_)
            {
                std::cout << " + ";
                shp.draw();
            }
            std::cout << "}\n";
        }

        void move(int dx, int dy)
        {
            for (auto &shp : shapes_)
                shp.move(dx, dy);
        }
    };

    inline void test_shape_polymorphic_variant()
    {
        using namespace Drawing;

        Shape shape = Circle(10, 20, 100);
        shape.draw();
        shape.move(10, 20);
        shape.draw();

        shape = Triangle(std::array<Point, 3>{{Point{0, 0}, Point{10, 0}, Point{10, 10}}});
        shape.draw();

        shape = Square(5, 5, 50);
        shape.draw();

        GraphicsDoc doc;
        doc.add(shape);
        doc.add(Circle(1, 1, 20));
        doc.add(Square(2, 3, 10));
        doc.add(Triangle(std::array<Point, 3>{{Point{0, 0}, Point{10, 0}, Point{10, 10}}}));

        std::cout << "Drawing the document:\n";
        doc.draw();

        std::cout << "Moving shapes in the document:\n";
        doc.move(5, 5);
        doc.draw();
    }
} // namespace PolymorphicVariant

#endif