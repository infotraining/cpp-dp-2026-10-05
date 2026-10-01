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
    cout << "Start..." << endl;

    GraphicsDoc doc;

    doc.load_from_file("drawing_fm_example.txt");

    cout << "\n";

    doc.render();

    doc.save_to_file("new_drawing.txt");
}
