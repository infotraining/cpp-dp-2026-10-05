#include "logger.hpp"

#include <catch2/catch_test_macros.hpp>
#include <source_location>

using namespace Logging;

namespace Experimental
{
    using Message = std::string;

    struct LogRecord
    {
        LogLevel level;
        std::string message;
    };

    static constexpr auto NullFactory = []() { return std::nullopt; };

    template <typename T, std::invocable auto EntryFactory = NullFactory>
    struct LogEntry
    {
        std::optional<T> entry;

        LogEntry(T entry = EntryFactory())
            : entry(entry)
        { }

        static auto factory()
        {
            return EntryFactory();
        }

        explicit operator bool() const noexcept
        {
            return static_cast<bool>(entry);
        }

        auto operator*() const noexcept
        {
            return *entry;
        }

        auto operator->() const noexcept
        {
            return &(*entry);
        }

        auto value_or(T default_value) const noexcept
        {
            return entry.value_or(default_value);
        }

        auto has_value() const noexcept
        {
            return static_cast<bool>(entry);
        }

        auto& value() const
        {
            return entry.value();
        }
    };

    namespace Details
    {
        template <typename... TArgs>
        auto init_from(std::tuple<TArgs...> args_tuple, auto&&... factories)
        {
            std::tuple factories_tuple(std::forward<decltype(factories)>(factories)...);

            constexpr std::size_t args_size = sizeof...(TArgs);
            constexpr std::size_t factories_size = sizeof...(factories);

            auto pick_value_or_from_factory = [&args_tuple, &factories_tuple]<std::size_t Index>() {
                if constexpr (Index < args_size)
                {
                    return std::get<Index>(args_tuple);
                }
                else
                {
                    return std::get<Index>(factories_tuple)();
                }
            };

            auto make_tuple_from_value_or_factory = [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                return std::tuple(pick_value_or_from_factory.template operator()<Indices>()...);
            };

            return make_tuple_from_value_or_factory(std::make_index_sequence<factories_size>{});
        }
    } // namespace Details

    template <typename... TLogEntries>
    struct LogRecordWith : LogRecord
    {
    private:
        std::tuple<TLogEntries...> entries;

        template <typename... TArgs>
        LogRecordWith(LogLevel level, const std::string& message, TArgs&&... args)
            : LogRecord{.level = level, .message = message}
            , entries(std::forward<TArgs>(args)...)
        { }

    public:
        template <typename... TArgs>
        static auto create(LogLevel level, const std::string& message, TArgs... args)
        {
            return LogRecordWith{level, message, std::forward<TArgs>(args)...};
        }

        template <typename T>
        auto& get()
        {
            if constexpr (std::is_same_v<T, LogLevel>)
            {
                return LogRecord::level;
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return LogRecord::message;
            }
            else
            {
                return std::get<T>(entries).entry;
            }
        }
    };

    using Timestamp = LogEntry<std::chrono::system_clock::time_point, []() { return std::chrono::system_clock::now(); }>;
    using ThreadId = LogEntry<std::thread::id, []() { return std::this_thread::get_id(); }>;
    using SourceLocation = LogEntry<std::source_location, []() { return std::source_location::current(); }>;

    using LogRecordWithTimestamp = LogRecordWith<Timestamp>;
    using LogRecordWithThreadId = LogRecordWith<ThreadId>;
    using LogRecordWithTimestampAndThreadId = LogRecordWith<Timestamp, ThreadId>;
    using LogRecordWithSourceLocation = LogRecordWith<SourceLocation>;
} // namespace Experimental

TEST_CASE("init_from", "[for-fun]")
{
    using namespace Experimental::Details;
    using namespace std::literals;

    auto r0 = init_from(std::tuple(), [] { return 665; }, []() { return 3.14; });
    REQUIRE(std::get<0>(r0) == 665);
    REQUIRE(std::get<1>(r0) == 3.14);

    auto r1 = init_from(std::tuple(42), [] { return 665; }, []() { return 3.14; });
    REQUIRE(std::get<0>(r1) == 42);
    REQUIRE(std::get<1>(r1) == 3.14);

    auto r2 = init_from(std::tuple(42, "test"s), [] { return 665; }, []() { return "hello"s; });
    REQUIRE(std::get<0>(r2) == 42);
    REQUIRE(std::get<1>(r2) == "test"s);
}

TEST_CASE("Experimental Logger Facade Templates")
{
    using namespace Experimental;

    LogRecordWithTimestamp recordWithTimestamp = LogRecordWithTimestamp::create(LogLevel::Info, "This is a test message with timestamp");
    REQUIRE(recordWithTimestamp.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithTimestamp.get<Message>() == "This is a test message with timestamp");
    REQUIRE(recordWithTimestamp.get<Timestamp>().value().time_since_epoch().count() > 0);

    LogRecordWithThreadId recordWithThreadId1 = LogRecordWithThreadId::create(LogLevel::Info, "This is a test message with thread id");
    REQUIRE(recordWithThreadId1.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithThreadId1.get<Message>() == "This is a test message with thread id");
    REQUIRE(recordWithThreadId1.get<ThreadId>() == std::this_thread::get_id());

    LogRecordWithThreadId recordWithThreadId2 = LogRecordWithThreadId::create(LogLevel::Info, "Another test message with thread id", std::thread::id{});
    REQUIRE(recordWithThreadId2.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithThreadId2.get<Message>() == "Another test message with thread id");
    REQUIRE(recordWithThreadId2.get<ThreadId>() == std::thread::id{});

    LogRecordWithTimestampAndThreadId recordWithTimestampAndThreadId1 = LogRecordWithTimestampAndThreadId::create(LogLevel::Info, "This is a test message with timestamp and thread id");
    REQUIRE(recordWithTimestampAndThreadId1.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithTimestampAndThreadId1.get<Message>() == "This is a test message with timestamp and thread id");
    REQUIRE(recordWithTimestampAndThreadId1.get<Timestamp>().value().time_since_epoch().count() > 0);
    REQUIRE(recordWithTimestampAndThreadId1.get<ThreadId>() == std::this_thread::get_id());

    LogRecordWithTimestampAndThreadId recordWithTimestampAndThreadId2 = LogRecordWithTimestampAndThreadId::create(LogLevel::Info, "Another test message with timestamp and thread id", std::chrono::system_clock::time_point{}, std::thread::id{});
    REQUIRE(recordWithTimestampAndThreadId2.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithTimestampAndThreadId2.get<Message>() == "Another test message with timestamp and thread id");
    REQUIRE(recordWithTimestampAndThreadId2.get<Timestamp>().value().time_since_epoch().count() == 0);
    REQUIRE(recordWithTimestampAndThreadId2.get<ThreadId>() == std::thread::id{});

    LogRecordWithSourceLocation recordWithSourceLocation = LogRecordWithSourceLocation::create(LogLevel::Info, "This is a test message with source location", std::source_location::current());  auto src_loc = std::source_location::current();    
    REQUIRE(recordWithSourceLocation.get<LogLevel>() == LogLevel::Info);
    REQUIRE(recordWithSourceLocation.get<Message>() == "This is a test message with source location");
    REQUIRE(recordWithSourceLocation.get<SourceLocation>().value().file_name() == std::string(src_loc.file_name()));
    REQUIRE(recordWithSourceLocation.get<SourceLocation>().value().line() == src_loc.line());
}