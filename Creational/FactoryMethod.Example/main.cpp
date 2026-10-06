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
    shape_factory_map.register_factory(Rectangle::id, make_shape<Rectangle>);
    shape_factory_map.register_factory(Square::id, make_shape<Square>);

    cout << "Start..." << endl;

    GraphicsDoc doc(shape_factory_map);

    doc.load_from_file("drawing_fm_example.txt");

    cout << "\n";

    doc.render();

    doc.save_to_file("new_drawing.txt");
}
