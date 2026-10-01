#include "graphics_doc.hpp"
#include "shape.hpp"
#include "shape_factories.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

int main()
{
    using namespace Drawing;
    using namespace Drawing::IO;

    std::cout << "Starting GraphicsDoc application...\n";

    GraphicsDoc doc(SingletonShapeFactory::instance(), SingletonShapeRWFactory::instance());
    doc.load_from_file("drawing_prototype_exercise.txt");
    std::cout << "GraphicsDoc loaded...\n";

    std::cout << "Rendering GraphicsDoc...\n";
    doc.render();

    // TODO: Uncomment this code
    // GraphicsDoc backup_doc = doc;
    // backup_doc.render();
    // backup_doc.save_to_file("new_drawing.txt");
}
