#include "person.hpp"
#include "person_builder.hpp"

PersonBuilder Person::create(std::string first_name, std::string last_name)
{
    return PersonBuilder{std::move(first_name), std::move(last_name)};
}