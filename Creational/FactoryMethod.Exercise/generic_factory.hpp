#ifndef GENERIC_FACTORY_HPP
#define GENERIC_FACTORY_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>

namespace Factories
{
    template <typename ProductType, typename IdType = std::string, typename CreatorType = std::function<std::unique_ptr<ProductType>()>>
    class GenericFactory
    {
        std::unordered_map<IdType, CreatorType> creators_;

    public:
        bool register_creator(IdType id, CreatorType creator)
        {
            bool is_inserted;
            std::tie(std::ignore, is_inserted) = creators_.insert(std::make_pair(std::move(id), std::move(creator)));

            return is_inserted;
        }

        std::unique_ptr<ProductType> create(const IdType& id) const
        {
            auto& creator = creators_.at(id);

            return creator();
        }
    };


    // Creates a type index for the given object.
    template <typename T>
    std::type_index make_type_index(const T& obj)
    {
        return std::type_index(typeid(obj));
    }

    // Creates a type index for the given type.
    template <typename T>
    std::type_index make_type_index()
    {
        return std::type_index(typeid(T));
    }

} // namespace Factories

#endif // GENERIC_FACTORY_HPP
