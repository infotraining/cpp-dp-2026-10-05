#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <iostream>
#include <memory>
#include <set>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////
template <typename TSource, typename... TEventArgs>
class Observer
{
public:
    virtual void update(TSource&, TEventArgs... args) = 0;
    virtual ~Observer() = default;
};

//////////////////////////////////////////////////////////////////////////////////////
template <typename TSource, typename... TEventArgs>
struct Observable
{
    using ObserverPtr = std::weak_ptr<Observer<TSource, TEventArgs...>>;

    void subscribe(ObserverPtr observer)
    {
        observers_.insert(std::move(observer));
    }

    void unsubscribe(const ObserverPtr& observer) { observers_.erase(observer); }

protected:
    void notify(TSource& source, TEventArgs... args)
    {
        for (auto it = observers_.begin(); it != observers_.end();)
        {
            if (auto observer = it->lock())
            {
                observer->update(source, args...);
                ++it;
            }
            else
            {
                it = observers_.erase(it); // observer was destroyed - remove its stale entry
            }
        }
    }

private:
    std::set<ObserverPtr, std::owner_less<ObserverPtr>> observers_;
};

// TODO: Make Stock the observable (subject): derive it from Observable so that
// it notifies subscribed observers whenever its price changes.
class Stock
{
private:
    std::string symbol_;
    double price_;
public:
    Stock(const std::string& symbol, double price) : symbol_(symbol), price_(price)
    {
    }

    std::string get_symbol() const
    {
        return symbol_;
    }

    double get_price() const
    {
        return price_;
    }

    void set_price(double price)
    {
        price_ = price;

        // TODO: notify all subscribed observers about the new price
    }
};

// Concrete observer that reacts to stock price changes.
// TODO: derive it from Observer<Stock, ...> so it can subscribe to a Stock.
class Investor 
{
    std::string name_;

public:
    Investor(const std::string& name) : name_(name)
    {
    }

    void update(/*...*/)
    {
        // TODO: implement the callback, e.g. print the investor's name and the stock's symbol and new price
    }
};

#endif /*STOCK_HPP_*/
