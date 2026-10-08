#include "logger.hpp"

#include <catch2/catch_test_macros.hpp>
#include <source_location>

using namespace Logging;

TEST_CASE("LogLevel to_string", "[LogLevel]")
{
    REQUIRE(to_string(LogLevel::NotSet) == "NotSet");
    REQUIRE(to_string(LogLevel::Debug) == "Debug");
    REQUIRE(to_string(LogLevel::Info) == "Info");
    REQUIRE(to_string(LogLevel::Warning) == "Warning");
    REQUIRE(to_string(LogLevel::Error) == "Error");
    REQUIRE(to_string(LogLevel::Critical) == "Critical");
    REQUIRE(to_string(static_cast<LogLevel>(255)) == "<Unknown>");
}

TEST_CASE("LogLevel comparison", "[LogLevel]")
{
    REQUIRE(LogLevel::Debug < LogLevel::Info);
    REQUIRE(LogLevel::Info < LogLevel::Warning);
    REQUIRE(LogLevel::Warning < LogLevel::Error);
    REQUIRE(LogLevel::Error < LogLevel::Critical);
    REQUIRE(LogLevel::NotSet < LogLevel::Debug);
    REQUIRE(LogLevel::Critical > LogLevel::Debug);
}

TEST_CASE("LogRecord creation", "[LogRecord]")
{
    const auto timestamp = std::chrono::system_clock::now();
    LogRecord record{
        .level = LogLevel::Info,
        .message = "Test message",
        .timestamp = timestamp};
    REQUIRE(record.level == LogLevel::Info);
    REQUIRE(record.message == "Test message");
    REQUIRE(record.timestamp == timestamp);
}

TEST_CASE("LogRecordFactory creation", "[LogRecordFactory]")
{
    const auto expected_timestamp = std::chrono::system_clock::time_point{std::chrono::seconds{42}};
    auto timestamp_provider = [expected_timestamp]() { return expected_timestamp; };
    LogRecordFactory factory(timestamp_provider);

    LogRecord record = factory(LogLevel::Warning, "Factory test message");
    REQUIRE(record.level == LogLevel::Warning);
    REQUIRE(record.message == "Factory test message");
    REQUIRE(record.timestamp == expected_timestamp);
    REQUIRE_THROWS_AS(LogRecordFactory(ITimestamptProvider{}), std::invalid_argument);
}

TEST_CASE("DefaultFormatter formatting", "[DefaultFormatter]")
{
    DefaultFormatter formatter;
    LogRecord record{
        .level = LogLevel::Error,
        .message = "Formatting test message",
        .timestamp = std::chrono::system_clock::now()};

    std::cout << "Formatted log message: " << formatter.format(record) << std::endl;

    std::string formatted_message = formatter.format(record);
    REQUIRE(formatted_message.find("Formatting test message") != std::string::npos);
    REQUIRE(formatted_message.find("Error") != std::string::npos);
}

TEST_CASE("ConsoleHandler formatting", "[ConsoleHandler]")
{
    ConsoleHandler handler;

    LogRecord record{
        .level = LogLevel::Info,
        .message = "Console handler test message",
        .timestamp = std::chrono::system_clock::now()};

    handler.emit(record);
}

TEST_CASE("StreamHandler - formatting", "[StreamHandler]")
{
    std::ostringstream oss;
    StreamHandler handler(oss);

    LogRecord record{
        .level = LogLevel::Debug,
        .message = "Stream handler test message",
        .timestamp = std::chrono::system_clock::now()};

    handler.emit(record);
    std::string output = oss.str();
    REQUIRE(output.find("Stream handler test message") != std::string::npos);
    REQUIRE(output.find("Debug") != std::string::npos);
}

TEST_CASE("Handler - level filtering", "[Handler]")
{
    std::ostringstream oss;
    StreamHandler handler(oss);
    handler.set_level(LogLevel::Warning);

    LogRecord record1{
        .level = LogLevel::Info,
        .message = "This message should not be logged",
        .timestamp = std::chrono::system_clock::now()};

    LogRecord record2{
        .level = LogLevel::Error,
        .message = "This message should be logged",
        .timestamp = std::chrono::system_clock::now()};

    handler.emit(record1);
    handler.emit(record2);

    std::string output = oss.str();
    REQUIRE(output.find("This message should not be logged") == std::string::npos);
    REQUIRE(output.find("This message should be logged") != std::string::npos);
}

