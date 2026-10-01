#include "game.hpp"
#include "random_generator.hpp"

using namespace std;
using namespace Game;

int main()
{
    random_device rd{};
    RandGen rng{std::mt19937{rd()}};

    GameApp game{rng};
    game.select_level(GameLevel::easy);
    game.play();

    cout << "\n\n";

    game.select_level(GameLevel::die_hard);    
    game.play();
}
