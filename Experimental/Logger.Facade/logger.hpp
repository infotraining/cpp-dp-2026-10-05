#include <cassert>
#include <chrono>
#include <compare>
#include <cstdint>
#include <ctime>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stacktrace>

namespace Logging
{
    enum class LogLevel : uint8_t {
        NotSet = 0,
        Debug = 10,
        Info = 20,
        Warning = 30,
        Error = 40,
        Critical = 50
    };

    inline std::string to_string(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::NotSet:
            return "NotSet";
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Critical:
            return "Critical";
        default:
            return "<Unknown>";
        }
    }

    inline std::strong_ordering operator<=>(LogLevel lhs, LogLevel rhs)
    {
        return static_cast<std::underlying_type_t<LogLevel>>(lhs) <=> static_cast<std::underlying_type_t<LogLevel>>(rhs);
    }

    struct LogRecord
    {
        LogLevel level;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::optional<std::stacktrace> stacktrace;
    };

    // Factory Method: creates log records. The callable is injected so callers can customize
    // record creation, for example to inject a deterministic clock in tests.
    using ILogRecordFactory = std::function<LogRecord(LogLevel, const std::string&, std::optional<std::stacktrace>)>;

    using ITimestamptProvider = std::function<std::chrono::system_clock::time_point()>;

    class LogRecordFactory
    {
        ITimestamptProvider timestamp_provider;

    public:
        explicit LogRecordFactory(ITimestamptProvider provider)
            : timestamp_provider(std::move(provider))
        {
            if (!timestamp_provider)
            {
                throw std::invalid_argument("Timestamp provider must be callable");
            }
        }

        LogRecord operator()(LogLevel level, const std::string& message, std::optional<std::stacktrace> stacktrace = std::nullopt) const
        {
            return LogRecord{
                .level = level,
                .message = message,
                .timestamp = timestamp_provider(),
                .stacktrace = stacktrace};
        }
    };

    // Strategy: a filter decides whether an individual record reaches a handler.
    // Example: handler.set_filter([](const LogRecord& record) { return record.level >= LogLevel::Warning; });
    using Filter = std::function<bool(const LogRecord&)>;

    static const Filter allow_all_filter = [](const LogRecord& record) { return true; };

    // Strategy interface for rendering records. Provide another implementation to customize log output.
    // Example: handler.set_formatter(std::make_shared<DefaultFormatter>());
    class Formatter
    {
    public:
        virtual ~Formatter() = default;
        virtual std::string format(const LogRecord& record) const = 0;
        virtual std::string format_timestamp(const LogRecord& record) const = 0;
        virtual std::string format_stacktrace(const LogRecord& record) const = 0;
    };

    class DefaultFormatter : public Formatter
    {
        constexpr static std::string_view timepoint_fmt_string = "%Y-%m-%d %H:%M:%S";
        constexpr static std::string_view message_fmt_string = "[{}] {}: {}";

    public:
        std::string format(const LogRecord& record) const override
        {
            auto formatted = std::format(message_fmt_string, format_timestamp(record), to_string(record.level), format_message(record));

            if (record.stacktrace.has_value())
            {
                formatted += "\n" + format_stacktrace(record);
            }

            return formatted;
        }

        std::string format_message(const LogRecord& record) const
        {
            return record.message;
        }

        std::string format_timestamp(const LogRecord& record) const override
        {
            auto time = std::chrono::system_clock::to_time_t(record.timestamp);
            std::tm tm{};
            if (localtime_s(&tm, &time) != 0)
            {
                throw std::runtime_error("Failed to convert log timestamp");
            }
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }

        std::string format_stacktrace(const LogRecord& record) const override
        {
            return std::format("{}", record.stacktrace.value_or(std::stacktrace{}));
        }
    };

    // Chain of Responsibility: each handler decides whether to process a log record based on its level and filter.
    // Template Method: emit() owns filtering and formatting, while subclasses implement the sink hooks.
    // Example: StreamHandler handler{std::cerr}; handler.emit(record);
    class Handler
    {
    private:
        LogLevel level = LogLevel::NotSet;
        std::shared_ptr<Formatter> formatter = std::make_shared<DefaultFormatter>();
        Filter filter = allow_all_filter;

    public:
        virtual ~Handler() = default;

        void set_level(LogLevel level)
        {
            this->level = level;
        }

        LogLevel get_level() const
        {
            return level;
        }

        void set_formatter(std::shared_ptr<Formatter> formatter)
        {
            if (!formatter)
            {
                throw std::invalid_argument("Formatter must not be null");
            }
            this->formatter = std::move(formatter);
        }

        std::shared_ptr<Formatter> get_formatter() const
        {
            return formatter;
        }

        void set_filter(const Filter& filter)
        {
            if (!filter)
            {
                throw std::invalid_argument("Filter must be callable");
            }
            this->filter = filter;
        }

        Filter get_filter() const
        {
            return filter;
        }

        void emit(const LogRecord& record)
        {

            if (record.level < level || !filter(record))
            {
                return;
            }

            std::string formatted_message = formatter->format(record);

            write_to_sink(formatted_message);
        }

        void flush()
        {
            flush_sink();
        }

    protected:
        virtual void write_to_sink(const std::string& formatted_message) = 0;
        virtual void flush_sink() = 0;
    };

    class ConsoleHandler : public Handler
    {
    protected:
        void write_to_sink(const std::string& formatted_message) override
        {
            std::cout << formatted_message << "\n";
        }

        void flush_sink() override
        {
            std::cout << std::flush;
        }
    };

    class StreamHandler : public Handler
    {
        std::ostream& stream_;

    public:
        explicit StreamHandler(std::ostream& stream)
            : stream_(stream)
        { }

        ~StreamHandler() override
        {
            flush_sink();
        }

    protected:
        void write_to_sink(const std::string& formatted_message) override
        {
            stream_ << formatted_message << "\n";
        }

        void flush_sink() override
        {
            stream_ << std::flush;
        }
    };

    // Coordinates record creation and dispatches each accepted record to its handlers.
    // Example:
    //   Logger logger{"application", LogLevel::Info};
    //   logger.add_handler(std::make_unique<StreamHandler>(std::clog));
    //   logger.log(LogLevel::Info, "Application started");
    class Logger
    {
        const std::string name_;
        LogLevel level_;
        std::vector<std::unique_ptr<Handler>> handlers_;
        ILogRecordFactory log_record_factory_;

    public:
        explicit Logger(
            std::string name,
            LogLevel level = LogLevel::NotSet,
            ILogRecordFactory log_record_factory = LogRecordFactory([]() { return std::chrono::system_clock::now(); }))
            : name_(std::move(name))
            , level_(level)
            , log_record_factory_(std::move(log_record_factory))
        {
            if (!log_record_factory_)
            {
                throw std::invalid_argument("Log record factory must be callable");
            }
        }

        void set_level(LogLevel level)
        {
            level_ = level;
        }

        void add_handler(std::unique_ptr<Handler> handler)
        {
            if (!handler)
            {
                throw std::invalid_argument("Handler must not be null");
            }
            handlers_.push_back(std::move(handler));
        }

        void log(LogLevel level, const std::string& message, std::optional<std::stacktrace> stacktrace = std::nullopt)
        {
            if (level < level_)
            {
                return;
            }

            LogRecord record = log_record_factory_(level, message, stacktrace);

            for (const auto& handler : handlers_)
            {
                handler->emit(record);
            }
        }
    };

    // Builder: assembles a Logger and transfers configured handlers to the completed object.
    // Example:
    //   auto logger = LoggerBuilder{"application"}
    //       .set_level(LogLevel::Debug)
    //       .handler<StreamHandler>(std::clog).with_level(LogLevel::Info).add()
    //       .build();
    class LoggerBuilder
    {
        std::string name_;
        LogLevel level_ = LogLevel::NotSet;
        std::vector<std::unique_ptr<Handler>> handlers_;
        ILogRecordFactory log_record_factory_ = LogRecordFactory([]() { return std::chrono::system_clock::now(); });
        bool built_ = false;

    public:
        explicit LoggerBuilder(std::string name)
            : name_(std::move(name))
        { }

        LoggerBuilder& set_level(LogLevel level)
        {
            level_ = level;
            return *this;
        }

        struct HandlerBuilder
        {
            LoggerBuilder& parent_builder;
            std::unique_ptr<Handler> handler;
            static inline std::shared_ptr<Formatter> default_formatter = std::make_shared<DefaultFormatter>();

            HandlerBuilder(LoggerBuilder& parent_builder, std::unique_ptr<Handler> handler)
                : parent_builder(parent_builder)
                , handler(std::move(handler))
            { }

            HandlerBuilder& with_level(LogLevel level)
            {
                handler->set_level(level);
                return *this;
            }

            HandlerBuilder& with_formatter(std::shared_ptr<Formatter> formatter)
            {
                handler->set_formatter(std::move(formatter ? formatter : default_formatter));
                return *this;
            }

            HandlerBuilder& with_filter(const Filter& filter)
            {
                handler->set_filter(filter);
                return *this;
            }

            LoggerBuilder& add()
            {
                if (!validate_handler(handler))
                {
                    throw std::runtime_error("Invalid handler configuration");
                }
                parent_builder.handlers_.push_back(std::move(handler));

                return parent_builder;
            }
        private: 
            bool validate_handler(const std::unique_ptr<Handler>& handler)
            {
                return handler && handler->get_level() != LogLevel::NotSet && handler->get_formatter() != nullptr;
            }

        };

        template <typename THandler, typename... Args>
        HandlerBuilder handler(Args&&... args)
        {
            return HandlerBuilder(*this, std::make_unique<THandler>(std::forward<Args>(args)...));
        }

        LoggerBuilder& with_log_record_factory(ILogRecordFactory log_record_factory)
        {
            if (!log_record_factory)
            {
                throw std::invalid_argument("Log record factory must be callable");
            }
            log_record_factory_ = std::move(log_record_factory);
            return *this;
        }

        Logger build()
        {
            if (built_)
            {
                throw std::logic_error("LoggerBuilder can only build once");
            }

            Logger logger(name_, level_, log_record_factory_);
            for (auto& handler : handlers_)
            {
                logger.add_handler(std::move(handler));
            }
            handlers_.clear();
            built_ = true;
            return logger;
        }
    };
} // namespace Logging

