#include "shape_polymorphic_variant.hpp"
#include "shape_polymorphic_wrappers.hpp"
#include "shapes.hpp"

#if __cplusplus >= 202603L

#include <protocol.hh>
#include <variant>

namespace ReflectionPolymorphism
{
    struct IShape
    {
        void draw() const;
        void move(int dx, int dy);
    };

    template <typename T>
    concept ShapeType = requires(T a, int dx, int dy) {
        { a.draw() } -> std::same_as<void>;
        { a.move(dx, dy) } -> std::same_as<void>;
    };

    using Shape = xyz::reflection::protocol<IShape>;

    // composite document for shapes
    class GraphicsDoc
    {
        std::vector<Shape> shapes_;

    public:
        GraphicsDoc() = default;

        void add(Shape shp)
        {
            shapes_.push_back(shp);
        }

        template <ShapeType T, typename... TArgs>
        void add(TArgs&&... args)
        {
            shapes_.emplace_back(T(std::forward<TArgs>(args)...));
        }

        void draw() const
        {
            std::cout << "GraphicsDoc{\n";
            for (const auto& shp : shapes_)
            {
                std::cout << " + ";
                shp.draw();
            }
            std::cout << "}\n";
        }

        void move(int dx, int dy)
        {
            for (auto& shp : shapes_)
                shp.move(dx, dy);
        }
    };

    inline void test_shape_reflection_poly()
    {
        using namespace Drawing;

        Shape shp{Circle(10, 20, 100)};
        shp.draw();
        shp.move(10, 20);
        shp.draw();

        std::cout << "Adding shapes to the document:\n";
        GraphicsDoc doc;
        doc.add(shp);
        doc.add<Circle>(1, 1, 20);
        doc.add<Square>(2, 3, 10);
        doc.add<Triangle>(std::array<Point, 3>{{Point{0, 0}, Point{10, 0}, Point{10, 10}}});

        std::cout << "Drawing the document:\n";
        doc.draw();

        std::cout << "Moving shapes in the document:\n";
        doc.move(5, 5);
        doc.draw();
    }
} // namespace ReflectionPolymorphism

#endif // __cplusplus >= 202603L