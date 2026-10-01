#include "adapter.hpp"
#include "dip.hpp"

#include <iostream>

using namespace std;

class Client
{
public:
    void do_operation(Target& t)
    {
        t.request();
    }
};

int main()
{
    Client client;

    cout << "-- do_operation on ClassAdapter" << endl;
    ClassAdapter cadapter;
    client.do_operation(cadapter);

    cout << endl;

    cout << "-- do_operation on ObjectAdapter" << endl;
    Adaptee adaptee;
    ObjectAdapter oadapter(adaptee);
    client.do_operation(oadapter);
}

void dip_and_adapters_layer_example()
{
    using namespace AdaptersLayer::ObjectAdapter;
    using namespace HighLevel::LooselyCoupled;

    // class adapter in action
    AdaptersLayer::ClassAdapter::LedLightAdapter class_adapter;
    ToggleButton class_button(class_adapter);
    
    class_button.toggle(); // Turn on the LED
    class_button.toggle(); // Turn off the LED

    // object adapter in action
    LowLevel::LedLight led_light;
    LedLightAdapter adapter(led_light);
    ToggleButton button(adapter);

    button.toggle(); // Turn on the LED
    button.toggle(); // Turn off the LED
}