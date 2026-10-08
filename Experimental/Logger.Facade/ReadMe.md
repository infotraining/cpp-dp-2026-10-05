# Logger.Facade

A small C++23 logging library with configurable log levels, handlers, filters,
formatters, and log-record creation. The implementation is in [`logger.hpp`](./logger.hpp);
the tests in [`logger_tests.cpp`](./logger_tests.cpp) provide additional usage examples.

## Quick start

```cpp
#include "logger.hpp"

#include <iostream>
#include <memory>

int main()
{
    using namespace Logging;

    auto logger = LoggerBuilder("application")
        .set_level(LogLevel::Debug)
        .handler<StreamHandler>(std::clog)
            .with_level(LogLevel::Info)
            .add()
        .build();

    logger.log(LogLevel::Info, "Application started");
    logger.log(LogLevel::Debug, "This is filtered by the handler");
}
```

The logger-level threshold and handler-level threshold are applied independently:
the logger rejects records below its threshold before creating a record, then each
handler decides whether it accepts that record. In the example, the logger allows
Debug records, but the handler only emits Info and higher.

`LoggerBuilder::build()` transfers its configured handlers into the logger and can
only be called once. Any `std::ostream` passed to a `StreamHandler` must remain alive
while the logger uses that handler.

## Main classes and types

### `LogLevel` and `to_string`

`LogLevel` defines `NotSet`, `Debug`, `Info`, `Warning`, `Error`, and `Critical`.
Levels are ordered, so threshold checks can use comparisons such as
`record.level >= LogLevel::Warning`. `to_string(level)` returns the printable name.
`NotSet` has the lowest underlying value and therefore acts as a permissive threshold
when used by a logger.

### `LogRecord`

Each accepted log call is represented by a `LogRecord` containing its level, message,
a `std::chrono::system_clock::time_point` timestamp, and an optional
`std::stacktrace`. It is an aggregate and can be constructed directly; the
stacktrace is empty unless one is supplied:

```cpp
Logging::LogRecord record{
    .level = Logging::LogLevel::Warning,
    .message = "Disk space is low",
    .timestamp = std::chrono::system_clock::now(),
    .stacktrace = std::nullopt};
```

### `LogRecordFactory` and `ILogRecordFactory`

`LogRecordFactory` creates records using an injected timestamp provider. The
`ILogRecordFactory` alias also allows a caller to replace record creation entirely.
This is useful for deterministic tests or for adding message metadata:

```cpp
using namespace Logging;

const auto fixed_time =
    std::chrono::system_clock::time_point{std::chrono::seconds{42}};
LogRecordFactory factory([fixed_time] { return fixed_time; });
Logger logger("test", LogLevel::NotSet, factory);
```

A custom factory receives the level, message, and optional stacktrace, and must
return a complete `LogRecord`:

```cpp
ILogRecordFactory factory = [](LogLevel level,
                               const std::string& message,
                               std::optional<std::stacktrace> stacktrace) {
    return LogRecord{
        .level = level,
        .message = "application: " + message,
        .timestamp = std::chrono::system_clock::now(),
        .stacktrace = std::move(stacktrace)};
};

auto logger = LoggerBuilder("application")
    .with_log_record_factory(factory)
    .build();
```

The logger rejects an empty factory. The `LogRecordFactory` constructor likewise
rejects an empty timestamp provider.

### `Logger`

`Logger` owns its handlers, applies the logger-level threshold, creates one record
for each accepted message, and dispatches that record to every handler:

```cpp
using namespace Logging;

Logger logger("application", LogLevel::Warning);
logger.add_handler(std::make_unique<StreamHandler>(std::clog));

logger.log(LogLevel::Info, "Not emitted: below the logger threshold");
logger.log(LogLevel::Error, "Emitted");
```

`log()` takes an optional third argument, a `std::optional<std::stacktrace>`. When
it is supplied, `DefaultFormatter` writes the stacktrace on the lines following the
message; when omitted, the output is a single line:

```cpp
logger.log(LogLevel::Error, "Request failed", std::stacktrace::current());
```

The logger's `name` is retained as configuration but is not currently added to the
formatted output. Use `set_level()` to change its threshold after construction.
Adding a null handler is an error.

### `Handler`, `ConsoleHandler`, and `StreamHandler`

`Handler` is the polymorphic handler base. It applies its own severity threshold and
filter, formats accepted records, and then writes to its sink. Its default formatter
is `DefaultFormatter`; its default filter allows all records. `ConsoleHandler` writes
to `std::cout`, while `StreamHandler` writes to a caller-supplied `std::ostream`.
`StreamHandler` flushes its stream when destroyed; call `flush()` to flush explicitly.