TEST_CASE("Handler - custom filter", "[Handler]")
{
    std::ostringstream oss;
    StreamHandler handler(oss);

    auto custom_filter = [](const LogRecord& record) {
        return record.message.starts_with("Filtered");
    };

    handler.set_filter(custom_filter);

    LogRecord record1{
        .level = LogLevel::Info,
        .message = "This message should not be logged",
        .timestamp = std::chrono::system_clock::now()};

    LogRecord record2{
        .level = LogLevel::Error,
        .message = "Filtered: This message should be logged",
        .timestamp = std::chrono::system_clock::now()};

    handler.emit(record1);
    handler.emit(record2);

    std::string output = oss.str();
    REQUIRE(output.find("This message should not be logged") == std::string::npos);
    REQUIRE(output.find("Filtered: This message should be logged") != std::string::npos);
}

TEST_CASE("Handler rejects empty strategies", "[Handler]")
{
    std::ostringstream oss;
    StreamHandler handler(oss);

    REQUIRE_THROWS_AS(handler.set_formatter(nullptr), std::invalid_argument);
    REQUIRE_THROWS_AS(handler.set_filter(Filter{}), std::invalid_argument);
}

TEST_CASE("Logger - no handlers", "[Logger]")
{
    Logger logger("TestLogger");

    // No handlers added, should not throw or crash
    logger.log(LogLevel::Info, "This message has no handlers");
}

TEST_CASE("Logger rejects invalid dependencies", "[Logger]")
{
    Logger logger("TestLogger");

    REQUIRE_THROWS_AS(logger.add_handler(nullptr), std::invalid_argument);
    REQUIRE_THROWS_AS(Logger("TestLogger", LogLevel::NotSet, ILogRecordFactory{}), std::invalid_argument);
}

TEST_CASE("Logger does not create records for filtered messages", "[Logger]")
{
    bool factory_called = false;
    ILogRecordFactory factory = [&factory_called](LogLevel level, const std::string& message, std::optional<std::stacktrace> stacktrace = std::nullopt) {
        factory_called = true;
        return LogRecord{
            .level = level,
            .message = message,
            .timestamp = std::chrono::system_clock::now(),
            .stacktrace = stacktrace};
    };
    Logger logger("TestLogger", LogLevel::Warning, factory);

    logger.log(LogLevel::Info, "Filtered message");
    REQUIRE_FALSE(factory_called);

    logger.log(LogLevel::Warning, "Accepted message");
    REQUIRE(factory_called);
}

TEST_CASE("Logger - default level", "[Logger]")
{
    std::ostringstream oss;
    Logger logger("TestLogger");
    logger.add_handler(std::make_unique<StreamHandler>(oss));
    logger.set_level(LogLevel::Info);

    logger.log(LogLevel::Info, "This message should be logged with default level");
    logger.log(LogLevel::Debug, "This debug message should not be logged with default level");
    logger.log(LogLevel::Error, "This error message should be logged with default level");

    std::string output = oss.str();
    REQUIRE(output.find("This message should be logged with default level") != std::string::npos);
    REQUIRE(output.find("This debug message should not be logged with default level") == std::string::npos);
    REQUIRE(output.find("This error message should be logged with default level") != std::string::npos);
    REQUIRE(output.find("Info") != std::string::npos);
    REQUIRE(output.find("Debug") == std::string::npos);
    REQUIRE(output.find("Error") != std::string::npos);
}

TEST_CASE("Logger - changing log level", "[Logger]")
{
    std::ostringstream oss;
    Logger logger("TestLogger");
    logger.add_handler(std::make_unique<StreamHandler>(oss));

    logger.set_level(LogLevel::Error);
    logger.log(LogLevel::Info, "This info message should not be logged");
    logger.log(LogLevel::Error, "This error message should be logged");

    std::string output = oss.str();
    REQUIRE(output.find("This info message should not be logged") == std::string::npos);
    REQUIRE(output.find("This error message should be logged") != std::string::npos);
}

TEST_CASE("Logger - basic logging", "[Logger]")
{
    std::ostringstream oss;
    auto log_record_factory = LogRecordFactory([]() { return std::chrono::system_clock::now(); });
    Logger logger("TestLogger", LogLevel::Debug, log_record_factory);
    logger.add_handler(std::make_unique<StreamHandler>(oss));

    logger.log(LogLevel::Info, "Logger test message");

    std::string output = oss.str();
    REQUIRE(output.find("Logger test message") != std::string::npos);
    REQUIRE(output.find("Info") != std::string::npos);
}

TEST_CASE("Logger - default log record factory", "[Logger]")
{
    std::ostringstream oss;
    Logger logger("TestLogger");
    logger.add_handler(std::make_unique<StreamHandler>(oss));

    logger.log(LogLevel::Info, "Logger default factory test message");

    std::string output = oss.str();
    REQUIRE(output.find("Logger default factory test message") != std::string::npos);
    REQUIRE(output.find("Info") != std::string::npos);
}

