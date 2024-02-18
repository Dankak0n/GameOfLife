#ifndef LIFEGAME_LIFEGAME_H
#define LIFEGAME_LIFEGAME_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <initializer_list>
#include <string>
#include <array>

template <int HEIGHT, int WIDTH>
class LifeGame {
private:
    class Cell {
    public:
        bool value;
        explicit Cell() : value(0) {}
        explicit Cell(bool _value) : value(_value) {}
        void inverse() { value = !value; }
    };

    float width_of_cell  = 30;
    float height_of_cell = 30;
    float left_indent    = 5;
    float up_indent      = 5;
    int max_fps          = 10;

    sf::Color color_of_alive = sf::Color::Black;
    sf::Color color_of_dead  = sf::Color::White;

    const sf::Vector2i NOTCELL{-1, -1};
    const int MIN_POSSIBLE_FPS = 1;
    const int MAX_POSSIBLE_FPS = 100;

    const std::array< sf::Vector2i, 5 > glider_mask
    {{ {0, 1}, {1, 1}, {1, 0}, {1, -1}, {-1, -1} }};

    template<size_t LEN>
    void add_shape(const std::array< sf::Vector2i, LEN > &mask, const sf::Vector2i &cell_vec) {
        for (sf::Vector2i displacement_vector : mask) {
            sf::Vector2i cell_of_glider = cell_vec + displacement_vector;
            if (inside_field(cell_of_glider)) {
                set_cell_value(cell_of_glider, true);
            }
        }
    }

    sf::Vector2f size_of_cell() {
        return sf::Vector2f(width_of_cell, height_of_cell);
    }

    sf::Color color_of_cell(bool alive) {
        if (alive) return color_of_alive;
        else       return color_of_dead;
    }

    sf::Vector2f get_left_up_corner_pos(int x, int y) {
        sf::Vector2f cords;
        cords.x = x * height_of_cell + up_indent;
        cords.y = y * width_of_cell  + left_indent;
        return cords;
    }

    sf::Time get_sleep_time_milliseconds(int speed) {
        return sf::Time( sf::milliseconds(1000 / speed) );
    }

    float window_width(int width) {
        return 2.f * left_indent + width * width_of_cell;
    }

    float window_height(int height) {
        return 2.f * up_indent + height * height_of_cell;
    }

    bool set_max_fps(int new_max_fps) {
        if (MIN_POSSIBLE_FPS <= new_max_fps && new_max_fps <= MAX_POSSIBLE_FPS) {
            max_fps = new_max_fps;
            return true;
        }
        return false;
    }

    int get_max_fps() { return max_fps; }

    void set_colors(sf::Color new_color_of_alive, sf::Color new_color_of_dead) {
        color_of_alive = new_color_of_alive;
        color_of_dead  = new_color_of_dead;
    }

    void set_cell_size(float height, float width) {
        height_of_cell = height;
        width_of_cell  = width;
    }

    float get_up_indent()      { return up_indent; }
    float get_left_indent()    { return left_indent; }
    float get_height_of_cell() { return height_of_cell; }
    float get_width_of_cell()  { return width_of_cell; }




    bool inside_field(const int32_t x, const int32_t y)              { return (0 <= x && x < HEIGHT) && (0 <= y && y < WIDTH); }
    bool inside_field(const std::pair<int32_t, int32_t> &cell_cords) { return inside_field(cell_cords.first, cell_cords.second); }
    bool inside_field(const sf::Vector2i &cell_vec)                  { return inside_field(cell_vec.x, cell_vec.y); }

    bool add_alive_cell(const int32_t x, const int32_t y) {
        if (!inside_field(x, y)) return false;
        field[x][y].value = true;
        return true;
    }
    bool add_alive_cell(const std::pair<int32_t, int32_t> &cell_cords) { return add_alive_cell(cell_cords.first, cell_cords.second); }
    bool add_alive_cell(const sf::Vector2i &cell_vec)                  { return add_alive_cell(cell_vec.x, cell_vec.y); }

    bool is_alive(const int32_t x, const int32_t y)              { return inside_field(x, y) && field[x][y].value; }
    bool is_alive(const std::pair<int32_t, int32_t> &cell_cords) { return is_alive(cell_cords.first, cell_cords.second); }
    bool is_alive(const sf::Vector2i &cell_vec)                  { return is_alive(cell_vec.x, cell_vec.y); }

    bool inverse_cell(const sf::Vector2i &cords) {
        if (!inside_field(cords)) return false;
        field[cords.x][cords.y].value = !field[cords.x][cords.y].value;
        return true;
    }

