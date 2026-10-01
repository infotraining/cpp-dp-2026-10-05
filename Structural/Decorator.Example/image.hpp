#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

struct BoundingBox
{
    size_t x, y, width, height;
};

using Bitmap = std::vector<std::string>;

using Canvas = std::vector<std::string>;

class Image
{
public:
    virtual void draw(Canvas& canvas) = 0;
    virtual BoundingBox bounding_box() = 0;
    virtual ~Image() = default;
};

class BitmapImage : public Image
{
    Bitmap bitmap_;

public:
    BitmapImage(Bitmap bmp)
        : bitmap_(std::move(bmp))
    { }

    void draw(Canvas& canvas) override
    {
        canvas = bitmap_;
    }

    BoundingBox bounding_box() override
    {
        auto width = std::ranges::fold_left(bitmap_, 0u, [](size_t acc, const auto& row) { return std::max(acc, row.size()); });

        // Return the bounding box of the bitmap image
        return BoundingBox{0, 0, width, static_cast<uint32_t>(bitmap_.size())};
    }
};

#endif /*IMAGE_HPP_*/
