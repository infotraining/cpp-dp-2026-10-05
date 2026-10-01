#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "shapes.hpp"
#include "square.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

namespace Drawing
{
    class Square : public Shape
    {
    public:
        Square(Point top_left, int size)
            : top_left_(top_left)
            , size_(size)
        {
        }

        void draw() const override
        {
            std::cout << "Drawing Square at (" << top_left_.x << ", " << top_left_.y << ") with size " << size_ << std::endl;
        }

        Point coord() const { return top_left_; }
        void set_coord(const Point& top_left) { top_left_ = top_left; }

        int size() const { return size_; }
        void set_size(int size) { size_ = size; }

    private:
        Point top_left_;
        int size_;
    };
} // namespace Drawing

namespace Drawing::IO
{
    class SquareTextReader : public IShapeReader
    {
    public:
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            std::string shapeType;
            Point coord;
            int size;

            in >> shapeType >> coord >> size;

            if (shapeType != "Square")
                throw std::runtime_error("Invalid shape type");

            return std::make_unique<Square>(coord, size);
        }
    };

    class SquareJsonReader : public IShapeReader
    {
    public:
        std::unique_ptr<Shape> read(std::istream& in) const override
        {
            nlohmann::json square_json;
            in >> square_json;
            const auto& s = square_json["Square"];
            Point topLeft{s["Coord"][0].get<int>(), s["Coord"][1].get<int>()};
            int size = s["Size"].get<int>();
            return std::make_unique<Square>(topLeft, size);
        }
    };

} // namespace Drawing::IO

#endif // SQUARE_HPP
