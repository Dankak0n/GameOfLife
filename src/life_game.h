#ifndef LIFEGAME_LIFEGAME_H
#define LIFEGAME_LIFEGAME_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <vector>
#include <string>
#include <array>

template <int HEIGHT, int WIDTH>
class LifeGame {
public:
    class Rules;
    Rules* rules;
private:
    std::array< std::array<int32_t, WIDTH>, HEIGHT > field {}, prev_field {};
    sf::RenderWindow window {};
    
    void (*judge_field) (const std::array< std::array<int32_t, WIDTH>, HEIGHT >&,
                               std::array< std::array<int32_t, WIDTH>, HEIGHT >&)
     = nullptr;

    sf::Color (*judge_color)(int32_t id)
     = nullptr;

    int32_t& get_cell_mouse_points_to() {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        float x, y;
        x = (pos.y - rules->get_up_indent())   / rules->get_height_of_cell();
        y = (pos.x - rules->get_left_indent()) / rules->get_width_of_cell();
        int32_t x_int = static_cast<int>(x);
        int32_t y_int = static_cast<int>(y);
        if (x < 0 || x > HEIGHT || x - x_int < Rules::_EPS || x - x_int > 1 - Rules::_EPS ||
            y < 0 || y > WIDTH  || y - y_int < Rules::_EPS || y - y_int > 1 - Rules::_EPS) {
                return rules->_NOTCELL;
        }
        return field[ static_cast<int>(x) ][ static_cast<int>(y) ];
    }

    sf::Time get_sleep_time_milliseconds(int speed) {
        return sf::Time( sf::milliseconds(1000 / speed) );
    }

    sf::Vector2f get_left_up_corner_pos(int x, int y) {
        sf::Vector2f cords;
        cords.x = x * rules->get_height_of_cell() + rules->get_up_indent();
        cords.y = y * rules->get_width_of_cell()  + rules->get_left_indent();
        return cords;
    }
    
    float get_window_width(int width) {
        return 2.f * rules->get_left_indent() + width * rules->get_width_of_cell();
    }

    float get_window_height(int height) {
        return 2.f * rules->get_up_indent() + height * rules->get_height_of_cell();
    }

    void make_step() {
        judge_field(field, prev_field);
        std::swap(prev_field, field);
    }

    sf::Vector2f get_size_of_cell() {
        return sf::Vector2f(rules->get_width_of_cell(), rules->get_height_of_cell());
    }

    void draw_field() {
        for (int32_t x = 0; x < HEIGHT; x++) {
            for (int32_t y = 0; y < WIDTH; y++) {
                sf::RectangleShape cell_to_draw(get_size_of_cell());
                cell_to_draw.setOutlineThickness(1);
                cell_to_draw.setOutlineColor(sf::Color::Black);
                cell_to_draw.setFillColor( judge_color(get_id(x, y)) );
                sf::Vector2f cords = get_left_up_corner_pos(x, y);
                std::swap(cords.x, cords.y);
                cell_to_draw.setPosition(cords);
                window.draw(cell_to_draw);
            }
        }
    }
public:
    void set_judge_field_function(void (*judge_func) (const std::array< std::array<int32_t, WIDTH>, HEIGHT >&,
                                                            std::array< std::array<int32_t, WIDTH>, HEIGHT >&)) {
        judge_field = judge_func;
    }
    void set_judge_color_function(sf::Color (*judge_func)(int32_t id)) {
        judge_color = judge_func;
    }

    int32_t get_id(int32_t x, int32_t y) {
        if (0 <= x && x < HEIGHT && 0 <= y && y < WIDTH) {
            return field[x][y];
        } else {
            return -1;        
        }
    }
    int32_t get_id(std::pair<int32_t, int32_t> cords) {
        return get_id(cords.first, cords.second);
    }
    int32_t get_id(sf::Vector2i cords) {
        return get_id(cords.x, cords.x);
    }

    void set_id(int32_t x, int32_t y, int32_t id) {
        if (0 <= x && x < HEIGHT && 0 <= y && y < WIDTH) {
            field[x][y] = id;
        }
    }
    void set_id(std::pair<int32_t, int32_t> cords, int32_t id) {
        set_id(cords.first, cords.second, id);
    }
    void set_id(sf::Vector2i cords, int32_t id) {
        set_id(cords.x, cords.x, id);
    }

class Rules {
private:    
    int32_t MIN_POSSIBLE_FPS   {1};
    int32_t MAX_POSSIBLE_FPS   {10000};

