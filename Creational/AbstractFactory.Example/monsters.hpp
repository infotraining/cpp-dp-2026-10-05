#ifndef MONSTERS_HPP_
#define MONSTERS_HPP_

#include "random_generator.hpp"

#include <algorithm>
#include <iostream>
#include <string>

namespace Game
{
    // Pure interface - no shared implementation, so it says nothing about how an enemy behaves.
    class Enemy
    {
    public:
        virtual ~Enemy() = default;

        virtual const std::string& name() const = 0;
        virtual int health() const = 0;
        virtual bool is_alive() const = 0;
        virtual void take_damage(int dmg) = 0;

        // Plays out this enemy's turn (attack, ability, etc.) and returns the damage it deals.
        virtual int take_turn() = 0;
    };


    // Common state/bookkeeping implemented once and reused via inheritance.
    class EnemyBase : public Enemy
    {
    public:
        const std::string& name() const override { return name_; }
        int health() const override { return health_; }
        bool is_alive() const override { return health_ > 0; }
        void take_damage(int dmg) override { health_ = std::max(0, health_ - dmg); }

    protected:
        EnemyBase(std::string name, int health, int attack_power, RandomGenerator& rng)
            : name_{std::move(name)}
            , health_{health}
            , max_health_{health}
            , attack_power_{attack_power}
            , rng_{rng}
        {
        }

        int max_health() const { return max_health_; }
        int attack_power() const { return attack_power_; }
        void heal(int amount) { health_ = std::min(max_health_, health_ + amount); }
        RandomGenerator& random_generator() const { return rng_; }

    private:
        std::string name_;
        int health_;
        int max_health_;
        int attack_power_;
        RandomGenerator& rng_;
    };


    class Soldier : public EnemyBase
    {
        using EnemyBase::EnemyBase; // inheriting constructors from EnemyBase
    };

    class Monster : public EnemyBase
    {
        using EnemyBase::EnemyBase; // inheriting constructors from EnemyBase
    };

    class Behemoth : public EnemyBase
    {
        using EnemyBase::EnemyBase; // inheriting constructors from EnemyBase
    };

    //////////////////////////////////////////////////
    // Concrete enemy behaviours - each is a genuinely different
    // algorithm. Easy/die-hard variants only need different numbers,
    // so they are not modeled as separate classes.
    //////////////////////////////////////////////////

    class RegularSoldier : public Soldier
    {
    public:
        RegularSoldier(std::string name, int health, int attack_power, double crit_chance, RandomGenerator& rng)
            : Soldier{std::move(name), health, attack_power, rng}
            , crit_chance_{crit_chance}
        {
        }

        // Executes this enemy's turn and returns the damage dealt.
        // Determines if the attack is critical and calculates the damage accordingly (double damage on critical).
        int take_turn() override
        {
            const bool is_critical = random_generator().bernoulli(crit_chance_);
            const int damage = is_critical ? attack_power() * 2 : attack_power();

            std::cout << name() << " (" << health() << "/" << max_health() << " HP) slashes"
                      << (is_critical ? " a CRITICAL HIT" : "") << " for " << damage << " damage\n";
            return damage;
        }

    private:
        double crit_chance_;
    };

    class RegularMonster : public Monster
    {
    public:
        RegularMonster(std::string name, int health, int attack_power, int regen, RandomGenerator& rng)
            : Monster{std::move(name), health, attack_power, rng}
            , regen_{regen}
        {
        }

        // Executes this enemy's turn and returns the damage dealt.
        // Regenerates health if below max health before attacking.
        int take_turn() override
        {
            if (health() < max_health())
            {
                const int heal_points = random_generator().uniform_int(1, regen_);
                heal(heal_points);
                std::cout << name() << " regenerates " << heal_points << " HP (" << health() << "/"
                          << max_health() << ")\n";
            }

            std::cout << name() << " smashes for " << attack_power() << " damage\n";
            return attack_power();
        }

    private:
        int regen_;
    };

    class RegularBehemoth : public Behemoth
    {
    public:
        RegularBehemoth(std::string name, int health, int attack_power, double rage_threshold, RandomGenerator& rng)
            : Behemoth{std::move(name), health, attack_power, rng}
            , rage_threshold_{rage_threshold}
        {
        }

        // Executes this enemy's turn and returns the damage dealt
        // Determines if the RegularBehemoth is enraged and calculates the damage accordingly (double damage when enraged).
        int take_turn() override
        {
            const bool enraged = health() <= max_health() * rage_threshold_;
            const int damage = enraged ? attack_power() * 2 : attack_power();

            std::cout << name() << " (" << health() << "/" << max_health() << " HP)"
                      << (enraged ? " goes into a RAGE and" : "") << " strikes for " << damage << " damage\n";
            return damage;
        }

    private:
        double rage_threshold_;
    };

    /////////////////////////////////////////////////////////////
    // DieHard family of enemies
    class DieHardSoldier : public RegularSoldier
    {
        static constexpr double recovery_chance_ = 0.3;

    public:
        DieHardSoldier(std::string name, int health, int attack_power, double crit_chance, RandomGenerator& rng)
            : RegularSoldier{std::move(name), health, attack_power, crit_chance, rng}
            , crit_chance_{crit_chance}
        {
        }

        // Executes this enemy's turn and returns the damage dealt.
        // Recovers health with a certain probability after attacking.
        int take_turn() override
        {
            const int damage = RegularSoldier::take_turn();

            int recovered = 0;

            if (random_generator().bernoulli(recovery_chance_))
            {
                recovered = random_generator().uniform_int(1, damage / 2);
                heal(recovered);
                std::cout << name() << " (DieHardSoldier) recovers " << recovered << " HP\n";
            }

            return damage - recovered;
        }

    private:
        double crit_chance_;
    };

    class DieHardMonster : public RegularMonster
    {
        static constexpr double repeat_chance_ = 0.5;

    public:
        DieHardMonster(std::string name, int health, int attack_power, int regen, RandomGenerator& rng)
            : RegularMonster{std::move(name), health, attack_power, regen, rng}

        {
        }

        // Executes this enemy's turn and returns the damage dealt.
        // May repeat the attack with a certain probability.
        int take_turn() override
        {
            int damage = RegularMonster::take_turn();

            if (random_generator().bernoulli(repeat_chance_))
            {
                damage += RegularMonster::take_turn();
                std::cout << name() << " (DieHardMonster) repeats the attack for " << damage << " damage\n";
            }

            return damage;
        }
    };

    class DieHardBehemoth : public RegularBehemoth
    {
        uint8_t turn_counter_ = 0;

    public:
        DieHardBehemoth(std::string name, int health, int attack_power, double crit_chance, RandomGenerator& rng)
            : RegularBehemoth{std::move(name), health, attack_power, crit_chance, rng}
        {
        }

        // Executes this enemy's turn and returns the damage dealt.
        // Repeats the attack every other turn.
        int take_turn() override
        {
            ++turn_counter_;

            int damage = RegularBehemoth::take_turn();

            if (turn_counter_ % 2 == 1)
            {
                damage += RegularBehemoth::take_turn();
                std::cout << name() << " (DieHardBehemoth) repeats the attack for " << damage << " damage\n";
            }

            return damage;
        }
    };
} // namespace Game

#endif /*MONSTERS_HPP_*/
