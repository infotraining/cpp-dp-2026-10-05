#include "shape_group_reader_writer.hpp"

using namespace std;
using namespace Drawing;
using namespace Drawing::IO;

namespace
{
    // TODO: Register creator for ShapeGroupReaderWriter
    bool is_registered = SingletonShapeRWFactory::instance()
                             .register_creator(
                                 Factories::make_type_index<ShapeGroup>(),
                                 []() -> std::unique_ptr<ShapeReaderWriter> {
                                     return std::make_unique<ShapeGroupReaderWriter>(SingletonShapeFactory::instance(), SingletonShapeRWFactory::instance());
                                 });
} // namespace

namespace Drawing::IO
{
    void ShapeGroupReaderWriter::read(Shape& shp, std::istream& in)
    {
        ShapeGroup& group = dynamic_cast<ShapeGroup&>(shp);

        size_t shape_count = 0;
        in >> shape_count;

        for (size_t i = 0; i < shape_count; ++i)
        {
            std::string shape_type_id;
            in >> shape_type_id;
            auto shape = shape_factory_.create(shape_type_id);

            std::unique_ptr<ShapeReaderWriter> shape_rw = shape_rw_factory_.create(Factories::make_type_index(*shape));
            shape_rw->read(*shape, in);
            group.add(std::move(shape));
        }
    }

    void ShapeGroupReaderWriter::write(const Shape& shp, std::ostream& out)
    {
        const ShapeGroup& group = dynamic_cast<const ShapeGroup&>(shp);

        out << group.id << " " << group.size() << "\n";
        for (const auto& shape : group.shapes())
        {
            std::unique_ptr<ShapeReaderWriter> shape_rw = shape_rw_factory_.create(Factories::make_type_index(*shape));
            shape_rw->write(*shape, out);
        }
    }
} // namespace Drawing::IO