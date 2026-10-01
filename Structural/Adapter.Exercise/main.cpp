#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "shape.hpp"
#include "shape_factories.hpp"
#include "graphics_doc.hpp"

using namespace std;
using namespace Drawing;
using namespace Drawing::IO;

int main()
{
    cout << "Start..." << endl;

    Drawing::GraphicsDoc doc(SingletonShapeFactory::instance(), SingletonShapeRWFactory::instance());

    doc.load_from_file("drawing_adapter.txt");

    cout << "\n";

    doc.render();

    doc.save_to_file("new_drawing_adapter.txt");
}
