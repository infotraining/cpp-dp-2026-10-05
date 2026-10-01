#include <iostream>

struct Point
{
    int x, y;

    explicit Point(int x_val = 0, int y_val = 0)
        : x{x_val}, y{y_val}
    {
    }

    bool operator==(const Point& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Point& point)
    {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
    }
};