#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

struct StatResult
{
    std::string description;
    double value;

    StatResult(const std::string& desc, double val)
        : description(desc)
        , value(val)
    {
    }
};

using Data = std::vector<double>;
using Results = std::vector<StatResult>;

enum StatisticsType {
    avg,
    min_max,
    sum
};

namespace ClassicalStrategyPattern
{
    class StatisticsStrategy
    {
    public:
        virtual ~StatisticsStrategy() = default;
        virtual void calculate(const Data& data, Results& results) = 0;
    };

    class AverageStrategy : public StatisticsStrategy
    {
    public:
        void calculate(const Data& data, Results& results) override
        {
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double avg = sum / data.size();
            results.push_back(StatResult("Avg", avg));
        }
    };

    class MinMaxStrategy : public StatisticsStrategy
    {
    public:
        void calculate(const Data& data, Results& results) override
        {
            double min = *(std::min_element(data.begin(), data.end()));
            double max = *(std::max_element(data.begin(), data.end()));
            results.push_back(StatResult("Min", min));
            results.push_back(StatResult("Max", max));
        }
    };

    class SumStrategy : public StatisticsStrategy
    {
    public:
        void calculate(const Data& data, Results& results) override
        {
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            results.push_back(StatResult("Sum", sum));
        }
    };

    class StatGroup : public StatisticsStrategy
    {
        std::vector<std::shared_ptr<StatisticsStrategy>> strategies_;

    public:
        void add_strategy(std::shared_ptr<StatisticsStrategy> strategy)
        {
            strategies_.push_back(strategy);
        }

        void calculate(const Data& data, Results& results) override
        {
            for (auto& strategy : strategies_)
            {
                strategy->calculate(data, results);
            }
        }
    };

    class DataAnalyzer
    {
        Data data_;
        Results results_;
        std::shared_ptr<StatisticsStrategy> statistics_;

    public:
        DataAnalyzer(std::shared_ptr<StatisticsStrategy> s)
            : statistics_{s}
        {
        }

        void load_data(const std::string& file_name)
        {
            data_.clear();
            results_.clear();

            std::ifstream fin(file_name.c_str());
            if (!fin)
                throw std::runtime_error("File not opened");

            double d;
            while (fin >> d)
            {
                data_.push_back(d);
            }

            std::cout << "File " << file_name << " has been loaded...\n";
        }

        void set_statistics(std::shared_ptr<StatisticsStrategy> new_statistics)
        {
            assert(new_statistics != nullptr && "New statistics strategy must not be null");

            statistics_ = new_statistics;
        }

        void calculate()
        {
            statistics_->calculate(data_, results_);
        }

        const Results& results() const
        {
            return results_;
        }
    };

} // namespace ClassicalStrategyPattern

namespace ModernCppStrategyPattern
{
    using StatisticsStrategy = std::function<void(const Data& data, Results& results)>;

    class AverageStrategy
    {
    public:
        void operator()(const Data& data, Results& results)
        {
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double avg = sum / data.size();
            results.push_back(StatResult("Avg", avg));
        }
    };

    class MinStrategy
    {
    public:
        void operator()(const Data& data, Results& results)
        {
            double min = *(std::min_element(data.begin(), data.end()));
            results.push_back(StatResult("Min", min));
        }
    };

    class MaxStrategy
    {
    public:
        void operator()(const Data& data, Results& results)
        {
            double max = *(std::max_element(data.begin(), data.end()));
            results.push_back(StatResult("Max", max));
        }
    };

    class SumStrategy
    {
    public:
        void operator()(const Data& data, Results& results)
        {
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            results.push_back(StatResult("Sum", sum));
        }
    };

    class StatGroup 
    {
        std::vector<StatisticsStrategy> strategies_;

    public:
        void add_strategy(StatisticsStrategy strategy)
        {
            strategies_.push_back(strategy);
        }

        void operator()(const Data& data, Results& results)
        {
            for (auto& stat : strategies_)
            {
                stat(data, results);
            }
        }
    };

    class DataAnalyzer
    {
        Data data_;
        Results results_;
        StatisticsStrategy statistics_;

    public:
        DataAnalyzer(StatisticsStrategy s)
            : statistics_{s}
        {
        }

        void load_data(const std::string& file_name)
        {
            data_.clear();
            results_.clear();

            std::ifstream fin(file_name.c_str());
            if (!fin)
                throw std::runtime_error("File not opened");

            double d;
            while (fin >> d)
            {
                data_.push_back(d);
            }

            std::cout << "File " << file_name << " has been loaded...\n";
        }

        void set_statistics(StatisticsStrategy new_statistics)
        {
            statistics_ = new_statistics;
        }

        void calculate()
        {
            statistics_(data_, results_);
        }

        const Results& results() const
        {
            return results_;
        }
    };

} // namespace ModernCppStrategyPattern

void show_results(const Results& results)
{
    for (const auto& rslt : results)
        std::cout << rslt.description << " = " << rslt.value << std::endl;
}

int main()
{
    // TODO: Refactor the code above using Strategy Pattern
    // Hint#1: Define an interface for calculating Statistics
    // Hint#2: Use delegation technique to replace ifs in calculate() method

    using namespace ModernCppStrategyPattern;

    AverageStrategy avg;
    StatGroup min_max;
    min_max.add_strategy(MinStrategy{});
    min_max.add_strategy(MaxStrategy{});
    SumStrategy sum;

    StatGroup std_stat_group;
    std_stat_group.add_strategy(avg);
    std_stat_group.add_strategy(min_max);
    std_stat_group.add_strategy(sum);

    DataAnalyzer da{std_stat_group};
    da.load_data("stats_data.dat");
    da.calculate();

    show_results(da.results());

    std::cout << "\n\n";

    da.load_data("new_stats_data.dat");
    da.calculate();

    show_results(da.results());
}