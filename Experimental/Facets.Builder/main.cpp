#include <iostream>
#include <optional>
#include <string>

#include "person.hpp"
#include "person_builder.hpp"
#include "employee.hpp"

int main()
{
    // clang-format off
    Person p = 
        Person::create("Jan", "Kowalski")
            .lives()
                .at("Westerplatte 2/8")
                .in("Cracow")
                .with_postal_code("31-000")
            .works()
                .in_company("Infotraining")
                .with_tax_id("PL00011100");
    // clang-format on

    std::cout << p.description() << "\n";

    Person other = Person::create("Ewa", "Kowalska")
        .lives()
            .at("Some Street 1/2")
            .in("Some City")
            .with_postal_code("00-000")
        .works()
            .in_company("Some Company")
            .with_tax_id("PL00000000");
        

    std::cout << other.description() << "\n";

    Person third = Person::create("Adam", "Nowak")
        .lives()
            .at("Pilsudskiego 10/4")
            .in("Warsaw")
            .with_postal_code("00-950")
        .works()
            .in_company("TechCorp")
            .with_tax_id("PL12345678");

    std::cout << third.description() << "\n";

    NoBuilder::Employee e1{"Jan", "Kowalski", "jan@x.pl", "123", "Westerplatte 2/8", "31-000", "Cracow",
        "R&D", "Developer", "Ewa Kowalska", 10000.0};
    std::cout << e1.description() << "\n";

    WithBuilder::Employee e2 =
        WithBuilder::Employee::create("Jan", "Kowalski")
            .contact()
                .email("jan@x.pl")
                .phone("123")
            .lives()
                .at("Westerplatte 2/8")
                .in("Cracow")
                .with_postal_code("31-000")
            .works()
                .in_department("R&D")
                .as("Developer")
                .reporting_to("Ewa Kowalska")
                .earning(10000.0);
    std::cout << e2.description() << "\n";

    WithBuilder::Employee e3 =
        WithBuilder::Employee::create("Ewa", "Kowalska")
            .lives()
                .at("Some Street 1/2")
                .in("Some City")
                .with_postal_code("00-000")
            .works()
                .in_department("HR")
                .as("Manager")
                .reporting_to("Jan Kowalski")
                .earning(12000.0);
    std::cout << e3.description() << "\n";
}
