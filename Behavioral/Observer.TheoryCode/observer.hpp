#ifndef OBSERVER_HPP_
#define OBSERVER_HPP_

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
                it = observers_.erase(it); // observer no longer exists
            }
        }
    }

private:
    std::set<ObserverPtr, std::owner_less<ObserverPtr>> observers_;
};

#endif /*OBSERVER_HPP_*/
