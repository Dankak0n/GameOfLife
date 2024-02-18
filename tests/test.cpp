#include <life_game.h>

int main() {
    LifeGame<10, 20> Game {{ {3, 3}, {3, 4}, {3, 5} }};

    Game.start();

    return 0;
}