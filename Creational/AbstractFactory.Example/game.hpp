#ifndef GAME_HPP_
#define GAME_HPP_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "enemy_factory.hpp"
#include "random_generator.hpp"

namespace Game
{
    enum class GameLevel
    {
        easy,
        die_hard
    };

    class Hero
    {
    public:
        Hero(std::string name, int health, int attack_power)
            : name_{std::move(name)}, health_{health}, max_health_{health}, attack_power_{attack_power}
        {
        }

        const std::string& name() const { return name_; }
        int health() const { return health_; }
        bool is_alive() const { return health_ > 0; }

        void reset()
        {
            health_ = max_health_;
        }

        void take_damage(int dmg)
        {
            health_ = std::max(0, health_ - dmg);
        }

        int attack() const
        {
            std::cout << name_ << " strikes back for " << attack_power_ << " damage\n";
            return attack_power_;
        }

    private:
        std::string name_;
        int health_;
        int max_health_;
        int attack_power_;
    };

    class GameApp
    {
        std::vector<std::unique_ptr<Enemy>> enemies_;
        std::unique_ptr<AbstractEnemyFactory> enemy_factory_;
        RandomGenerator& rng_;
        Hero hero_{"Hero", 100, 18};
        int defeated_count_ = 0;

    public:
        explicit GameApp(RandomGenerator& rng)
            : rng_{rng}
        {
        }

        virtual ~GameApp() = default;

        GameApp(const GameApp&) = delete;
        GameApp& operator=(const GameApp&) = delete;

        void select_level(GameLevel level)
        {
            switch (level)
            {
            case GameLevel::easy:
                enemy_factory_ = std::make_unique<EasyLevelEnemyFactory>(rng_);
                break;
            case GameLevel::die_hard:
                enemy_factory_ = std::make_unique<DieHardLevelEnemyFactory>(rng_);
                break;
            }
        }

        void play()
        {
            init_game(12);
            fight();
            show_score();
        }

    protected:
        virtual void init_game(int number_of_enemies)
        {
            assert(enemy_factory_ != nullptr);
            enemies_.clear();
            hero_.reset();
            defeated_count_ = 0;

            // Initialize the list of enemies using the selected enemy factory.
            for (int i = 0; i < number_of_enemies; ++i)
            {
                auto rnd_value = rng_.uniform_int(0, 100);
                if (rnd_value < 40)
                    enemies_.push_back(enemy_factory_->create_soldier());
                else if (rnd_value > 70)
                    enemies_.push_back(enemy_factory_->create_monster());
                else
                    enemies_.push_back(enemy_factory_->create_behemoth());
            }
        }

        virtual void fight()
        {
            for (const auto& e : enemies_)
            {
                if (!hero_.is_alive())
                {
                    std::cout << hero_.name() << " has fallen! Game over.\n";
                    break;
                }

                std::cout << "--- " << e->name() << " approaches ---\n";

                while (e->is_alive() && hero_.is_alive())
                {
                    hero_.take_damage(e->take_turn());
                    if (!hero_.is_alive())
                        break;

                    e->take_damage(hero_.attack());
                }

                if (!e->is_alive())
                {
                    std::cout << e->name() << " is defeated!\n";
                    ++defeated_count_;
                }
            }
        }

        virtual void show_score()
        {
            std::cout << "\n=== Score ===\n"
                       << "Enemies defeated: " << defeated_count_ << "/" << enemies_.size() << "\n"
                       << hero_.name() << " remaining HP: " << hero_.health() << "\n";
        }
    };
}
#endif /*GAME_HPP_*/
