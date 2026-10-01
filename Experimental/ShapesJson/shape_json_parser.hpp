#ifndef SHAPE_JSON_PARSER_HPP
#define SHAPE_JSON_PARSER_HPP

#include "shape_reader_factory.hpp"
#include "shapes.hpp"

#include <nlohmann/json.hpp>

namespace Drawing::IO
{
    class ShapeSaxHandler : public nlohmann::json_sax<nlohmann::json>
    {
    public:
        ShapeSaxHandler(const ShapeReaderFactory& factory,
            std::vector<std::unique_ptr<Shape>>& out)
            : factory_(factory)
            , out_(out)
        { }

        bool start_object(std::size_t) override
        {
            depth_++;
            if (depth_ == 2)
            {
                capturing_ = true;
                buffer_.str("");
                buffer_.clear();
                frames_.clear();
            }
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << "{";
                frames_.push_back(Frame{false, true});
            }
            return true;
        }

        bool end_object() override
        {
            if (capturing_)
            {
                buffer_ << "}";
                frames_.pop_back();
            }

            if (capturing_ && depth_ == 2)
            {
                capturing_ = false;
                parse_one(buffer_.str());
            }

            depth_--;
            return true;
        }

        bool null() override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << "null";
            }
            return true;
        }

        bool binary(nlohmann::json::binary_t&) override
        {
            return true;
        }

        bool key(std::string& k) override
        {
            if (capturing_)
            {
                begin_object_member_if_needed();
                buffer_ << "\"" << k << "\":";
            }
            return true;
        }

        bool string(std::string& v) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << "\"" << v << "\"";
            }
            return true;
        }

        bool boolean(bool v) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << (v ? "true" : "false");
            }
            return true;
        }

        bool number_integer(number_integer_t v) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << v;
            }
            return true;
        }

        bool number_unsigned(number_unsigned_t v) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << v;
            }
            return true;
        }

        bool number_float(number_float_t v, const string_t&) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << v;
            }
            return true;
        }

        bool start_array(std::size_t) override
        {
            if (capturing_)
            {
                begin_array_value_if_needed();
                buffer_ << "[";
                frames_.push_back(Frame{true, true});
            }
            depth_++;
            return true;
        }

        bool end_array() override
        {
            depth_--;
            if (capturing_)
            {
                buffer_ << "]";
                frames_.pop_back();
            }
            return true;
        }

        bool parse_error(std::size_t, const std::string&, const nlohmann::json::exception&) override
        {
            return false;
        }

    private:
        struct Frame
        {
            bool isArray{};
            bool firstItem{};
        };

        void begin_array_value_if_needed()
        {
            if (frames_.empty() || !frames_.back().isArray)
                return;

            if (frames_.back().firstItem)
                frames_.back().firstItem = false;
            else
                buffer_ << ",";
        }

        void begin_object_member_if_needed()
        {
            if (frames_.empty() || frames_.back().isArray)
                return;

            if (frames_.back().firstItem)
                frames_.back().firstItem = false;
            else
                buffer_ << ",";
        }

        void parse_one(const std::string& jsonText)
        {
            nlohmann::json j = nlohmann::json::parse(jsonText);
            const auto& key = j.begin().key();

            if (auto reader = factory_.create(key))
            {
                std::stringstream ss(jsonText);
                out_.push_back(reader->read(ss));
            }
        }

        const ShapeReaderFactory& factory_;
        std::vector<std::unique_ptr<Shape>>& out_;

        bool capturing_ = false;
        int depth_ = 0;
        std::stringstream buffer_;
        std::vector<Frame> frames_;
    };

    class ShapesJsonParser
    {
        ShapeReaderFactory& factory_;

    public:
        explicit ShapesJsonParser(ShapeReaderFactory& factory)
            : factory_(factory)
        { }

        std::vector<std::unique_ptr<Shape>> parse(std::istream& json_stream)
        {
            std::vector<std::unique_ptr<Shape>> shapes;
            ShapeSaxHandler handler(factory_, shapes);
            nlohmann::json::sax_parse(json_stream, &handler);
            return shapes;
        }
    };

} // namespace Drawing::IO

#endif // SHAPE_JSON_PARSER_HPP