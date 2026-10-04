#include "shape_polymorphic_variant.hpp"
#include "shape_polymorphic_wrappers.hpp"
#include "shape_reflection_wrappers.hpp"

int main(int argc, char* argv[])
{
    using namespace Drawing;

    std::cout << "__cplusplus = " << __cplusplus << "\n";

    PolymorphicVariant::test_shape_polymorphic_variant();

    std::cout << "\n----------------------------\n";

#if __cplusplus >= 202400L

    PolymorphicWrappers::test_shape_polymorphic_wrappers();

    std::cout << "\n----------------------------\n";

    ReflectionPolymorphism::test_shape_reflection_poly();

#endif // __cplusplus >= 202603L
}