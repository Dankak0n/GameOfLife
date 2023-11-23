#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <initializer_list>
#include <string>
#include <array>

namespace rules {
    bool whether_it_will_live(int count_of_alive, bool was_it_live) {
        if (was_it_live)
            return count_of_alive == 2 || count_of_alive == 3;
        else
            return count_of_alive == 3;
    }

    const int DEFAULT_FPS = 10;

    const std::array< std::pair<int, int>, 8 > neighbour_cells{{ {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} }};    
}

namespace drawing {

    const float hight_of_cell = 32;
    const float width_of_cell = 32;

    const float left_indent  = 10;
    const float up_indent    = 10;

    sf::Vector2f size_of_cell() {
        return sf::Vector2f(hight_of_cell, width_of_cell);
    }

    sf::Color color_of_cell(bool alive) {
        if (alive)
            return sf::Color::Black;
        else
            return sf::Color::White;
    }

    sf::Vector2f get_cords(int x, int y, int hight, int width) {
        sf::Vector2f cords;

        cords.x = x * hight_of_cell + up_indent;
        cords.y = y * width_of_cell + left_indent;

        return cords;
    }

    int get_milliseconds(int speed) {
        return 1000 / speed;
    }
}

class Cell {
public:
    bool value;
    Cell() : value(0) {}
    Cell(bool _value) : value(_value) {}
    void renew(int count_of_alive) {
        value = rules::whether_it_will_live(count_of_alive, value);
    }
};

template <int HIGHT, int WIDTH>
class LifeGame {
public:
    std::array< std::array<Cell, WIDTH>, HIGHT > field;
    int fps;
    sf::RenderWindow window;

    explicit LifeGame() : fps(rules::DEFAULT_FPS) {}

    explicit LifeGame(std::initializer_list< std::pair<int, int> > initializer_list_of_cords) : LifeGame(initializer_list_of_cords, rules::DEFAULT_FPS) {}

    explicit LifeGame(std::initializer_list< std::pair<int, int> > initializer_list_of_cords, int input_fps) {
        fps = input_fps;
        for (std::pair<int, int> cell_cords : initializer_list_of_cords)
            add_alive_cell(cell_cords);
    }

    void draw() {
        for (int x = 0; x < HIGHT; x++)
            for (int y = 0; y < WIDTH; y++) {
                
                sf::RectangleShape cell_to_draw(drawing::size_of_cell());

                cell_to_draw.setOutlineThickness(2);

                cell_to_draw.setOutlineColor(sf::Color::Black);

                cell_to_draw.setFillColor(drawing::color_of_cell( is_alive(x, y) ));

                cell_to_draw.setPosition(drawing::get_cords(x, y, HIGHT, WIDTH));

                window.draw(cell_to_draw);
            }
    }

    void start() {
        window.create(sf::VideoMode(1000, 1000), "Game of life");
        window.setVerticalSyncEnabled(true);

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            draw();

            window.display();

            sf::sleep( sf::Time( sf::milliseconds( drawing::get_milliseconds( fps ) ) ) );
            
            make_step();

            window.clear(sf::Color::White);
        }

    }

    void add_alive_cell(int x, int y) {
        field[x][y] = true;
    }

    void add_alive_cell(std::pair<int, int> cell_cords) {
        add_alive_cell(cell_cords.first, cell_cords.second);
    }

    bool inside_field(int x, int y) {
        return (0 <= x && x < HIGHT) && (0 <= y && y < WIDTH);
    }

    bool inside_field(std::pair<int, int> cell_cords) {
        return inside_field(cell_cords.first, cell_cords.second);
    }

    bool is_alive(int x, int y) {
        return field[x][y].value;
    }

    bool is_alive(std::pair<int, int> cell_cords) {
        return is_alive(cell_cords.first, cell_cords.second);
    }

    void make_step() {
        std::array<std::array<Cell, WIDTH>, HIGHT> new_field;

        for (int x = 0; x < WIDTH; x++)
            for (int y = 0; y < HIGHT; y++) {
                int count_of_alive_neighbours = 0;

                for (std::pair<int, int> displacement_vector : rules::neighbour_cells) {
                    std::pair<int, int> may_be_neighbour{x, y};

                    may_be_neighbour.first  += displacement_vector.first;
                    may_be_neighbour.second += displacement_vector.second;

                    if (inside_field(may_be_neighbour) && is_alive(may_be_neighbour))
                        count_of_alive_neighbours++;
                }

                new_field[x][y] = rules::whether_it_will_live( count_of_alive_neighbours, is_alive(x, y) );
            }
        field = new_field;
    }

    void ConsoleOut() {
        for (auto row : field) {
            for (auto cell : row)
                std::cout << (cell.value ? '@' : '`');
            std::cout << '\n';
        }
        std::cout << '\n';
    }
};

int main() {
    LifeGame<30, 30> Game{ {10, 10}, {10, 11}, {10, 12} };

    Game.add_alive_cell(20, 20);
    Game.add_alive_cell(20, 21);
    Game.add_alive_cell(20, 22);

    Game.start();

    return 0;
}