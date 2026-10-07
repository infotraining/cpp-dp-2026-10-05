#ifndef TEXT_HPP
#define TEXT_HPP

#include "paragraph.hpp"
#include "shape.hpp"
#include <string>

namespace Drawing
{
    // TODO: Adapt LegacyCode::Paragraph class to Shape interface
    // Hint#1: Use Class Adapter
    // Hint#2: Register Text creator in SingletonShapeFactory 
    class Text : public ShapeBase, private LegacyCode::Paragraph
    {
    public:
        constexpr static const char* id = "Text";

        Text() = default;

        Text(int x, int y, const std::string& text)
            : ShapeBase{x, y}, LegacyCode::Paragraph{text.c_str()}
        {
        }
    
        void draw() const override
        {
            LegacyCode::Paragraph::render_at(coord().x, coord().y);
        }

        void set_text(const std::string& text)
        {
            LegacyCode::Paragraph::set_paragraph(text.c_str());
        }

        std::string text() const
        {
            return LegacyCode::Paragraph::get_paragraph();
        }
    };
}

#endif