TEST_CASE("Logger - level filtering", "[Logger]")
{
    std::ostringstream oss;
    auto log_record_factory = LogRecordFactory([]() { return std::chrono::system_clock::now(); });
    Logger logger("TestLogger", LogLevel::Warning, log_record_factory);
    logger.add_handler(std::make_unique<StreamHandler>(oss));

    logger.log(LogLevel::Info, "This message should not be logged");
    logger.log(LogLevel::Error, "This message should be logged");

    std::string output = oss.str();
    REQUIRE(output.find("This message should not be logged") == std::string::npos);
    REQUIRE(output.find("This message should be logged") != std::string::npos);
}

TEST_CASE("Logger - multiple handlers", "[Logger]")
{
    std::ostringstream oss1;
    std::ostringstream oss2;
    auto log_record_factory = LogRecordFactory([]() { return std::chrono::system_clock::now(); });
    Logger logger("TestLogger", LogLevel::NotSet, log_record_factory);
    logger.add_handler(std::make_unique<StreamHandler>(oss1));
    logger.add_handler(std::make_unique<StreamHandler>(oss2));
    logger.add_handler(std::make_unique<ConsoleHandler>());

    logger.log(LogLevel::Info, "Multiple handlers test message#1");
    logger.log(LogLevel::Debug, "Multiple handlers test message#2");
    logger.log(LogLevel::Error, "Multiple handlers test message#3");

    std::string output1 = oss1.str();
    std::string output2 = oss2.str();
    REQUIRE(output1.find("Multiple handlers test message#1") != std::string::npos);
    REQUIRE(output1.find("Multiple handlers test message#2") != std::string::npos);
    REQUIRE(output1.find("Multiple handlers test message#3") != std::string::npos);
    REQUIRE(output2.find("Multiple handlers test message#1") != std::string::npos);
    REQUIRE(output2.find("Multiple handlers test message#2") != std::string::npos);
    REQUIRE(output2.find("Multiple handlers test message#3") != std::string::npos);
}

TEST_CASE("Logger - custom log record factory", "[Logger]")
{
    std::ostringstream oss;
    auto log_record_factory = [](LogLevel level, const std::string& message, std::optional<std::stacktrace> stacktrace = std::nullopt) {
        return LogRecord{
            .level = level,
            .message = "Custom: " + message,
            .timestamp = std::chrono::system_clock::now(),
            .stacktrace = stacktrace};
    };
    Logger logger("TestLogger", LogLevel::NotSet, log_record_factory);
    logger.add_handler(std::make_unique<StreamHandler>(oss));

    logger.log(LogLevel::Info, "Custom log record factory test message");

    std::string output = oss.str();
    REQUIRE(output.find("Custom: Custom log record factory test message") != std::string::npos);
    REQUIRE(output.find("Info") != std::string::npos);
}

TEST_CASE("Logger - many handlers with different levels", "[Logger]")
{
    std::ostringstream oss1;
    std::ostringstream oss2;
    auto log_record_factory = LogRecordFactory([]() { return std::chrono::system_clock::now(); });
    Logger logger("TestLogger", LogLevel::NotSet, log_record_factory);
    auto handler1 = std::make_unique<StreamHandler>(oss1);
    handler1->set_level(LogLevel::Warning);
    logger.add_handler(std::move(handler1));
    auto handler2 = std::make_unique<StreamHandler>(oss2);
    handler2->set_level(LogLevel::Error);
    logger.add_handler(std::move(handler2));

    logger.log(LogLevel::Info, "This message should not be logged");
    logger.log(LogLevel::Warning, "This message should be logged by handler1");
    logger.log(LogLevel::Error, "This message should be logged by both handlers");

    std::string output1 = oss1.str();
    std::string output2 = oss2.str();
    REQUIRE(output1.find("This message should not be logged") == std::string::npos);
    REQUIRE(output1.find("This message should be logged by handler1") != std::string::npos);
    REQUIRE(output1.find("This message should be logged by both handlers") != std::string::npos);
    REQUIRE(output2.find("This message should not be logged") == std::string::npos);
    REQUIRE(output2.find("This message should be logged by handler1") == std::string::npos);
    REQUIRE(output2.find("This message should be logged by both handlers") != std::string::npos);
}

