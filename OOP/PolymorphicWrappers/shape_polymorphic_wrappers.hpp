#ifndef SHAPE_POLYMORPHIC_WRAPPERS_HPP
#define SHAPE_POLYMORPHIC_WRAPPERS_HPP

#include <memory>
#include <vector>
#include <iostream>

#include "shapes.hpp"

#if __cplusplus >= 202603L

namespace PolymorphicWrappers
{
    ////////////////////////////////////////////////////////////
    // Polymorphic wrapper for shapes
    ////////////////////////////////////////////////////////////

    class Shape
    {
        // inteface for shape wrappers
        class IShape
        {
        public:
            virtual ~IShape() = default;
            virtual void move(int x, int y) = 0;
            virtual void draw() const = 0;
        };

        // template for concrete shape wrappers
        template <typename T>
        class ShapeWrapper : public IShape
        {
            T shape_; // concrete shape instance

        public:
            template <typename U>
                requires (!std::is_same_v<std::decay_t<U>, ShapeWrapper<T>>)
            ShapeWrapper(U &&shp)
                : shape_{std::forward<U>(shp)}
            {
            }

            void draw() const override
            {
                shape_.draw(); // delegate drawing to the concrete shape instance
            }

            void move(int x, int y) override
            {
                shape_.move(x, y); // delegate moving to the concrete shape instance
            }
        };

    public:
        template <typename T, typename... TArgs>
        Shape(std::in_place_t, TArgs &&...args)
            : shape_{std::in_place, std::forward<TArgs>(args)...}
        {
        }

        template <typename U>
            requires(!std::is_same_v<std::decay_t<U>, Shape>)
        Shape(U &&shp)
            : shape_{std::in_place_type<ShapeWrapper<std::decay_t<U>>>, std::forward<U>(shp)}
        {
        }

        void swap(Shape &other)
        {
            shape_.swap(other.shape_);
        }

        void draw() const
        {
            shape_->draw();
        }

        void move(int x, int y)
        {
            shape_->move(x, y);
        }

    private:
        std::polymorphic<IShape> shape_;
    };


    // composite document for shapes
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

    inline void test_shape_polymorphic_wrappers()
    {
        using namespace Drawing;

        Shape shp{Circle(10, 20, 100)};
        shp.draw();
        shp.move(10, 20);
        shp.draw();

        std::cout << "Adding shapes to the document:\n";
        GraphicsDoc doc;
        doc.add(shp);
        doc.add(Circle(1, 1, 20));
        doc.add(Square(2, 3, 10));
        doc.add(Triangle(std::array<Point, 3>{{Point{0, 0}, Point{10, 0}, Point{10, 10}}}));

        std::cout << "Drawing the document:\n";
        doc.draw();

        std::cout << "Moving shapes in the document:\n";
        doc.move(5, 5);
        doc.draw();
    }

} // namespace PolymorphicWrappers

#endif // __cplusplus >= 202603L

#endif