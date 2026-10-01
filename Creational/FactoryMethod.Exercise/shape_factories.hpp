#ifndef SHAPE_FACTORIES_HPP
#define SHAPE_FACTORIES_HPP

#include "generic_factory.hpp"
#include "shape.hpp"
#include "shape_readers_writers/shape_reader_writer.hpp"
#include "singleton.hpp"

#include <typeindex>

namespace Drawing
{
    using ShapeFactory = Factories::GenericFactory<Drawing::Shape>;
    using SingletonShapeFactory = SingletonHolder<ShapeFactory>;

    using ShapeRWFactory = Factories::GenericFactory<Drawing::IO::ShapeReaderWriter, std::type_index>;
    using SingletonShapeRWFactory = SingletonHolder<ShapeRWFactory>;
} // namespace Drawing

#endif // SHAPE_FACTORIES_HPP