Handlers can also be configured without a builder:

```cpp
using namespace Logging;

StreamHandler handler(std::clog);
handler.set_level(LogLevel::Warning);
handler.set_filter([](const LogRecord& record) {
    return record.message.find("network") != std::string::npos;
});
```

### `Formatter` and `DefaultFormatter`

`Formatter` defines the formatting strategy. `DefaultFormatter` produces a local-time
timestamp, level name, and message, for example:

```text
[2026-10-07 20:55:00] Warning: Disk space is low
```

If the record carries a stacktrace, `DefaultFormatter::format()` appends a newline
followed by the output of `format_stacktrace()`:

```text
[2026-10-07 20:55:00] Error: Request failed
   0> main.cpp(42): app!handle_request+0x1F
   1> main.cpp(60): app!main+0x2A
```

Custom formatters that override `format()` decide for themselves whether to include
`format_stacktrace(record)`.

Custom formatters implement all three virtual functions:

```cpp
class CompactFormatter : public Logging::Formatter
{
public:
    std::string format(const Logging::LogRecord& record) const override
    {
        return Logging::to_string(record.level) + ": " + record.message;
    }

    std::string format_timestamp(const Logging::LogRecord&) const override
    {
        return {};
    }

    std::string format_stacktrace(const Logging::LogRecord&) const override
    {
        return {};
    }
};
```

Attach a formatter to a handler with `set_formatter()` or configure it through
`LoggerBuilder::HandlerBuilder::with_formatter()`.

### `Filter`

`Filter` is a callable that accepts a `const LogRecord&` and returns `true` to let
the record through. For example, this filter accepts warnings and more severe records:

```cpp
Logging::Filter warnings_only = [](const Logging::LogRecord& record) {
    return record.level >= Logging::LogLevel::Warning;
};
```

Filters are per-handler: a rejected record is skipped by that handler without
preventing other handlers from receiving it. Empty filters are rejected by
`Handler::set_filter()`.

### `LoggerBuilder` and `HandlerBuilder`

`LoggerBuilder` configures a logger and transfers ownership of configured handlers
when `build()` is called. Each `handler<THandler>(...)` call creates a handler and
returns a `HandlerBuilder`; configure its level, filter, and formatter, then call
`add()` to attach it to the parent logger builder.

The handler must have an explicitly set level before `add()`. The default formatter
is already installed, so explicitly supplying one is optional.

### `ConsoleLogger`

`ConsoleLogger::instance()` is a process-wide facade with convenience methods
`debug`, `info`, `warn`, `error`, and `critical`:

```cpp
Logging::ConsoleLogger::instance().info("Application started");
Logging::ConsoleLogger::instance().warn("Disk space is low");
```

The singleton uses a console handler and accepts Debug and higher. For named logger
instances, `ConsoleLogger::get(name)` returns a `Logging::Logger&`; call its `log()`
method with an explicit level:

```cpp
Logging::ConsoleLogger::get("database")
    .log(Logging::LogLevel::Error, "Connection failed");
```

Repeated lookups with the same name return the same logger. The named logger is
configured for console output at Debug and higher.

## Design patterns

### Builder: `LoggerBuilder` and `HandlerBuilder`

The builders let callers assemble a logger incrementally without exposing its
ownership details. Handler configuration reads as a fluent chain:

```cpp
auto logger = Logging::LoggerBuilder("service")
    .set_level(Logging::LogLevel::Debug)
    .handler<Logging::StreamHandler>(std::clog)
        .with_level(Logging::LogLevel::Info)
        .with_filter([](const Logging::LogRecord& record) {
            return !record.message.starts_with("secret:");
        })
        .with_formatter(std::make_shared<Logging::DefaultFormatter>())
        .add()
    .build();
```

`add()` returns the parent logger builder, so another handler can be added to the
same configuration.

### Strategy: formatters and filters

`Formatter` and `Filter` encapsulate interchangeable behavior. A handler delegates
formatting to its formatter and acceptance decisions to its filter. Different
handlers can therefore use different strategies for the same record:

