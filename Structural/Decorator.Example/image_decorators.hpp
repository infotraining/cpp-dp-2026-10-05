#ifndef IMAGE_DECORATORS_HPP_
#define IMAGE_DECORATORS_HPP_

#include "image.hpp"

class ImageDecorator : public Image
{
    std::unique_ptr<Image> image_;

public:
    ImageDecorator(std::unique_ptr<Image> image)
        : image_(std::move(image))
    { }

    void draw(Canvas& canvas) override
    {
        image_->draw(canvas);
    }

    BoundingBox bounding_box() override
    {
        return image_->bounding_box();
    }
};

class BorderedImage : public ImageDecorator
{
    uint32_t border_thickness_;

public:
    BorderedImage(std::unique_ptr<Image> image, uint32_t border_thickness = 0)
        : ImageDecorator(std::move(image))
        , border_thickness_(border_thickness)
    { }

    void draw(Canvas& canvas) override
    {
        Canvas original;

        ImageDecorator::draw(original);

        // Draw the border around the image
        auto bbox = ImageDecorator::bounding_box();

        Bitmap new_bmp;

        // Add top border
        new_bmp.push_back("+" + std::string(bbox.width + 2 * border_thickness_, '-') + "+");

        for (uint32_t i = 0; i < border_thickness_; ++i)
        {
            new_bmp.push_back(std::format("|{}|", std::string(bbox.width + 2 * border_thickness_, ' ')));
        }

        const std::string border_fill(border_thickness_, ' ');

        for (const auto& row : original)
        {
            new_bmp.push_back(std::format("|{}{}{}|", border_fill, row, border_fill));
        }

        for (uint32_t i = 0; i < border_thickness_; ++i)
        {
            new_bmp.push_back(std::format("|{}|", std::string(bbox.width + 2 * border_thickness_, ' ')));
        }

        // Add bottom border
        new_bmp.push_back("+" + std::string(bbox.width + 2 * border_thickness_, '-') + "+");

        canvas = new_bmp;
    }

    BoundingBox bounding_box() override
    {
        auto bbox = ImageDecorator::bounding_box();
        return BoundingBox{bbox.x, bbox.y, bbox.width + 2 * border_thickness_ + 2, bbox.height + 2 * border_thickness_ + 2};
    }
};

class TaggedImage : public ImageDecorator
{
    std::string tag_;

public:
    TaggedImage(std::unique_ptr<Image> image, std::string tag)
        : ImageDecorator(std::move(image))
        , tag_(std::move(tag))
    { }

    void draw(Canvas& canvas) override
    {
        Canvas new_canvas;
        const BoundingBox bbox = ImageDecorator::bounding_box();

        ImageDecorator::draw(new_canvas);

        if (!new_canvas.empty())
        {
            std::string tag_line = std::format("{:^{}}", tag_, bbox.width);
            new_canvas.push_back(tag_line);
        }

        canvas = new_canvas;
    }
};

#endif /*IMAGE_DECORATORS_HPP_*/