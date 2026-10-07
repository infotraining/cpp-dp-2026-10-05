#include "text.hpp"
#include "shape_factories.hpp"

namespace
{
    using namespace Drawing;

    bool is_registered = 
        SingletonShapeFactory::instance()
            .register_creator(Drawing::Text::id, [](){ return std::make_unique<Drawing::Text>(); });
}