namespace Logging
{
    // Facade: exposes named logging operations without requiring callers to configure Logger or Handler objects.
    // Singleton: instance() returns the process-wide, lazily initialized console logger.
    // Example: ConsoleLogger::instance().info("Application started");
    class ConsoleLogger
    {
        Logging::Logger logger_;

        explicit ConsoleLogger(Logging::Logger&& logger)
            : logger_(std::move(logger))
        { }

        ~ConsoleLogger() = default;

    public:
        ConsoleLogger(const ConsoleLogger&) = delete;
        ConsoleLogger& operator=(const ConsoleLogger&) = delete;

        static Logging::Logger& get(const std::string& name)
        {
            static std::mutex loggers_mutex;
            static std::unordered_map<std::string, std::unique_ptr<Logging::Logger>> loggers;

            std::lock_guard<std::mutex> lock(loggers_mutex);
            const auto existing = loggers.find(name);
            if (existing != loggers.end())
            {
                return *existing->second;
            }

            auto logger = std::make_unique<Logging::Logger>(
                Logging::LoggerBuilder(name)
                    .set_level(Logging::LogLevel::Debug)
                    .handler<Logging::ConsoleHandler>()
                        .with_level(Logging::LogLevel::Debug)
                    .add()
                    .build());
            auto& result = *logger;
            loggers.emplace(name, std::move(logger));
            return result;
        }

        void info(const std::string& message)
        {
            logger_.log(Logging::LogLevel::Info, message);
        }

        void debug(const std::string& message)
        {
            logger_.log(Logging::LogLevel::Debug, message);
        }

        void warn(const std::string& message)
        {
            logger_.log(Logging::LogLevel::Warning, message);
        }

        void error(const std::string& message)
        {
            logger_.log(Logging::LogLevel::Error, message);
        }

        void critical(const std::string& message)
        {
            logger_.log(Logging::LogLevel::Critical, message);
        }

        static ConsoleLogger& instance()
        {
            static ConsoleLogger instance{
                Logging::LoggerBuilder("main-logger")
                    .set_level(Logging::LogLevel::Debug)
                    .handler<Logging::ConsoleHandler>()
                        .with_level(Logging::LogLevel::Debug)
                        .with_formatter(std::make_shared<Logging::DefaultFormatter>())
                    .add()
                    .build()};

            return instance;
        }
    };
} // namespace Logging