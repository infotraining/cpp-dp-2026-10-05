#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <array>
#include <iostream>
#include <memory>
#include <vector>

namespace Drawing
{
    ////////////////////////////////////////////////////////////
    // Concrete shapes
    ////////////////////////////////////////////////////////////

    struct Circle
    {
        int x, y;
        int r;

        Circle(int x, int y, int r)
            : x{x}
            , y{y}
            , r{r}
        {
        }

        void draw() const
        {
            std::cout << "Circle([" << x << ", " << y << "], " << r << ")\n";
        }

        void move(int dx, int dy)
        {
            x += dx;
            y += dy;
        }
    };

    struct Square
    {
        int x, y;
        int size;

        Square(int x, int y, int size)
            : x{x}
            , y{y}
            , size{size}
        {
        }

        void draw() const
        {
            std::cout << "Square([" << x << ", " << y << "], " << size << ")\n";
        }

        void move(int dx, int dy)
        {
            x += dx;
            y += dy;
        }
    };

    struct Point
    {
        int x, y;
    };

    struct Triangle
    {
        std::array<Point, 3> vertices;

        explicit Triangle(const std::array<Point, 3>& vertices)
            : vertices{vertices}
        {
        }

        void draw() const
        {
            std::cout << "Triangle({ ";
            for (const Point& v : vertices)
                std::cout << "[" << v.x << ", " << v.y << "]"
                          << " ";
            std::cout << "})\n";
        }

        void move(int dx, int dy)
        {
            for (auto& v : vertices)
            {
                v.x += dx;
                v.y += dy;
            }
        }
    };

} // namespace Drawing

#endif