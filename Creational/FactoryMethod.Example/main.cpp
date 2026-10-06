#include "rectangle.hpp"
#include "shape.hpp"
#include "shape_readers_writers/rectangle_reader_writer.hpp"
#include "shape_readers_writers/square_reader_writer.hpp"
#include "square.hpp"
#include "graphics_doc.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace Drawing;
using namespace Drawing::IO;

int main()
{
    // bootstrap the shape factory map
    shape_factory.register_factory(Rectangle::id, make_shape<Rectangle>);
    shape_factory.register_factory(Square::id, make_shape<Square>);

    // bootstrap the shape reader/writer factory map
    shape_rw_factory.register_factory(typeid(Rectangle), [] { return std::make_unique<RectangleReaderWriter>(); });
    shape_rw_factory.register_factory(typeid(Square), [] { return std::make_unique<SquareReaderWriter>(); });

    cout << "Start..." << endl;

    GraphicsDoc doc(shape_factory, shape_rw_factory);

    doc.load_from_file("drawing_fm_example.txt");

    cout << "\n";

    doc.render();

    doc.save_to_file("new_drawing.txt");
}
