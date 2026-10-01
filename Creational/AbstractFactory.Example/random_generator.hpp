#ifndef RANDOM_GENERATOR_HPP_
#define RANDOM_GENERATOR_HPP_

#include <random>

namespace Game
{
    // Abstraction over a source of randomness so game logic never depends on a concrete algorithm/engine.
    class RandomGenerator
    {
    public:
        virtual ~RandomGenerator() = default;

        // Returns true with the given probability (0.0 - 1.0).
        virtual bool bernoulli(double chance) = 0;

        // Returns an integer in [min, max] (inclusive).
        virtual int uniform_int(int min, int max) = 0;
    };

    template <typename TEngine>
    class RandGen : public RandomGenerator
    {
    public:
        explicit RandGen(TEngine engine)
            : engine_{std::move(engine)}
        { }

        bool bernoulli(double chance) override
        {
            return std::bernoulli_distribution{chance}(engine_);
        }

        int uniform_int(int min, int max) override
        {
            return std::uniform_int_distribution<>{min, max}(engine_);
        }

    private:
        TEngine engine_;
    };
} // namespace Game

#endif /*RANDOM_GENERATOR_HPP_*/
