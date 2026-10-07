#include "composite.hpp"
#include "composite_with_iterator.hpp"

void test_basic_composite()
{

    std::cout << "Testing Basic Composite\n";

    // Create a tree structure
    auto root = std::make_shared<Composite>("root");

    root->add(std::make_shared<Leaf>("Leaf A"));
    root->add(std::make_shared<Leaf>("Leaf B"));

    auto comp = std::make_shared<Composite>("Composite X");
    comp->add(std::make_shared<Leaf>("Leaf XA"));
    comp->add(std::make_shared<Leaf>("Leaf XB"));

    root->add(comp);
    root->add(std::make_shared<Leaf>("Leaf C"));

    // Add and remove a leaf
    auto leaf = std::make_shared<Leaf>("Leaf D");
    root->add(leaf);

    // Recursively display tree
    root->display(1);

    std::cout << "\n\n---------------------\n\n";

    root->remove(leaf);

    root->display(1);
}

void test_composite_with_iterator()
{
    std::cout << "Testing Composite with Iterator\n";

    // Create a tree structure for composite with iterator
    auto root = std::make_shared<CompositeWithIterator::Composite>("root");
    root->add(std::make_shared<Leaf>("Leaf A"));
    root->add(std::make_shared<Leaf>("Leaf B"));

    auto comp = std::make_shared<CompositeWithIterator::Composite>("Composite X");
    comp->add(std::make_shared<Leaf>("Leaf XA"));
    comp->add(std::make_shared<Leaf>("Leaf XB"));
    root->add(comp);

    root->add(std::make_shared<Leaf>("Leaf C"));
    root->add(std::make_shared<Leaf>("Leaf D"));

    std::cout << "\nIterating over children using range-based for loop (non-recursive):\n";
    // Iterate over the children using range-based for loop
    for (const auto& child : *root)
    {
        std::cout << "Leaf: " << child.name() << std::endl;
    }

    std::cout << "\nIterating over the tree using the iterator (non-recursive):\n";
    // Iterate over the tree using the iterator
    for (auto it = root->begin(); it != root->end(); ++it)
    {
        std::cout << "Leaf: " << it->name() << std::endl;
    }

#if __cpp_lib_generator >= 202207L
    std::cout << "\nIterating over the tree recursively using children_recursive():\n";
    for (auto& child : root->children_recursive())
    {
        std::cout << "Leaf: " << child.name() << std::endl;
    }
#endif // __cpp_lib_generator >= 202207L
}

std::generator<int> my_numbers(int max)
{
    for (int i = 0; i < max; ++i)
    {
        co_yield i;
    }
}

int main()
{
    for (auto n : my_numbers(10))
    {
        std::cout << n << " ";
    }
    std::cout << "\n";

    test_basic_composite();

    // test_composite_withP_iterator();
}
