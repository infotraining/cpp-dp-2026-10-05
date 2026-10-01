#include "image_decorators.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <print>
#include <ranges>
#include <vector>

Bitmap create_diamonds_bitmap()
{
    return {
        {
            "                    ",
            "         **         ",
            "        ****        ",
            "        ****        ",
            "      ********      ",
            "     **********     ",
            "    ************    ",
            "   **************   ",
            "   **************   ",
            "    ************    ",
            "     **********     ",
            "      ********      ",
            "        ****        ",
            "         **         ",
            "                    ",
        }};
}

Bitmap create_clubs_bitmap()
{
    return {
        {
            "        ****        ",
            "      ********      ",
            "     **********     ",
            "     **********     ",
            "      ********      ",
            "        ****        ",
            "   ****      ****   ",
            "  ******    ******  ",
            " ********  ******** ",
            " ********  ******** ",
            "  ******    ******  ",
            "   ****      ****   ",
            "        ****        ",
            "        ****        ",
            "        ****        ",
            "       ******       "}};
}

Bitmap create_hearts_bitmap()
{
    return {
        {"                    ",
            "     **     **      ",
            "    ****   ****     ",
            "   ****** ******    ",
            "   **************   ",
            "   **************   ",
            "    ************    ",
            "     **********     ",
            "      ********      ",
            "       ******       ",
            "        ****        ",
            "         **         ",
            "                    "}};
}

void render_pixels(const Canvas& canvas)
{
    for (const auto& row : canvas)
    {
        std::cout << row << "\n";
    }
}

int main()
{
    auto bitmap = std::make_unique<BorderedImage>(
        std::make_unique<TaggedImage>(
            std::make_unique<BorderedImage>(
                std::make_unique<BitmapImage>(create_clubs_bitmap()),
                4),
            "Clubs"),
        2);

    auto tagged_bitmap = std::make_unique<TaggedImage>(
        std::move(bitmap),
        "Cards");

    Canvas canvas;
    tagged_bitmap->draw(canvas);

    render_pixels(canvas);
}
