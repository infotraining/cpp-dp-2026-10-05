#include "shape_reader_factory.hpp"
#include "shapes.hpp"

#include <catch2/catch_test_macros.hpp>

#include "circle.hpp"
#include "rectangle.hpp"
#include "square.hpp"
#include "shape_group.hpp"
#include "shape_text_parser.hpp"

TEST_CASE("parsing Circle from text", "[text]")
{
    using namespace Drawing;
    using namespace Drawing::IO;

    const std::string text = "Circle [10,20] 5";
    std::istringstream text_stream(text);

    CircleTextReader reader;
    std::unique_ptr<Shape> shape = reader.read(text_stream);
    Circle& circle = dynamic_cast<Circle&>(*shape);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);
}

TEST_CASE("parsing Rectangle from text", "[text]")
{
    using namespace Drawing;
    using namespace Drawing::IO;

    const std::string text = "Rectangle [30,40] 10 20";
    std::istringstream text_stream(text);

    RectangleTextReader reader;
    std::unique_ptr<Shape> shape = reader.read(text_stream);
    Rectangle& rectangle = dynamic_cast<Rectangle&>(*shape);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);
}

TEST_CASE("parsing Square from text", "[text]")
{
    using namespace Drawing;
    using namespace Drawing::IO;

    const std::string text = "Square [50,60] 15";
    std::istringstream text_stream(text);

    SquareTextReader reader;
    std::unique_ptr<Shape> shape = reader.read(text_stream);
    Square& square = dynamic_cast<Square&>(*shape);
    REQUIRE(square.coord() == Point{50, 60});
    REQUIRE(square.size() == 15);
}

TEST_CASE("parsing ShapeGroup from text", "[text]")
{
    using namespace Drawing;
    using namespace Drawing::IO;

    const std::string text = R"(
        ShapeGroup 2
        Circle [10,20] 5
        Rectangle [30,40] 10 20
    )";
    std::istringstream text_stream(text);

    ShapeReaderFactory factory;
    factory.register_creator("Circle", []() { return std::make_unique<CircleTextReader>(); });
    factory.register_creator("Rectangle", []() { return std::make_unique<RectangleTextReader>(); });
    factory.register_creator("ShapeGroup", [&factory]() {
        return std::make_unique<ShapeGroupTextReader>(factory);
    });

    ShapeGroupTextReader reader(factory);
    std::unique_ptr<Shape> shapes = reader.read(text_stream);
    
    ShapeGroup& group = dynamic_cast<ShapeGroup&>(*shapes);
    REQUIRE(group.size() == 2);

    Circle& circle = dynamic_cast<Circle&>(group[0]);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);

    Rectangle& rectangle = dynamic_cast<Rectangle&>(group[1]);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);
}


TEST_CASE("parsing shapes from text")
{
    using namespace Drawing;
    using namespace Drawing::IO;

    const std::string text = R"(
        Circle [10,20] 5
        ShapeGroup 2
        Circle [10,20] 5
        Rectangle [23, 34] 20 40
        Rectangle [30,40] 10 20
        Square [50,60] 15
    )";
    std::istringstream text_stream(text);

    ShapeReaderFactory factory;
    factory.register_creator("Circle", []() { return std::make_unique<CircleTextReader>(); });
    factory.register_creator("Rectangle", []() { return std::make_unique<RectangleTextReader>(); });
    factory.register_creator("Square", []() { return std::make_unique<SquareTextReader>(); });
    factory.register_creator("ShapeGroup", [&factory]() {
        return std::make_unique<ShapeGroupTextReader>(factory);
    });

    ShapesTextParser parser(factory);
    std::vector<std::unique_ptr<Shape>> shapes = parser.parse(text_stream);
    REQUIRE(shapes.size() == 4);

    Circle& circle = dynamic_cast<Circle&>(*shapes[0]);
    REQUIRE(circle.coord() == Point{10, 20});
    REQUIRE(circle.radius() == 5);

    ShapeGroup& group = dynamic_cast<ShapeGroup&>(*shapes[1]);
    REQUIRE(group.size() == 2);

    Rectangle& rectangle = dynamic_cast<Rectangle&>(*shapes[2]);
    REQUIRE(rectangle.coord() == Point{30, 40});
    REQUIRE(rectangle.width() == 10);
    REQUIRE(rectangle.height() == 20);

    Square& square = dynamic_cast<Square&>(*shapes[3]);
    REQUIRE(square.coord() == Point{50, 60});
    REQUIRE(square.size() == 15);

    for (const auto& shape : shapes)
    {
        shape->draw();
    }
}
