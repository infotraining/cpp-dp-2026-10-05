#include "singleton.hpp"
#include <iostream>

using namespace std;

class Logger
{
public:
    void log(const string& message)
    {
        cout << "Log: " << message << endl;
    }
};

using LoggerSingleton = SingletonHolder<Logger>;

void beware_of_implicit_dependencies()
{
    // This function demonstrates the problem of implicit dependencies when using singletons.
    // If we call LoggerSingleton::instance().log() here, it will create a dependency on LoggerSingleton,
    // which may not be obvious to someone reading the code. This can lead to issues with testing and maintenance.
    LoggerSingleton::instance().log("This is an implicit dependency on LoggerSingleton.");
}

void explicit_dependency_example(Logger& logger)
{
    // This function demonstrates the use of an explicit dependency. By passing a Logger reference as a parameter,
    // we make the dependency clear and allow for easier testing and maintenance.
    logger.log("This is an explicit dependency on Logger.");
}

int main()
{
    Singleton::instance().do_something();

    Singleton& singleObject = Singleton::instance();
    singleObject.do_something();

    ////////////////////////////////////////////////////////////////////////////

    LoggerSingleton::instance().log("Hello, Singleton Logger!");
    LoggerSingleton::instance().log("Another log message.");

    ///////////////////////////////////////////////////////////////////////////
    // Discussing implicit vs explicit dependencies when using singletons

    beware_of_implicit_dependencies();
    explicit_dependency_example(LoggerSingleton::instance());
    
}
