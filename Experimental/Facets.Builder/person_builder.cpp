#include "person_builder.hpp"

PersonJobBuilder PersonAddressBuilder::works() { return PersonJobBuilder{person_}; }

PersonAddressBuilder PersonJobBuilder::lives() { return PersonAddressBuilder{person_}; }