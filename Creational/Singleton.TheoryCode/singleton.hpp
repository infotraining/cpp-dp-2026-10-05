#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <iostream>

class Singleton
{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& instance()
    {
        static Singleton unique_instance; // lazy initialization, thread-safe in C++11 and later

        return unique_instance;
    }

    void do_something();

private:
    Singleton() // disallows creation of new instances outside the class
    {
        std::cout << "Constructor of singleton" << std::endl;
    }

    ~Singleton()
    {
        std::cout << "Singleton has been destroyed!" << std::endl;
    }
};

void Singleton::do_something()
{
    std::cout << "Singleton instance at " << std::hex << &instance() << std::endl;
}


template <typename T>
class SingletonHolder
{
    SingletonHolder() = default;
public:
    SingletonHolder(const SingletonHolder&) = delete;
    SingletonHolder& operator=(const SingletonHolder&) = delete;

    static T& instance()
    {
        static T unique_instance; // lazy initialization, thread-safe in C++11 and later

        return unique_instance;
    }
};

#endif /*SINGLETON_HPP_*/
