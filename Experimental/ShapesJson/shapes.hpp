#pragma once

#include "point.hpp"

#include <iostream>
#include <memory>
#include <vector>

namespace Drawing
{
    class Shape
    {
    public:
        virtual ~Shape() = default;
        virtual void draw() const = 0;
    };
} // namespace Drawing

namespace Drawing::IO
{
    struct IShapeReader
    {
        virtual ~IShapeReader() = default;
        virtual std::unique_ptr<Shape> read(std::istream& in) const = 0;
    };
} // namespace Drawing::IO