    bool set_cell_value(const sf::Vector2i &cell_vec, bool value) {
        if (!inside_field(cell_vec)) return false;
        field[cell_vec.x][cell_vec.y].value = value;
        return true;
    }

    void make_step() {
        std::array< std::array<Cell, WIDTH>, HEIGHT > new_field;
        for (int x = 0; x < HEIGHT; x++)
            for (int y = 0; y < WIDTH; y++)
                new_field[x][y].value = JudgeFunc(x, y, field);
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

    bool JudgeFunc(int x, int y, const std::array< std::array<Cell, WIDTH>, HEIGHT > &cur_field) {
        std::array<sf::Vector2i, 8> neighbour_cells
        {{ {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} }};

        int cnt_alive_neighbours = 0;
        for (const sf::Vector2i &displacement_vector : neighbour_cells) {
            std::pair<int, int> may_be_neighbour{x, y};
            may_be_neighbour.first += displacement_vector.x;
            may_be_neighbour.second += displacement_vector.y;
            if (inside_field(may_be_neighbour) && is_alive(may_be_neighbour))
                cnt_alive_neighbours++;
        }
        if (is_alive(x, y))
            return cnt_alive_neighbours == 2 || cnt_alive_neighbours == 3;
        else
            return cnt_alive_neighbours == 3;
    }

    void draw_current_field() {
        for (int x = 0; x < HEIGHT; x++)
            for (int y = 0; y < WIDTH; y++) {
                sf::RectangleShape cell_to_draw(size_of_cell());
                cell_to_draw.setOutlineThickness(1);
                cell_to_draw.setOutlineColor(sf::Color::Black);
                cell_to_draw.setFillColor( color_of_cell(is_alive(x, y)) );
                sf::Vector2f cords = get_left_up_corner_pos(x, y);
                std::swap(cords.x, cords.y);
                cell_to_draw.setPosition(cords);
                window.draw(cell_to_draw);
            }
    }

    std::array< std::array< Cell, WIDTH >, HEIGHT > field;
    sf::RenderWindow window;
public:

    LifeGame() {}

    explicit LifeGame
    (std::initializer_list< std::pair<int, int> > initializer_list_of_cords) : LifeGame() {
        for (const std::pair<int, int> &cell_cords : initializer_list_of_cords)
            add_alive_cell(cell_cords);
    }

    sf::Vector2i get_cell_mouse_points_to() {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        float x, y;

        x = (pos.y - get_up_indent())   / get_height_of_cell();
        y = (pos.x - get_left_indent()) / get_width_of_cell();

        if (x < 0 || x > HEIGHT || x - static_cast<int>(x) < 0.05 || x - static_cast<int>(x) > 0.95 ||
            y < 0 || y > WIDTH  || y - static_cast<int>(y) < 0.05 || y - static_cast<int>(y) > 0.95) {
                return NOTCELL;
            }

        return sf::Vector2i( static_cast<int>(x), static_cast<int>(y) );
    }

    void create_window(const std::string &title) {
        if (!window.isOpen())
            window.create(sf::VideoMode( window_width(WIDTH), window_height(HEIGHT) ), title);
        window.setTitle(title);
    }

    bool prepare() {
        create_window("Press 'S' to start");

        bool was_released = true;

        while (true) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return false;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && was_released) {
                sf::Vector2i cell_mouse_points_to = get_cell_mouse_points_to();

                if (cell_mouse_points_to != NOTCELL) {
                    inverse_cell(cell_mouse_points_to);
                }

                was_released = false;
            }

            if (event.type == sf::Event::MouseButtonReleased)
                was_released = true;

            if (event.type == sf::Event::TextEntered)
                if (event.text.unicode == 's' || event.text.unicode == 'S')
                    return true;

            draw_current_field();

            window.display();

            window.clear(sf::Color::White);
        }

        return false;
    }

    void start() {

        if (!prepare()) return;

        create_window("Game of life");

        bool was_released = true;

        while (window.isOpen()) {
            sf::Event event;

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && was_released) {
                sf::Vector2i cell_mouse_points_to = get_cell_mouse_points_to();

                if (cell_mouse_points_to != NOTCELL) {
                    add_shape(glider_mask, cell_mouse_points_to);
                }

                was_released = false;
            }

            if (event.type == sf::Event::MouseButtonReleased)
                was_released = true;

            draw_current_field();

            window.display();

            sf::sleep(get_sleep_time_milliseconds( get_max_fps() ));

            make_step();

            window.clear(sf::Color::White);
        }

    }
};

#endif // LIFEGAME_LIFEGAME_H
