#include "generic_factory.hpp"
#include "shapes.hpp"

namespace Drawing::IO
{
    using ShapeReaderFactory = GenericFactory<IShapeReader>;
}
