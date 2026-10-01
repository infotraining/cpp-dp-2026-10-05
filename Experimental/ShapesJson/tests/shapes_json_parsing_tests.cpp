#include <catch2/catch_test_macros.hpp>

#include "circle.hpp"
#include "rectangle.hpp"
#include "square.hpp"
#include "shape_group.hpp"
#include "shape_json_parser.hpp"

#include <sstream>

using namespace Drawing;
using namespace Drawing::IO;

TEST_CASE("parsing Circle from json", "[json]")
{
    const std::string json = R"({"Circle": {"Coord": [10, 20], "Radius": 5}})";
    std::istringstream json_stream(json);

    CircleJsonReader reader;
    std::unique_ptr<Shape> shape = reader.read(json_stream);
    Circle& circle = dynamic_cast<Circle&>(*shape);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);
}

TEST_CASE("parsing Rectangle from json", "[json]")
{
    const std::string json = R"({"Rectangle": {"Coord": [30, 40], "Width": 10, "Height": 20}})";
    std::istringstream json_stream(json);

    RectangleJsonReader reader;
    std::unique_ptr<Shape> shape = reader.read(json_stream);
    Rectangle& rectangle = dynamic_cast<Rectangle&>(*shape);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);
}

TEST_CASE("parsing Square from json", "[json]")
{
    const std::string json = R"({"Square": {"Coord": [50, 60], "Size": 15}})";
    std::istringstream json_stream(json);

    SquareJsonReader reader;
    std::unique_ptr<Shape> shape = reader.read(json_stream);
    Square& square = dynamic_cast<Square&>(*shape);
    REQUIRE(square.coord() == Point{50, 60});
    REQUIRE(square.size() == 15);
}

TEST_CASE("parsing nested shapes from json", "[json]")
{
    const std::string json = R"([{"ShapeGroup": [{"Circle": {"Coord": [10, 20], "Radius": 5}}, {"Rectangle": {"Coord": [30, 40], "Width": 10, "Height": 20}}]}])";

    ShapeReaderFactory factory;
    factory.register_creator("Circle", []() { return std::make_unique<CircleJsonReader>(); });
    factory.register_creator("Rectangle", []() { return std::make_unique<RectangleJsonReader>(); });
    factory.register_creator("ShapeGroup", [&factory]() {
        return std::make_unique<ShapeGroupJsonReader>(factory);
    });

    ShapesJsonParser parser(factory);

    std::istringstream json_stream(json);
    std::vector<std::unique_ptr<Shape>> shapes = parser.parse(json_stream);

    REQUIRE(shapes.size() == 1);
    ShapeGroup& group = dynamic_cast<ShapeGroup&>(*shapes[0]);

    Circle& circle = dynamic_cast<Circle&>(group[0]);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);

    Rectangle& rectangle = dynamic_cast<Rectangle&>(group[1]);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);

    for (const auto& shape : group)
    {
        shape->draw();
    }
}

TEST_CASE("parse json - nlohmann json", "[json]")
{
    const std::string json = R"([
        { "Circle": { "Coord": [10, 20], "Radius": 5 } },
        { "Rectangle": { "Coord": [30, 40], "Width": 10, "Height": 20 } },
        { "Square": { "Coord": [50, 60], "Size": 15 } },
        { "ShapeGroup": [
            { "Circle": { "Coord": [10, 20], "Radius": 5 } },
            { "Rectangle": { "Coord": [30, 40], "Width": 10, "Height": 20 } }]
        }
    ])";

    ShapeReaderFactory factory;
    factory.register_creator("Circle", []() { return std::make_unique<CircleJsonReader>(); });
    factory.register_creator("Rectangle", []() { return std::make_unique<RectangleJsonReader>(); });
    factory.register_creator("Square", []() { return std::make_unique<SquareJsonReader>(); });
    factory.register_creator("ShapeGroup", [&factory]() {
        return std::make_unique<ShapeGroupJsonReader>(factory);
    });

    ShapesJsonParser parser(factory);

    std::istringstream json_stream(json);
    std::vector<std::unique_ptr<Shape>> shapes = parser.parse(json_stream);

    REQUIRE(shapes.size() == 4);
    Circle& circle = dynamic_cast<Circle&>(*shapes[0]);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);

    Rectangle& rectangle = dynamic_cast<Rectangle&>(*shapes[1]);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);

    Square& square = dynamic_cast<Square&>(*shapes[2]);
    REQUIRE(square.coord() == Point{50, 60});
    REQUIRE(square.size() == 15);

    ShapeGroup& group = dynamic_cast<ShapeGroup&>(*shapes[3]);
    Circle& groupCircle = dynamic_cast<Circle&>(group[0]);
    REQUIRE(groupCircle.coord() == Point{10, 20});
    REQUIRE(groupCircle.radius() == 5);
    Rectangle& groupRectangle = dynamic_cast<Rectangle&>(group[1]);
    REQUIRE(groupRectangle.coord() == Point{30, 40});
    REQUIRE(groupRectangle.width() == 10);
    REQUIRE(groupRectangle.height() == 20);
}
