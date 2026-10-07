#ifndef COMPOSITE_WITH_ITERATOR_HPP_
#define COMPOSITE_WITH_ITERATOR_HPP_

#include "composite.hpp"

#include <version>

#if __cpp_lib_generator >= 202207L
#include <generator>
#endif

#include <iterator>
#include <type_traits>

namespace CompositeWithIterator
{
    class Composite : public Component
    {
    private:
        std::list<ComponentPtr> children;

    public:
        explicit Composite(std::string name)
            : Component(std::move(name))
        {
        }

        void add(ComponentPtr c)
        {
            children.push_back(c);
        }

        void remove(ComponentPtr c)
        {
            children.remove(c);
        }

        void display(int depth) override
        {
            std::cout << std::string(depth, '-') << name() << std::endl;

            for (const auto& child : children)
                child->display(depth + 2);
        }

        // Iterator support
        template <std::bidirectional_iterator TIter>
        class Iterator
        {
        private:
            TIter it;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using iterator_concept = std::bidirectional_iterator_tag;
            using value_type = Component;
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<
                std::is_same_v<TIter, std::list<ComponentPtr>::const_iterator>,
                const Component*,
                Component*>;
            using reference = std::conditional_t<
                std::is_same_v<TIter, std::list<ComponentPtr>::const_iterator>,
                const Component&,
                Component&>;

            Iterator() = default;

            explicit Iterator(TIter init)
                : it(init)
            { }

            reference operator*() const { return **it; }

            pointer operator->() const { return std::addressof(**it); }

            Iterator& operator++()
            {
                ++it;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            Iterator& operator--()
            {
                --it;
                return *this;
            }

            Iterator operator--(int)
            {
                Iterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const Iterator& other) const { return it == other.it; }
            bool operator!=(const Iterator& other) const { return it != other.it; }
        };

        using iterator = Iterator<std::list<ComponentPtr>::iterator>;
        using const_iterator = Iterator<std::list<ComponentPtr>::const_iterator>;

        iterator begin() { return iterator(children.begin()); }
        iterator end() { return iterator(children.end()); }

        const_iterator begin() const { return const_iterator(children.begin()); }
        const_iterator end() const { return const_iterator(children.end()); }

        const_iterator cbegin() const { return const_iterator(children.begin()); }
        const_iterator cend() const { return const_iterator(children.end()); }

#if __cpp_lib_generator >= 202207L
        // Support for recursively iterating over all children

        std::generator<Component&> children_recursive()
        {
            for (auto& child : children)
            {
                co_yield *child;

                if (auto composite = dynamic_cast<Composite*>(&*child))
                {
                    co_yield std::ranges::elements_of(composite->children_recursive());
                }
            }
        }

        std::generator<const Component&> children_recursive() const
        {
            for (auto& child : children)
            {
                co_yield *child;

                if (auto composite = dynamic_cast<const Composite*>(&*child))
                {
                    co_yield std::ranges::elements_of(composite->children_recursive());
                }
            }
        }
#endif // __cpp_lib_generator >= 202207L
    };

} // namespace CompositeWithIterator

static_assert(std::bidirectional_iterator<CompositeWithIterator::Composite::iterator>);
static_assert(std::bidirectional_iterator<CompositeWithIterator::Composite::const_iterator>);

#endif /*COMPOSITE_WITH_ITERATOR_HPP_*/