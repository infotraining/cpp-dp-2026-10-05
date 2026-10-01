#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "shapes.hpp"
#include <nlohmann/json.hpp>

namespace Drawing
{
    class Circle : public Shape
    {
    public:
        Circle(Point center, int radius)
            : center_(center)
            , radius_(radius)
        {
        }

        void draw() const override
        {
            std::cout << "Drawing Circle at (" << center_.x << ", " << center_.y << ") with radius " << radius_ << std::endl;
        }

        Point coord() const { return center_; }
        void set_coord(const Point& center) { center_ = center; }

        int radius() const { return radius_; }
        void set_radius(int radius) { radius_ = radius; }

    private:
        Point center_;
        int radius_;
    };
} // namespace Drawing

namespace Drawing::IO
{
    class CircleTextReader : public IShapeReader
    {
    public:
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            std::string shapeType;
            Point coord;
            int radius;

            in >> shapeType >> coord >> radius;

            if (shapeType != "Circle")
                throw std::runtime_error("Invalid shape type");

            return std::make_unique<Circle>(coord, radius);
        }
    };

    struct CircleJsonReader : public IShapeReader
    {
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            nlohmann::json circle_json;
            in >> circle_json;
            const auto& c = circle_json["Circle"];
            Point center{c["Coord"][0].get<int>(), c["Coord"][1].get<int>()};
            int radius = c["Radius"].get<int>();
            return std::make_unique<Circle>(center, radius);
        }
    };
} // namespace Drawing::IO

#endif // CIRCLE_HPP
