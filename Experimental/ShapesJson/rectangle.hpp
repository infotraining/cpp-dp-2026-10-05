#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "shapes.hpp"
#include "rectangle.hpp"
#include <nlohmann/json.hpp>

#include <iostream>
#include <ostream>

namespace Drawing
{
    class Rectangle : public Shape
    {
    public:
        Rectangle(Point topLeft, int width, int height)
            : top_left_(topLeft)
            , width_(width)
            , height_(height)
        {
        }

        void draw() const override
        {
            std::cout << "Drawing Rectangle at (" << top_left_.x << ", " << top_left_.y << ") with width " << width_ << " and height " << height_ << std::endl;
        }

        Point coord() const { return top_left_; }
        void set_coord(const Point& topLeft) { top_left_ = topLeft; }

        int width() const { return width_; }
        void set_width(int width) { width_ = width; }

        int height() const { return height_; }
        void set_height(int height) { height_ = height; }

    private:
        Point top_left_;
        int width_;
        int height_;
    };
} // namespace Drawing

namespace Drawing::IO
{
    class RectangleTextReader : public IShapeReader
    {
    public:
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            std::string shapeType;
            Point coord;
            int width, height;

            in >> shapeType >> coord >> width >> height;

            if (shapeType != "Rectangle")
                throw std::runtime_error("Invalid shape type");

            return std::make_unique<Rectangle>(coord, width, height);
        }
    };

    struct RectangleJsonReader : public IShapeReader
    {
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            nlohmann::json rect_json;
            in >> rect_json;

            const auto& r = rect_json["Rectangle"];
            Point topLeft{r["Coord"][0].get<int>(), r["Coord"][1].get<int>()};
            int width = r["Width"].get<int>();
            int height = r["Height"].get<int>();
            return std::make_unique<Rectangle>(topLeft, width, height);
        }
    };

} // namespace Drawing::IO

#endif // RECTANGLE_HPP