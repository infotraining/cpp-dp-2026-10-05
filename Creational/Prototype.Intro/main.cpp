#include "point.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

class Shape
{
public:
    virtual void draw() = 0;
    virtual void move(int dx, int dy) = 0;
    virtual std::unique_ptr<Shape> clone() const = 0;
    virtual ~Shape() = default;
};

template <typename T>
struct ShapeTraits
{
    using base_type = Shape;
};



// CRTP (Curiously Recurring Template Parameter) for cloneable shapes
template <typename TShape>
class CloneableShape : public ShapeTraits<TShape>::base_type
{
public:
    using base_type = ShapeTraits<TShape>::base_type;
    using base_type::base_type;

    std::unique_ptr<Shape> clone() const override
    {
        return std::make_unique<TShape>(static_cast<const TShape&>(*this));
    }
};

class Circle : public CloneableShape<Circle>
{
public:
    using base_type = Shape;
    
    Point center_;
    int radius_;

public:
    Circle(int x, int y, int radius)
        : center_{x, y}
        , radius_{radius}
    {
    }

    void draw() override
    {
        std::cout << "Drawing Circle at (" << center_.x << ", " << center_.y << ") with radius " << radius_ << "\n";
    }

    void move(int dx, int dy) override
    {
        center_.x += dx;
        center_.y += dy;
    }
};

struct Color
{
    int r, g, b;

    explicit Color(int red = 0, int green = 0, int blue = 0)
        : r{red}
        , g{green}
        , b{blue}
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        os << "Color(R: " << color.r << ", G: " << color.g << ", B: " << color.b << ")";
        return os;
    }
};

class ColorCircle;

template <>
struct ShapeTraits<ColorCircle>
{
    using base_type = Circle;
};

class ColorCircle : public CloneableShape<ColorCircle>
{
    Color color_;

public:
    using base_type = ShapeTraits<ColorCircle>::base_type;

    ColorCircle(int x, int y, int radius, const Color& color)
        : CloneableShape<ColorCircle>{x, y, radius}
        , color_{color}
    {
    }

    void draw() override
    {
        std::cout << "Setting " << color_ << " & ";
        Circle::draw();
    }

    std::unique_ptr<Shape> clone() const override
    {
        return std::make_unique<ColorCircle>(*this);
    }
};

class Rectangle : public CloneableShape<Rectangle>
{
    Point left_top_;
    int width_, height_;

public:
    Rectangle(int x, int y, int width, int height)
        : left_top_{x, y}
        , width_{width}
        , height_{height}
    {
    }

    void draw() override
    {
        std::cout << "Drawing Rectangle at (" << left_top_.x << ", " << left_top_.y << ") with width " << width_ << " and height " << height_ << "\n";
    }

    void move(int dx, int dy) override
    {
        left_top_.x += dx;
        left_top_.y += dy;
    }

    // std::unique_ptr<Shape> clone() const override
    // {
    //     return std::make_unique<Rectangle>(*this);
    // }
};

class GraphicsDoc
{
    std::vector<std::unique_ptr<Shape>> shapes_;

public:
    GraphicsDoc() noexcept
    {
    }

    GraphicsDoc(const GraphicsDoc& other)
    {
        for (const auto& shape : other.shapes_)
        {
            shapes_.push_back(shape->clone());
        }
    }

    void add_shape(std::unique_ptr<Shape> shape)
    {
        shapes_.push_back(std::move(shape));
    }

    void render() const
    {
        std::cout << "Rendering GraphicsDoc:\n";
        for (const auto& shape : shapes_)
        {
            shape->draw();
        }
    }
};

int main()
{
    GraphicsDoc doc_original;
    doc_original.add_shape(std::make_unique<Circle>(10, 20, 5));
    doc_original.add_shape(std::make_unique<ColorCircle>(15, 25, 10, Color{255, 0, 0}));
    doc_original.add_shape(std::make_unique<Rectangle>(15, 25, 10, 20));
    doc_original.render();

    std::cout << "\n";

    GraphicsDoc doc_copy = doc_original;
    doc_copy.render();
}