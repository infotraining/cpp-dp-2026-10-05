#ifndef COFFEEHELL_HPP_
#define COFFEEHELL_HPP_

#include <iostream>
#include <string>

class Coffee
{
public:
    virtual ~Coffee() = default;

    virtual float get_total_price() const = 0;
    virtual std::string get_description() const = 0;
    virtual void prepare() = 0;
};

class CoffeeBase : public Coffee
{
    float price_;
    std::string description_;

public:
    CoffeeBase(float price, const std::string& description)
        : price_{price}
        , description_{description}
    {
    }

    float get_total_price() const override
    {
        return price_;
    }

    std::string get_description() const override
    {
        return description_;
    }
};

class Espresso : public CoffeeBase
{
public:
    Espresso(float price = 4.0, const std::string& description = "Espresso")
        : CoffeeBase{price, description}
    {
    }

    void prepare() override
    {
        std::cout << "Making a perfect espresso: 7 g, 15 bar and 24 sec.\n";
    }
};

class Americano : public CoffeeBase
{
public:
    Americano(float price = 6.0, const std::string& description = "Americano")
        : CoffeeBase{price, description}
    {
    }

    void prepare() override
    {
        std::cout << "Making a perfect americano.\n";
    }
};

class Decafeinated : public CoffeeBase
{
public:
    Decafeinated(float price = 5.0, const std::string& description = "Decafeinated Coffee")
        : CoffeeBase{price, description}
    {
    }

    void prepare() override
    {
        std::cout << "Making a perfect decafeinated coffee.\n";
    }
};

// TODO: Add condiments that can be added to coffee: WhippedCream: 2.5$, Whisky: 6.0$, ExtraEspresso: 4.0$
// Hint#1: Add CoffeeDecorator and concrete decorators for condiments

class CoffeeDecorator : public Coffee
{
    std::unique_ptr<Coffee> coffee_;

public:
    CoffeeDecorator(std::unique_ptr<Coffee> coffee)
        : coffee_{std::move(coffee)}
    { }

    float get_total_price() const override
    {
        return coffee_->get_total_price();
    }

    std::string get_description() const override
    {
        return coffee_->get_description();
    }

    void prepare() override
    {
        coffee_->prepare();
    }
};

class WhippedCreamCoffee : public CoffeeDecorator
{
public:
    WhippedCreamCoffee(std::unique_ptr<Coffee> coffee)
        : CoffeeDecorator{std::move(coffee)}
    { }

    float get_total_price() const override
    {
        return CoffeeDecorator::get_total_price() + 2.5;
    }

    std::string get_description() const override
    {
        return CoffeeDecorator::get_description() + ", Whipped Cream";
    }

    void prepare() override
    {
        CoffeeDecorator::prepare();
        std::cout << "Adding whipped cream.\n";
    }
};

class WhiskyCoffee : public CoffeeDecorator
{
public:
    WhiskyCoffee(std::unique_ptr<Coffee> coffee)
        : CoffeeDecorator{std::move(coffee)}
    { }

    float get_total_price() const override
    {
        return CoffeeDecorator::get_total_price() + 6.0;
    }

    std::string get_description() const override
    {
        return CoffeeDecorator::get_description() + ", Whisky";
    }

    void prepare() override
    {
        CoffeeDecorator::prepare();
        std::cout << "Adding whisky.\n";
    }
};

class ExtraEspressoCoffee : public CoffeeDecorator
{
public:
    ExtraEspressoCoffee(std::unique_ptr<Coffee> coffee)
        : CoffeeDecorator{std::move(coffee)}
    { }

    float get_total_price() const override
    {
        return CoffeeDecorator::get_total_price() + 4.0;
    }

    std::string get_description() const override
    {
        return CoffeeDecorator::get_description() + ", Extra Espresso";
    }

    void prepare() override
    {
        CoffeeDecorator::prepare();
        std::cout << "Adding extra espresso.\n";
    }
};

class CoffeeBuilder
{
    std::unique_ptr<Coffee> coffee_;

public:
    CoffeeBuilder() = default;

    // CoffeeBuilder& add_whipped_cream()
    // {
    //     coffee_ = std::make_unique<WhippedCream>(std::move(coffee_));
    //     return *this;
    // }

    // CoffeeBuilder& add_whisky()
    // {
    //     coffee_ = std::make_unique<Whisky>(std::move(coffee_));
    //     return *this;
    // }

    // CoffeeBuilder& add_extra_espresso()
    // {
    //     coffee_ = std::make_unique<ExtraEspresso>(std::move(coffee_));
    //     return *this;
    // }

    template <typename TCoffee, typename... TArgs>
        requires std::is_base_of_v<Coffee, TCoffee> && !std::is_base_of_v<CoffeeDecorator, TCoffee>
    CoffeeBuilder& create_coffee(TArgs&&... args)
    {
        coffee_ = std::make_unique<TCoffee>(std::forward<TArgs>(args)...);
        return *this;
    }

    template <typename TCondiment, typename... TArgs>
        requires std::is_base_of_v<CoffeeDecorator, TCondiment>
    CoffeeBuilder& add_condiment(TArgs&&... args)
    {
        coffee_ = std::make_unique<TCondiment>(std::move(coffee_), std::forward<TArgs>(args)...);
        return *this;
    }

    std::unique_ptr<Coffee> build()
    {
        return std::move(coffee_);
    }
};

#endif /*COFFEEHELL_HPP_*/