TEST_CASE("LoggerBuilder - basic usage", "[LoggerBuilder]")
{
    std::ostringstream oss;
    LoggerBuilder builder("TestLogger");
    builder.set_level(LogLevel::Info);
    builder.handler<StreamHandler>(oss)
        .with_level(LogLevel::Info)
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add();
    Logger logger = builder.build();

    logger.log(LogLevel::Debug, "This message should not be logged");
    logger.log(LogLevel::Info, "This message should be logged");

    std::string output = oss.str();
    REQUIRE(output.find("This message should not be logged") == std::string::npos);
    REQUIRE(output.find("This message should be logged") != std::string::npos);
}

TEST_CASE("LoggerBuilder - adding incorrect handler throws", "[LoggerBuilder]")
{
    std::ostringstream oss;
    LoggerBuilder builder("TestLogger");
    REQUIRE_THROWS_AS(builder
        .handler<StreamHandler>(oss)
        .add(), std::runtime_error);
}

TEST_CASE("LoggerBuilder can only build once", "[LoggerBuilder]")
{
    std::ostringstream oss;
    LoggerBuilder builder("TestLogger");
    builder.handler<StreamHandler>(oss)
        .with_level(LogLevel::Info)
        .add();

    auto logger = builder.build();
    REQUIRE_NOTHROW(logger.log(LogLevel::Info, "Builder output"));
    REQUIRE_THROWS_AS(builder.build(), std::logic_error);
}

TEST_CASE("LoggerBuilder rejects an empty record factory", "[LoggerBuilder]")
{
    LoggerBuilder builder("TestLogger");
    REQUIRE_THROWS_AS(builder.with_log_record_factory(ILogRecordFactory{}), std::invalid_argument);
}

TEST_CASE("LoggerBuilder - multiple handlers", "[LoggerBuilder]")
{
    std::ostringstream oss1;
    std::ostringstream oss2;
    LoggerBuilder builder("TestLogger");
    builder.set_level(LogLevel::NotSet);
    builder.handler<StreamHandler>(oss1)
        .with_level(LogLevel::Info)
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add();
    builder.handler<StreamHandler>(oss2)
        .with_level(LogLevel::Info)
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add();
    Logger logger = builder.build();

    logger.log(LogLevel::Info, "Multiple handlers test message");

    std::string output1 = oss1.str();
    std::string output2 = oss2.str();
    REQUIRE(output1.find("Multiple handlers test message") != std::string::npos);
    REQUIRE(output2.find("Multiple handlers test message") != std::string::npos);
}

TEST_CASE("LoggerBuilder - multiple handlers with different levels", "[LoggerBuilder]")
{
    std::ostringstream oss1;
    std::ostringstream oss2;
    LoggerBuilder builder("TestLogger");

    builder
        .handler<StreamHandler>(oss1)
        .with_level(LogLevel::Warning)
        .with_filter([](const LogRecord& record) { return record.level >= LogLevel::Warning; })
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add()
        .handler<StreamHandler>(oss2)
        .with_level(LogLevel::Error)
        .with_filter([](const LogRecord& record) { return record.level >= LogLevel::Error; })
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add();

    Logger logger = builder.build();

    logger.log(LogLevel::Info, "This message should not be logged");
    logger.log(LogLevel::Warning, "This message should be logged by handler1");
    logger.log(LogLevel::Error, "This message should be logged by both handlers");

    std::string output1 = oss1.str();
    std::string output2 = oss2.str();
    REQUIRE(output1.find("This message should not be logged") == std::string::npos);
    REQUIRE(output1.find("This message should be logged by handler1") != std::string::npos);
    REQUIRE(output1.find("This message should be logged by both handlers") != std::string::npos);
    REQUIRE(output2.find("This message should not be logged") == std::string::npos);
    REQUIRE(output2.find("This message should be logged by handler1") == std::string::npos);
    REQUIRE(output2.find("This message should be logged by both handlers") != std::string::npos);
}

TEST_CASE("LoggerBuilder - custom log record factory", "[LoggerBuilder]")
{
    auto log_record_factory = [](LogLevel level, const std::string& message, std::optional<std::stacktrace> stacktrace = std::nullopt) {
        return LogRecord{
            .level = level,
            .message = "Custom: " + message,
            .timestamp = std::chrono::system_clock::now()};
    };

    std::ostringstream oss;
    LoggerBuilder builder("TestLogger");

    builder
        .with_log_record_factory(log_record_factory)
        .handler<StreamHandler>(oss)
        .with_level(LogLevel::Info)
        .with_formatter(std::make_shared<DefaultFormatter>())
        .add();
    Logger logger = builder.build();

    logger.log(LogLevel::Info, "Custom log record factory test message");

    std::string output = oss.str();
    REQUIRE(output.find("Custom: Custom log record factory test message") != std::string::npos);
    REQUIRE(output.find("Info") != std::string::npos);
}

