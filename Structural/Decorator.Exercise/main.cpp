#include "starbugs_coffee.hpp"

#include <memory>

void client(std::unique_ptr<Coffee> coffee)
{
    std::cout << "Description: " << coffee->get_description() << "; Price: " << coffee->get_total_price() << std::endl;
    coffee->prepare();
}

int main()
{
    std::unique_ptr<Coffee> cf = std::make_unique<Espresso>();
    client(std::move(cf));

    CoffeeBuilder coffee_builder;

    coffee_builder.create_coffee<Espresso>()
        .add_condiment<ExtraEspressoCoffee>()
        .add_condiment<WhiskyCoffee>()
        .add_condiment<WhippedCreamCoffee>();

    std::unique_ptr<Coffee> decorated = coffee_builder.build();
    client(std::move(decorated));
}
