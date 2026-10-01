#ifndef ENEMY_FACTORY_HPP_
#define ENEMY_FACTORY_HPP_

#include "monsters.hpp"
#include "random_generator.hpp"
#include <memory>

namespace Game
{
    // Abstract factory interface for creating different types of enemies (Soldier, Monster, Behemoth).
    class AbstractEnemyFactory
    {
    public:
        virtual std::unique_ptr<Soldier> create_soldier() = 0;
        virtual std::unique_ptr<Monster> create_monster() = 0;
        virtual std::unique_ptr<Behemoth> create_behemoth() = 0;
        virtual ~AbstractEnemyFactory() = default;
    };


    // Concrete factory for creating easy level enemies.
    class EasyLevelEnemyFactory : public AbstractEnemyFactory
    {
    public:
        explicit EasyLevelEnemyFactory(RandomGenerator& rng) : rng_{rng} {}

        std::unique_ptr<Soldier> create_soldier() override
        {
            return std::make_unique<RegularSoldier>("RegularSoldier", 20, 6, 0.10, rng_);
        }

        std::unique_ptr<Monster> create_monster() override
        {
            return std::make_unique<RegularMonster>("RegularMonster", 35, 8, 2, rng_);
        }

        std::unique_ptr<Behemoth> create_behemoth() override
        {
            return std::make_unique<RegularBehemoth>("RegularBehemoth", 60, 10, 0.3, rng_);
        }

    private:
        RandomGenerator& rng_;
    };

    // Concrete factory for creating die-hard level enemies.
    class DieHardLevelEnemyFactory : public AbstractEnemyFactory
    {
    public:
        explicit DieHardLevelEnemyFactory(RandomGenerator& rng) : rng_{rng} {}

        std::unique_ptr<Soldier> create_soldier() override
        {
            return std::make_unique<DieHardSoldier>("Bad Soldier", 45, 12, 0.20, rng_);
        }

        std::unique_ptr<Monster> create_monster() override
        {
            return std::make_unique<DieHardMonster>("DieHardMonster", 70, 16, 5, rng_);
        }

        std::unique_ptr<Behemoth> create_behemoth() override
        {
            return std::make_unique<DieHardBehemoth>("DieHardBehemoth", 120, 22, 0.4, rng_);
        }

    private:
        RandomGenerator& rng_;
    };
}

#endif /*ENEMY_FACTORY_HPP_*/
