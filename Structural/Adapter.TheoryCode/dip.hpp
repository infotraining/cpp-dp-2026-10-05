#ifndef DIP_HPP_
#define DIP_HPP_

#include <iostream>

namespace LowLevel
{
    class LedLight
    {
    public:
        void set_rgb(int r, int g, int b)
        {
            std::cout << "Setting LED color to RGB(" << r << ", " << g << ", " << b << ")" << std::endl;
        }
    };
} // namespace LowLevel

namespace HighLevel
{
    namespace HighlyCoupled
    {
        class ToggleButton
        {
            LowLevel::LedLight led_light_;
            bool is_on_ = false;

        public:
            void toggle()
            {
                std::cout << "ToggleButton pressed" << std::endl;
                is_on_ = !is_on_;
                if (is_on_)
                {
                    led_light_.set_rgb(255, 255, 255); // Example: turn on the LED with white color
                }
                else
                {
                    led_light_.set_rgb(0, 0, 0); // Example: turn off the LED
                }
            }
        }; // namespace HighlyCoupledclass ToggleButton
    } // namespace HighlyCoupled

    namespace LooselyCoupled
    {
        class ISwitch
        {
        public:
            virtual void on() = 0;
            virtual void off() = 0;
            virtual ~ISwitch() = default;
        };

        class ToggleButton
        {
        public:
            ToggleButton(ISwitch& sw)
                : switch_(sw)
            { }

            void toggle()
            {
                if (is_on_)
                {
                    switch_.off();
                }
                else
                {
                    switch_.on();
                }
                is_on_ = !is_on_;
            }

        private:
            ISwitch& switch_;
            bool is_on_ = false;
        };
    } // namespace LooselyCoupled
} // namespace HighLevel

namespace AdaptersLayer
{
    namespace ObjectAdapter
    {
        class LedLightAdapter : public HighLevel::LooselyCoupled::ISwitch
        {
        private:
            LowLevel::LedLight& led_light_;

        public:
            LedLightAdapter(LowLevel::LedLight& led_light)
                : led_light_(led_light)
            { }

            void on() override
            {
                led_light_.set_rgb(255, 255, 255); // Example: turn on the LED with white color
            }

            void off() override
            {
                led_light_.set_rgb(0, 0, 0); // Example: turn off the LED
            }
        };

    } // namespace ObjectAdapter

    namespace ClassAdapter
    {
        class LedLightAdapter : private LowLevel::LedLight, public HighLevel::LooselyCoupled::ISwitch
        {
        public:
            void on() override
            {
                set_rgb(255, 255, 255); // Example: turn on the LED with white color
            }

            void off() override
            {
                set_rgb(0, 0, 0); // Example: turn off the LED
            }
        };
    } // namespace ClassAdapter

} // namespace AdaptersLayer

#endif /*DIP_HPP_*/