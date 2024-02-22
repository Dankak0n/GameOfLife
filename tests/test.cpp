#include <life_game.h>

#include <iostream>

template <int HEIGHT, int WIDTH>
void my_judge(const std::array< std::array<int32_t, WIDTH>, HEIGHT >& arr,
                           std::array< std::array<int32_t, WIDTH>, HEIGHT >& res) {
    static int32_t dx[] = {0, 0, 1, 1, 1, -1, -1, -1};
    static int32_t dy[] = {-1, 1, -1, 0, 1, -1, 0, 1};

    for (int x = 0; x < HEIGHT; x++) {
        for (int y = 0; y < WIDTH; y++) {
            int32_t cnt_alive = 0;
            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (0 <= nx && nx < HEIGHT && 0 <= ny && ny < WIDTH) {
                    cnt_alive += arr[nx][ny];
                }
            }
            if (arr[x][y] == 0) {
                res[x][y] = cnt_alive == 3;
            } else {
                res[x][y] = cnt_alive == 2 || cnt_alive == 3;
            }
        }
    }
}

sf::Color my_color_judge(int32_t color_id) {
    return color_id ? sf::Color::Black : sf::Color::White;
}

#define H 35
#define W 50

int main() {
    LifeGame<H, W> game {{ {3, 3}, {3, 4}, {3, 5} }};
    game.rules->set_max_fps( game.rules->get_max_possible_fps() );
    game.rules->set_height_of_cell(15);
    game.rules->set_width_of_cell(15);
    game.set_judge_field_function(my_judge<H, W>);
    game.set_judge_color_function(my_color_judge);
    game.start();
    return 0;
}