TEST_CASE("ConsoleLogger - facade", "[ConsoleLogger]")
{
    Logging::ConsoleLogger::instance().info("This message should be logged to console with default configuration");
    Logging::ConsoleLogger::instance().debug("This message should be logged to console with default configuration");
    Logging::ConsoleLogger::instance().warn("This message should be logged to console with default configuration");
    Logging::ConsoleLogger::instance().error("This message should be logged to console with default configuration");
    Logging::ConsoleLogger::instance().critical("This message should be logged to console with default configuration");
}

TEST_CASE("ConsoleLogger - named logger lookup", "[ConsoleLogger]")
{
    auto& first = ConsoleLogger::get("named-logger-test");
    auto& same = ConsoleLogger::get("named-logger-test");
    auto& other = ConsoleLogger::get("other-named-logger-test");

    REQUIRE(&first == &same);
    REQUIRE(&first != &other);
}

class CustomFormatter : public DefaultFormatter
{
public:
    std::string format(const LogRecord& record) const override
    {
        return "[Custom] " + record.message;
    }
};

TEST_CASE("Logger - complex logger configuration", "[Logger]")
{
    std::ostringstream oss1;
    std::ostringstream oss2;
    std::ostringstream oss3;

    auto default_formatter = std::make_shared<DefaultFormatter>();
    auto custom_formatter = std::make_shared<CustomFormatter>();

    LoggerBuilder builder("ComplexLogger");
    builder
        .handler<StreamHandler>(oss1)
            .with_level(LogLevel::Info)
            .with_formatter(default_formatter)
        .add()
        .handler<StreamHandler>(oss2)
            .with_level(LogLevel::Warning)
            .with_formatter(custom_formatter)
        .add()
        .handler<StreamHandler>(oss3)
            .with_level(LogLevel::Error)
            .with_filter([](const LogRecord& record) {
                return !record.message.starts_with("Filtered");
        })
        .with_formatter(default_formatter)
        .add();
    Logger logger = builder.build();

    logger.log(LogLevel::Info, "This is an info message");
    logger.log(LogLevel::Warning, "This is a warning message");
    logger.log(LogLevel::Error, "This is an error message");
    logger.log(LogLevel::Error, "Filtered-out message");

    std::string output1 = oss1.str();
    std::string output2 = oss2.str();
    std::string output3 = oss3.str();

    REQUIRE(output1.find("This is an info message") != std::string::npos);
    REQUIRE(output2.find("[Custom]This is an info message") == std::string::npos);
    REQUIRE(output3.find("This is an info message") == std::string::npos);

    REQUIRE(output1.find("This is a warning message") != std::string::npos);
    REQUIRE(output2.find("[Custom] This is a warning message") != std::string::npos);
    REQUIRE(output3.find("This is a warning message") == std::string::npos);

    REQUIRE(output1.find("This is an error message") != std::string::npos);
    REQUIRE(output2.find("[Custom] This is an error message") != std::string::npos);
    REQUIRE(output3.find("This is an error message") != std::string::npos);

    REQUIRE(output1.find("Filtered-out message") != std::string::npos);
    REQUIRE(output2.find("[Custom] Filtered-out message") != std::string::npos);
    REQUIRE(output3.find("Filtered-out message") == std::string::npos);
}

TEST_CASE("Logger - logging with stacktrace", "[Logger]")
{
    std::ostringstream oss;
    LoggerBuilder builder("StacktraceLogger");
    builder
        .handler<StreamHandler>(oss)
            .with_level(LogLevel::Error)
        .add();
    Logger logger = builder.build();

    const auto trace = std::stacktrace::current();
    REQUIRE_FALSE(trace.empty());
    logger.log(LogLevel::Error, "Error with trace", trace);

    std::string output = oss.str();
    REQUIRE(output.find("Error with trace") != std::string::npos);
    REQUIRE(output.find(std::to_string(trace)) != std::string::npos);

    std::cout << "Logger output with stacktrace:" << std::endl;
    std::cout << output << std::endl;
}

TEST_CASE("Logger - no stacktrace is written when none is provided", "[Logger]")
{
    std::ostringstream oss;
    LoggerBuilder builder("PlainLogger");
    builder
        .handler<StreamHandler>(oss)
            .with_level(LogLevel::Error)
        .add();
    Logger logger = builder.build();

    logger.log(LogLevel::Error, "Plain error");

    std::string output = oss.str();
    REQUIRE(output.find("Plain error") != std::string::npos);
    REQUIRE(std::count(output.begin(), output.end(), '\n') == 1);
}