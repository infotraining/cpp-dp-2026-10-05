#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>

class PersonBuilder;

class Person
{
    std::string first_name_;
    std::string last_name_;
    std::string address_, postal_code_, city_;

    std::string company_name_;
    std::string tax_id_;
public:
    Person() = default;

    std::string description() const
    {
        std::string result = first_name_ + " " + last_name_ + " - " + address_ + ", " + postal_code_ + " " + city_;
        result += " - " + company_name_ + " VAT#" + tax_id_;

        return result;
    }

    friend class PersonBuilder;
    friend class PersonAddressBuilder;
    friend class PersonJobBuilder;

    static PersonBuilder create(std::string first_name, std::string last_name);
};

#endif