    float width_of_cell        {10};
    float height_of_cell       {10};
    float left_indent          {5};
    float up_indent            {5};
    int32_t max_fps            {10000};
public:
    static constexpr float _EPS       {0.05};
    int32_t _NOTCELL {-1};
    
    Rules() = default;
    ~Rules() = default;
    int32_t get_min_possible_fps()      { return MIN_POSSIBLE_FPS; }
    int32_t get_max_possible_fps()      { return MAX_POSSIBLE_FPS; }
    float   get_width_of_cell()         { return width_of_cell;    }
    float   get_height_of_cell()        { return height_of_cell;   }
    float   get_left_indent()           { return left_indent;      }
    float   get_up_indent()             { return up_indent;        }
    int32_t get_max_fps()               { return max_fps;          }

    void set_width_of_cell(float val)   { width_of_cell = val;     }
    void set_height_of_cell(float val)  { height_of_cell = val;    }
    void set_left_indent(float val)     { left_indent = val;       }
    void set_up_indent(float val)       { up_indent = val;         }

    void set_max_fps(int32_t val) {
        val = std::min(val, MAX_POSSIBLE_FPS);
        val = std::max(val, MIN_POSSIBLE_FPS);
        max_fps = val;
    }

    static void life_game_judge(const std::array< std::array<int32_t, WIDTH>, HEIGHT >& arr,
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

    static sf::Color two_colors_judge(int32_t color_id) {
        return color_id ? sf::Color::Black : sf::Color::White;
    }
};

    void output_info(const char* file_name = nullptr) {
        std::ofstream out;
        if (file_name != nullptr) {
            out.open(file_name);
        }
        for (auto row : field) {
            for (auto cell : row) {
                if (file_name != nullptr) {
                    out << cell;
                } else {
                    fprintf(stderr, "%d ", cell);
                }
            }
            if (file_name != nullptr) {
                out << '\n';
            } else {
                fprintf(stderr, "\n");
            }
        }
        if (file_name != nullptr) {
            out << '\n';
        } else {
            fprintf(stderr, "\n");
        }
        if (file_name != nullptr) {
            out.close();
        }
    }

    LifeGame() {
        rules = new Rules();
        judge_field = Rules::life_game_judge;
        judge_color = Rules::two_colors_judge;
    }

    explicit LifeGame (std::initializer_list< std::pair<int, int> > initializer_list_of_cords) : LifeGame() {
        for (const std::pair<int, int>& cell_cords : initializer_list_of_cords) {
            set_id(cell_cords, 1);
        }
    }    
    ~LifeGame() = default;

    void renew_window(const std::string &title) {
        if (!window.isOpen()) {
            window.create(sf::VideoMode( get_window_width(WIDTH), get_window_height(HEIGHT) ), title);
        }
        window.setPosition(sf::Vector2i(200, 200));
        window.setTitle(title);
    }

    bool prepare() {
        renew_window("Press 'S' to start, press '0' for white and '1' for black");
        bool was_released = true;
        int32_t current_color = 1;
        while (true) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return false;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && was_released) {
                int32_t& cell_mouse_points_to = get_cell_mouse_points_to();
                if (cell_mouse_points_to != rules->_NOTCELL) {
                    cell_mouse_points_to = current_color;
                }
                was_released = false;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                was_released = true;
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 's' || event.text.unicode == 'S') {
                    return true;
                }
                if (event.text.unicode == '0') {
                    current_color = 0;
                }
                if (event.text.unicode == '1') {
                    current_color = 1;
                }
            }
            draw_field();
            window.display();
            window.clear(sf::Color::White);
        }

        return false;
    }

    void start() {
        if (!prepare()) return;
        renew_window("Game of life");
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            draw_field();
            window.display();
            sf::sleep(get_sleep_time_milliseconds( rules->get_max_fps() ));
            make_step();
            window.clear(sf::Color::White);
        }
    }
};

#endif // LIFEGAME_LIFEGAME_H