```cpp
auto logger = Logging::LoggerBuilder("service")
    .handler<Logging::StreamHandler>(info_stream)
        .with_level(Logging::LogLevel::Info)
        .with_filter([](const Logging::LogRecord& record) {
            return record.level == Logging::LogLevel::Info;
        })
        .with_formatter(std::make_shared<Logging::DefaultFormatter>())
        .add()
    .handler<Logging::StreamHandler>(error_stream)
        .with_level(Logging::LogLevel::Error)
        .with_filter([](const Logging::LogRecord& record) {
            return record.level >= Logging::LogLevel::Error;
        })
        .with_formatter(std::make_shared<CompactFormatter>())
        .add()
    .build();
```

### Chain of Responsibility (handler filtering and fan-out)

The logger dispatches each accepted record to every configured handler. Each handler
independently checks its level and filter before processing it. This resembles a
Chain of Responsibility in that each handler decides whether to handle a record, but
it is a **broadcast/fan-out**, not a short-circuiting chain: one handler accepting
or rejecting a record does not stop the others from checking it.

For example, a warning reaches a Warning handler and an Error handler configured
below; an error reaches both:

```cpp
auto logger = Logging::LoggerBuilder("service")
    .handler<Logging::StreamHandler>(warning_stream)
        .with_level(Logging::LogLevel::Warning)
        .add()
    .handler<Logging::StreamHandler>(error_stream)
        .with_level(Logging::LogLevel::Error)
        .add()
    .build();
```

### Template Method: `Handler::emit`

The Template Method pattern defines an algorithm's overall sequence in a base class
while allowing subclasses to customize selected steps. Here, `Handler::emit()` is
the non-virtual template method: it owns the common path for every log record, and
the protected virtual sink hooks supply the destination-specific behavior.

For each call, `emit()`:

1. Compares the record's level with the handler's configured minimum level. If the
   record is less severe, it returns without formatting or writing.
2. Invokes the handler's filter. If the filter returns `false`, it returns without
   formatting or writing.
3. Passes the accepted record to the configured formatter to produce a string.
4. Calls the virtual `write_to_sink()` hook with that formatted string.

In code, the shared part is:

```cpp
void emit(const LogRecord& record)
{
    if (record.level < level || !filter(record))
    {
        return;
    }

    std::string formatted_message = formatter->format(record);
    write_to_sink(formatted_message);
}
```

The base class owns and enforces the order of level filtering, predicate filtering,
formatting, and output. Subclasses do not reimplement that algorithm. Instead,
`ConsoleHandler` implements `write_to_sink()` using `std::cout`, while
`StreamHandler` writes the same formatted message to its referenced `std::ostream`.
Both also implement `flush_sink()`, which the public `flush()` method delegates to;
`StreamHandler` additionally calls it from its destructor.

This separation prevents sink implementations from accidentally bypassing the
configured level, filter, or formatter, and lets a new handler add another output
destination by implementing the sink hooks. The extensible steps are the sink
operations; the record-acceptance and formatting sequence remains consistent across
all handler types.

### Factory Method / dependency injection: record creation

The logger depends on the `ILogRecordFactory` callable rather than requiring a
particular record-creation implementation. The default `LogRecordFactory` supplies
the current system time; callers can inject a deterministic or customized factory
through the `Logger` constructor or `LoggerBuilder::with_log_record_factory()`.

### Facade: `ConsoleLogger`

`ConsoleLogger` provides a small set of level-named methods that hide the setup of
the underlying logger, handler, and formatter. Callers choose an operation that
expresses the severity instead of constructing a `LogRecord` or calling
`Logger::log()` themselves:

```cpp
void start_service()
{
    auto& log = Logging::ConsoleLogger::instance();
    log.info("Service started");
    log.warn("Configuration uses a default value");
    log.error("Could not connect to the database");
}
```

For a named logger, `ConsoleLogger::get(name)` returns a `Logger&`; this lower-level
API requires the caller to specify the level:

```cpp
void report_database_failure()
{
    Logging::ConsoleLogger::get("database")
        .log(Logging::LogLevel::Error, "Connection failed");
}
```

### Singleton: `ConsoleLogger::instance`

`instance()` lazily creates and returns the shared console facade. This is
appropriate for the simple default logger. Any component can use the same instance
without passing a logger through every call:

```cpp
Logging::ConsoleLogger& application_log()
{
    return Logging::ConsoleLogger::instance();
}

void load_configuration()
{
    application_log().debug("Loading configuration");
}

void run_application()
{
    application_log().info("Application is ready");
}
```

The local static instance is initialized on first use and subsequent calls return
the same object. `ConsoleLogger` cannot be copied, preserving the singleton instance.
This singleton uses the fixed default console configuration; use `LoggerBuilder`
when a logger needs custom handlers, filtering, or formatting.
