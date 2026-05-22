#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <memory>
#include <functional>

#include "include/Lab3-PiecewiseFunc.h"

class FunctionUI {
private:
    sf::RenderWindow window;
    sf::View view;
    sf::View ui_view;

    PiecewiseFunc<double>* func;

    double x_min, x_max, y_min, y_max;
    double default_x_min, default_x_max, default_y_min, default_y_max;

    bool is_dragging;
    sf::Vector2i last_mouse_pos;
    sf::Vector2f last_view_center;

    sf::Font font;
    std::unique_ptr<sf::Text> coord_text;
    std::unique_ptr<sf::Text> segments_text;

    std::unique_ptr<sf::Text> input_label;
    std::unique_ptr<sf::Text> input_x1_text;
    std::unique_ptr<sf::Text> input_x2_text;
    std::unique_ptr<sf::Text> input_a_text;
    std::unique_ptr<sf::Text> input_b_text;
    std::unique_ptr<sf::Text> input_c_text;
    std::unique_ptr<sf::Text> status_text;
    std::unique_ptr<sf::RectangleShape> input_bg;
    std::unique_ptr<sf::RectangleShape> action_button;
    std::unique_ptr<sf::Text> action_button_text;
    std::unique_ptr<sf::Text> continuous_text;
    std::unique_ptr<sf::Text> monotonic_text;

    std::unique_ptr<sf::RectangleShape> func_buttons[4];
    std::unique_ptr<sf::Text> func_button_texts[4];
    int selected_func_type;

    std::string x1_input;
    std::string x2_input;
    std::string a_input;
    std::string b_input;
    std::string c_input;
    bool is_typing_x1;
    bool is_typing_x2;
    bool is_typing_a;
    bool is_typing_b;
    bool is_typing_c;
    bool show_status;
    float status_timer;

    std::string last_x1;
    std::string last_x2;

    bool loadFont() {
        std::vector<std::string> font_paths = {
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "C:\\Windows\\Fonts\\arial.ttf",
            "C:\\Windows\\Fonts\\segoeui.ttf",
            "/System/Library/Fonts/Helvetica.ttc"
        };

        for (const auto& path : font_paths) {
            if (font.openFromFile(path)) {
                return true;
            }
        }
        return false;
    }

    void updateCoordText(sf::Vector2i mouse_pos) {
        if (!coord_text) return;

        sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos, view);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << "(" << world_pos.x << ", " << -world_pos.y << ")";
        coord_text->setString(ss.str());
        coord_text->setPosition(sf::Vector2f(10, 10));
    }

    void updateSegmentsText() {
        if (!segments_text) return;

        std::stringstream ss;
        if (!func || func->GetSize() == 0) {
            ss << "Segments: 0";
        } else {
            ss << "Segments: " << func->GetSize();
        }
        segments_text->setString(ss.str());
        segments_text->setPosition(sf::Vector2f(10, 40));
    }

    void drawGrid() {
        std::vector<sf::Vertex> grid_lines;

        double x_step = calculateStep(x_min, x_max) / 2;
        double x_start = std::ceil(x_min / x_step) * x_step;

        for (double x = x_start; x <= x_max; x += x_step) {
            sf::Vertex v1, v2;
            v1.position = sf::Vector2f(x, y_min);
            v2.position = sf::Vector2f(x, y_max);

            bool is_major = std::abs(std::fmod(x / x_step, 2.0)) < 0.1;

            if (std::abs(x) < x_step * 0.2) {
                v1.color = sf::Color(200, 200, 200, 255);
                v2.color = sf::Color(200, 200, 200, 255);
            } else if (is_major) {
                v1.color = sf::Color(100, 100, 100, 180);
                v2.color = sf::Color(100, 100, 100, 180);
            } else {
                v1.color = sf::Color(60, 60, 60, 120);
                v2.color = sf::Color(60, 60, 60, 120);
            }

            grid_lines.push_back(v1);
            grid_lines.push_back(v2);
        }

        double y_step = calculateStep(y_min, y_max) / 2;
        double y_start = std::ceil(y_min / y_step) * y_step;

        for (double y = y_start; y <= y_max; y += y_step) {
            sf::Vertex v1, v2;
            v1.position = sf::Vector2f(x_min, y);
            v2.position = sf::Vector2f(x_max, y);

            bool is_major = std::abs(std::fmod(y / y_step, 2.0)) < 0.1;

            if (std::abs(y) < y_step * 0.2) {
                v1.color = sf::Color(200, 200, 200, 255);
                v2.color = sf::Color(200, 200, 200, 255);
            } else if (is_major) {
                v1.color = sf::Color(100, 100, 100, 180);
                v2.color = sf::Color(100, 100, 100, 180);
            } else {
                v1.color = sf::Color(60, 60, 60, 120);
                v2.color = sf::Color(60, 60, 60, 120);
            }

            grid_lines.push_back(v1);
            grid_lines.push_back(v2);
        }

        window.draw(&grid_lines[0], grid_lines.size(), sf::PrimitiveType::Lines);
    }

    double calculateStep(double min, double max) {
        double range = max - min;
        double exponent = std::floor(std::log10(range));
        double fraction = range / std::pow(10, exponent);

        if (fraction < 2) return std::pow(10, exponent) / 2;
        if (fraction < 5) return std::pow(10, exponent);
        return std::pow(10, exponent) * 2;
    }

    void drawFunction() {
        if (!func || func->GetSize() == 0) return;

        std::vector<sf::Vertex> lines;

        double x_range = x_max - x_min;
        int num_points = static_cast<int>(window.getSize().x);
        double step = x_range / num_points;

        double prev_y = 0;
        bool prev_valid = false;
        double max_jump = (y_max - y_min) * 0.5;

        for (int i = 0; i <= num_points; ++i) {
            double x = x_min + i * step;

            try {
                double y = -func->evaluateInFunc(x);

                if (y >= y_min - (y_max - y_min) * 0.5 &&
                    y <= y_max + (y_max - y_min) * 0.5) {

                    if (prev_valid && std::abs(y - prev_y) > max_jump) {
                        if (lines.size() > 1) {
                            window.draw(&lines[0], lines.size(), sf::PrimitiveType::LineStrip);
                            lines.clear();
                        }
                    }

                    sf::Vertex v;
                    v.position = sf::Vector2f(x, y);
                    v.color = sf::Color::Yellow;
                    lines.push_back(v);
                    prev_y = y;
                    prev_valid = true;
                } else {
                    if (lines.size() > 1) {
                        window.draw(&lines[0], lines.size(), sf::PrimitiveType::LineStrip);
                        lines.clear();
                    } else if (!lines.empty()) {
                        lines.clear();
                    }
                    prev_valid = false;
                }
            } catch (const std::out_of_range&) {
                if (lines.size() > 1) {
                    window.draw(&lines[0], lines.size(), sf::PrimitiveType::LineStrip);
                    lines.clear();
                } else if (!lines.empty()) {
                    lines.clear();
                }
                prev_valid = false;
            }
        }

        if (lines.size() > 1) {
            window.draw(&lines[0], lines.size(), sf::PrimitiveType::LineStrip);
        }
    }

    void drawUI() {
        if (!input_bg) return;

        window.draw(*input_bg);

        for (int i = 0; i < 4; i++) {
            if (func_buttons[i]) {
                if (selected_func_type == i) {
                    func_buttons[i]->setFillColor(sf::Color(80, 80, 120));
                } else {
                    func_buttons[i]->setFillColor(sf::Color(60, 60, 80));
                }
                window.draw(*func_buttons[i]);
                if (func_button_texts[i]) window.draw(*func_button_texts[i]);
            }
        }

        if (input_label) window.draw(*input_label);
        if (input_x1_text) window.draw(*input_x1_text);
        if (input_x2_text) window.draw(*input_x2_text);

        if (selected_func_type != 3) {
            if (selected_func_type == 0) {
                if (input_a_text) window.draw(*input_a_text);
                if (input_b_text) window.draw(*input_b_text);
            } else if (selected_func_type == 1) {
                if (input_a_text) window.draw(*input_a_text);
                if (input_b_text) window.draw(*input_b_text);
                if (input_c_text) window.draw(*input_c_text);
            } else if (selected_func_type == 2) {
                if (input_a_text) window.draw(*input_a_text);
                if (input_b_text) window.draw(*input_b_text);
                if (input_c_text) window.draw(*input_c_text);
            }
        }

        if (action_button) window.draw(*action_button);
        if (action_button_text) window.draw(*action_button_text);

        if (selected_func_type == 3) {
            if (continuous_text) window.draw(*continuous_text);
            if (monotonic_text) window.draw(*monotonic_text);
        }

        if (status_text && show_status) window.draw(*status_text);
    }

    void getCurrentFunction(Segment<double>& seg) {
        double a = 0.0, b = 0.0, c = 0.0;
        switch (selected_func_type) {
            case 0:
            case 1:
                a = 1.0;
                break;
            case 2:
                a = 1.0;
                b = 1.0;
                break;
            default:
                break;
        }

        if (!a_input.empty()) a = std::stod(a_input);
        if (!b_input.empty()) b = std::stod(b_input);
        if (!c_input.empty()) c = std::stod(c_input);

        seg.is_continuous = true;
        seg.is_monotonic = true;
        switch (selected_func_type) {
            case 0:
                seg.func = [a, b](double x) { return a * x + b; };
                return;
            case 1:
                seg.func = [a, b, c](double x) { return a * x * x + b * x + c; };
                if (a != 0 && seg.start < -b/(2*a) && seg.end > -b/(2*a)) seg.is_monotonic = false;
                return;
            case 2:
                seg.func = [a, b, c](double x) { return a * std::sin(b * x + c); };
                if (b != 0) {
                    double exstr = (c * 2 + M_PI + -100 * M_PI * 2)/(b*2);
                    for (int n = -100; n <= 100; n++) {
                        if (seg.start < exstr && seg.end > exstr) {
                            seg.is_monotonic = false;
                            return;
                        }
                        exstr += M_PI/b;
                    }
                }
        }
    }

    void addFunction() {
        std::string use_x1 = x1_input.empty() ? last_x1 : x1_input;
        std::string use_x2 = x2_input.empty() ? last_x2 : x2_input;

        if (use_x1.empty() || use_x2.empty()) {
            showMessage("Please enter x1 and x2!");
            return;
        }

        try {
            double x1 = std::stod(use_x1);
            double x2 = std::stod(use_x2);

            if (x1 >= x2) {
                showMessage("x1 must be less than x2!");
                return;
            }

            last_x1 = use_x1;
            last_x2 = use_x2;

            Segment new_seg(x1, x2);
            getCurrentFunction(new_seg);
            func->RedefineOnInterval(new_seg);

            x1_input.clear();
            x2_input.clear();
            a_input.clear();
            b_input.clear();
            c_input.clear();

            updateInputDisplay();
            updateSegmentsText();

            showMessage("Function added!");

        } catch (const std::exception& e) {
            showMessage("Invalid input!");
        }
    }

    void checkProperties() {
        if (!func || func->GetSize() == 0) {
            if (continuous_text) continuous_text->setString("Continuous: No function");
            if (monotonic_text) monotonic_text->setString("Monotonic: No function");
            return;
        }

        std::string use_x1 = x1_input.empty() ? last_x1 : x1_input;
        std::string use_x2 = x2_input.empty() ? last_x2 : x2_input;

        if (use_x1.empty() || use_x2.empty()) {
            if (continuous_text) continuous_text->setString("Continuous: Enter x1,x2");
            if (monotonic_text) monotonic_text->setString("Monotonic: Enter x1,x2");
            return;
        }

        double x1 = std::stod(use_x1);
        double x2 = std::stod(use_x2);

        if (x1 >= x2) {
            if (continuous_text) continuous_text->setString("Continuous: x1 < x2 needed");
            if (monotonic_text) monotonic_text->setString("Monotonic: x1 < x2 needed");
            return;
        }

        try {
            bool is_continuous = func->isContinuousOnInterval(x1, x2);
            if (continuous_text) {
                std::stringstream ss;
                ss << "Continuous: " << (is_continuous ? "Yes" : "No");
                continuous_text->setString(ss.str());
            }
        } catch (const std::exception& e) {
            if (continuous_text) continuous_text->setString("Continuous: ?");
        }
        try {
            bool is_monotonic = func->isMonotonicOnInterval(x1, x2);
            if (monotonic_text) {
                std::stringstream ss;
                ss << "Monotonic: " << (is_monotonic ? "Yes" : "No");
                monotonic_text->setString(ss.str());
            }
        } catch (const std::exception& e) {
            if (monotonic_text) monotonic_text->setString("Monotonic: ?");
        }

        last_x1 = use_x1;
        last_x2 = use_x2;
    }

    void handleUIInput(const sf::Event::KeyPressed* key) {
        if (!is_typing_x1 && !is_typing_x2 && !is_typing_a && !is_typing_b && !is_typing_c) return;

        if (key->code == sf::Keyboard::Key::Enter) {
            if (is_typing_x1) {
                is_typing_x1 = false;
                is_typing_x2 = true;
                if (input_x2_text) input_x2_text->setFillColor(sf::Color::White);
            } else if (is_typing_x2) {
                is_typing_x2 = false;
                if (selected_func_type != 3) {
                    is_typing_a = true;
                    if (input_a_text) input_a_text->setFillColor(sf::Color::White);
                } else {
                    checkProperties();
                }
            } else if (is_typing_a) {
                is_typing_a = false;
                is_typing_b = true;
                if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            } else if (is_typing_b) {
                is_typing_b = false;
                if (selected_func_type >= 1) {
                    is_typing_c = true;
                    if (input_c_text) input_c_text->setFillColor(sf::Color::White);
                } else {
                    addFunction();
                }
            } else if (is_typing_c) {
                is_typing_c = false;
                addFunction();
            }
        }
        else if (key->code == sf::Keyboard::Key::Backspace) {
            if (is_typing_x1 && !x1_input.empty()) {
                x1_input.pop_back();
                updateInputDisplay();
            } else if (is_typing_x2 && !x2_input.empty()) {
                x2_input.pop_back();
                updateInputDisplay();
            } else if (is_typing_a && !a_input.empty()) {
                a_input.pop_back();
                updateInputDisplay();
            } else if (is_typing_b && !b_input.empty()) {
                b_input.pop_back();
                updateInputDisplay();
            } else if (is_typing_c && !c_input.empty()) {
                c_input.pop_back();
                updateInputDisplay();
            }
        }
        else if (key->code == sf::Keyboard::Key::Escape) {
            is_typing_x1 = is_typing_x2 = is_typing_a = is_typing_b = is_typing_c = false;
        }
    }

    void handleUITextInput(char32_t unicode) {
        if (!is_typing_x1 && !is_typing_x2 && !is_typing_a && !is_typing_b && !is_typing_c) return;

        if ((unicode >= U'0' && unicode <= U'9') || unicode == U'-' || unicode == U'.') {
            if (is_typing_x1 && x1_input.length() < 10) {
                x1_input += static_cast<char>(unicode);
                updateInputDisplay();
            } else if (is_typing_x2 && x2_input.length() < 10) {
                x2_input += static_cast<char>(unicode);
                updateInputDisplay();
            } else if (is_typing_a && a_input.length() < 10) {
                a_input += static_cast<char>(unicode);
                updateInputDisplay();
            } else if (is_typing_b && b_input.length() < 10) {
                b_input += static_cast<char>(unicode);
                updateInputDisplay();
            } else if (is_typing_c && c_input.length() < 10) {
                c_input += static_cast<char>(unicode);
                updateInputDisplay();
            }
        }
    }

    void updateInputDisplay() {
        if (input_x1_text) {
            std::string display = x1_input.empty() ? last_x1 : x1_input;
            if (display.empty()) display = "_";
            if (is_typing_x1) display += "|";
            input_x1_text->setString("x1: " + display);
        }
        if (input_x2_text) {
            std::string display = x2_input.empty() ? last_x2 : x2_input;
            if (display.empty()) display = "_";
            if (is_typing_x2) display += "|";
            input_x2_text->setString("x2: " + display);
        }
        if (input_a_text) {
            std::string display = a_input.empty() ? "1" : a_input;
            if (is_typing_a) display += "|";
            input_a_text->setString("a: " + display);
        }
        if (input_b_text) {
            std::string display = b_input.empty() ? (selected_func_type == 2 ? "1" : "0") : b_input;
            if (is_typing_b) display += "|";
            input_b_text->setString("b: " + display);
        }
        if (input_c_text) {
            std::string display = c_input.empty() ? "0" : c_input;
            if (is_typing_c) display += "|";
            input_c_text->setString("c: " + display);
        }
    }

    void showMessage(const std::string& msg) {
        if (status_text) {
            status_text->setString(msg);
            show_status = true;
            status_timer = 3.0f;
        }
    }

    void updateStatus(float delta_time) {
        if (show_status) {
            status_timer -= delta_time;
            if (status_timer <= 0) {
                show_status = false;
                if (status_text) status_text->setString("");
            }
        }
    }

    void handleMouseClick(sf::Vector2i mouse_pos) {
        sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos, ui_view);

        for (int i = 0; i < 4; i++) {
            if (func_buttons[i] && func_buttons[i]->getGlobalBounds().contains(world_pos)) {
                selected_func_type = i;
                a_input.clear();
                b_input.clear();
                c_input.clear();

                if (action_button_text) {
                    if (selected_func_type == 3) {
                        action_button_text->setString("Check");
                    } else {
                        action_button_text->setString("Add");
                    }
                }

                updateInputDisplay();
                showMessage("Selected: " + getFunctionName(i));
                return;
            }
        }

        if (action_button && action_button->getGlobalBounds().contains(world_pos)) {
            if (selected_func_type == 3) {
                checkProperties();
            } else {
                addFunction();
            }
            return;
        }

        if (input_x1_text && input_x1_text->getGlobalBounds().contains(world_pos)) {
            is_typing_x1 = true;
            is_typing_x2 = is_typing_a = is_typing_b = is_typing_c = false;
            input_x1_text->setFillColor(sf::Color::Yellow);
            input_x2_text->setFillColor(sf::Color::White);
            if (input_a_text) input_a_text->setFillColor(sf::Color::White);
            if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            if (input_c_text) input_c_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
        else if (input_x2_text && input_x2_text->getGlobalBounds().contains(world_pos)) {
            is_typing_x2 = true;
            is_typing_x1 = is_typing_a = is_typing_b = is_typing_c = false;
            input_x2_text->setFillColor(sf::Color::Yellow);
            input_x1_text->setFillColor(sf::Color::White);
            if (input_a_text) input_a_text->setFillColor(sf::Color::White);
            if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            if (input_c_text) input_c_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
        else if (input_a_text && input_a_text->getGlobalBounds().contains(world_pos)) {
            is_typing_a = true;
            is_typing_x1 = is_typing_x2 = is_typing_b = is_typing_c = false;
            input_a_text->setFillColor(sf::Color::Yellow);
            input_x1_text->setFillColor(sf::Color::White);
            input_x2_text->setFillColor(sf::Color::White);
            if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            if (input_c_text) input_c_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
        else if (input_b_text && input_b_text->getGlobalBounds().contains(world_pos)) {
            is_typing_b = true;
            is_typing_x1 = is_typing_x2 = is_typing_a = is_typing_c = false;
            input_b_text->setFillColor(sf::Color::Yellow);
            input_x1_text->setFillColor(sf::Color::White);
            input_x2_text->setFillColor(sf::Color::White);
            if (input_a_text) input_a_text->setFillColor(sf::Color::White);
            if (input_c_text) input_c_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
        else if (input_c_text && input_c_text->getGlobalBounds().contains(world_pos)) {
            is_typing_c = true;
            is_typing_x1 = is_typing_x2 = is_typing_a = is_typing_b = false;
            input_c_text->setFillColor(sf::Color::Yellow);
            input_x1_text->setFillColor(sf::Color::White);
            input_x2_text->setFillColor(sf::Color::White);
            if (input_a_text) input_a_text->setFillColor(sf::Color::White);
            if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
        else {
            is_typing_x1 = is_typing_x2 = is_typing_a = is_typing_b = is_typing_c = false;
            input_x1_text->setFillColor(sf::Color::White);
            input_x2_text->setFillColor(sf::Color::White);
            if (input_a_text) input_a_text->setFillColor(sf::Color::White);
            if (input_b_text) input_b_text->setFillColor(sf::Color::White);
            if (input_c_text) input_c_text->setFillColor(sf::Color::White);
            updateInputDisplay();
        }
    }

    std::string getFunctionName(int type) {
        switch (type) {
            case 0: return "y = a*x + b";
            case 1: return "y = a*x^2 + b*x + c";
            case 2: return "y = a*sin(b*x + c)";
            case 3: return "Check properties";
            default: return "Unknown";
        }
    }

    void handleZoom(sf::Vector2i mouse_pos, float delta) {
        sf::Vector2f before = window.mapPixelToCoords(mouse_pos, view);

        float zoom_factor = (delta > 0) ? 0.9f : 1.1f;
        view.zoom(zoom_factor);

        sf::Vector2f after = window.mapPixelToCoords(mouse_pos, view);
        view.move(before - after);

        updateViewBounds();
        window.setView(view);
    }

    void updateViewBounds() {
        sf::Vector2f center = view.getCenter();
        sf::Vector2f size = view.getSize();

        x_min = center.x - size.x / 2;
        x_max = center.x + size.x / 2;
        y_min = center.y - size.y / 2;
        y_max = center.y + size.y / 2;
    }

    void handleInput() {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                handleUIInput(key);

                switch (key->code) {
                    case sf::Keyboard::Key::R:
                        resetView();
                        break;
                    case sf::Keyboard::Key::Escape:
                        if (!is_typing_x1 && !is_typing_x2 && !is_typing_a && !is_typing_b && !is_typing_c)
                            window.close();
                        break;
                    case sf::Keyboard::Key::A:
                    case sf::Keyboard::Key::Left:
                        view.move(sf::Vector2f(-1, 0));
                        updateViewBounds();
                        window.setView(view);
                        break;
                    case sf::Keyboard::Key::D:
                    case sf::Keyboard::Key::Right:
                        view.move(sf::Vector2f(1, 0));
                        updateViewBounds();
                        window.setView(view);
                        break;
                    case sf::Keyboard::Key::W:
                    case sf::Keyboard::Key::Up:
                        view.move(sf::Vector2f(0, -1));
                        updateViewBounds();
                        window.setView(view);
                        break;
                    case sf::Keyboard::Key::S:
                    case sf::Keyboard::Key::Down:
                        view.move(sf::Vector2f(0, 1));
                        updateViewBounds();
                        window.setView(view);
                        break;
                    default:
                        break;
                }
            }
            else if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
                handleUITextInput(text->unicode);
            }
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Middle) {
                    is_dragging = true;
                    last_mouse_pos = sf::Mouse::getPosition(window);
                    last_view_center = view.getCenter();
                }
                else if (mouse->button == sf::Mouse::Button::Left) {
                    handleMouseClick(sf::Mouse::getPosition(window));
                }
            }
            else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse->button == sf::Mouse::Button::Middle) {
                    is_dragging = false;
                }
            }
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
                if (is_dragging) {
                    sf::Vector2i current_pos = sf::Mouse::getPosition(window);
                    sf::Vector2i delta = last_mouse_pos - current_pos;

                    sf::Vector2f move(static_cast<float>(delta.x), static_cast<float>(delta.y));
                    move.x *= view.getSize().x / window.getSize().x;
                    move.y *= view.getSize().y / window.getSize().y;

                    view.setCenter(last_view_center + move);
                    window.setView(view);
                    updateViewBounds();
                }

                updateCoordText(sf::Mouse::getPosition(window));
            }
            else if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                handleZoom(sf::Mouse::getPosition(window), scroll->delta);
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea(sf::Vector2f(0, 0), sf::Vector2f(resized->size.x, resized->size.y));
                window.setView(sf::View(visibleArea));
                view = window.getView();
                updateViewBounds();

                ui_view = sf::View(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(resized->size.x, resized->size.y)));
                initUIElements();
            }
        }
    }

    void initUIElements() {
        float window_width = window.getSize().x;
        float window_height = window.getSize().y;

        input_bg = std::make_unique<sf::RectangleShape>(sf::Vector2f(180, 450));
        input_bg->setPosition(sf::Vector2f(window_width - 190, window_height - 460));
        input_bg->setFillColor(sf::Color(40, 40, 50, 230));
        input_bg->setOutlineColor(sf::Color(100, 100, 120));
        input_bg->setOutlineThickness(2);

        input_label = std::make_unique<sf::Text>(font);
        input_label->setString("Function tools");
        input_label->setCharacterSize(14);
        input_label->setFillColor(sf::Color(200, 200, 200));
        input_label->setPosition(sf::Vector2f(window_width - 180, window_height - 440));

        const char* func_names[] = {"y = a*x + b", "y = a*x^2 + b*x + c", "y = a*sin(b*x + c)", "Check properties"};
        for (int i = 0; i < 4; i++) {
            func_buttons[i] = std::make_unique<sf::RectangleShape>(sf::Vector2f(150, 25));
            func_buttons[i]->setPosition(sf::Vector2f(window_width - 180, window_height - 400 + i * 30));
            func_buttons[i]->setFillColor(sf::Color(60, 60, 80));
            func_buttons[i]->setOutlineColor(sf::Color(120, 120, 140));
            func_buttons[i]->setOutlineThickness(1);

            func_button_texts[i] = std::make_unique<sf::Text>(font);
            func_button_texts[i]->setString(func_names[i]);
            func_button_texts[i]->setCharacterSize(11);
            func_button_texts[i]->setFillColor(sf::Color::White);
            func_button_texts[i]->setPosition(sf::Vector2f(window_width - 175, window_height - 396 + i * 30));
        }

        input_x1_text = std::make_unique<sf::Text>(font);
        input_x1_text->setString("x1: _");
        input_x1_text->setCharacterSize(13);
        input_x1_text->setFillColor(sf::Color::White);
        input_x1_text->setPosition(sf::Vector2f(window_width - 180, window_height - 270));

        input_x2_text = std::make_unique<sf::Text>(font);
        input_x2_text->setString("x2: _");
        input_x2_text->setCharacterSize(13);
        input_x2_text->setFillColor(sf::Color::White);
        input_x2_text->setPosition(sf::Vector2f(window_width - 180, window_height - 245));

        input_a_text = std::make_unique<sf::Text>(font);
        input_a_text->setString("a: 1");
        input_a_text->setCharacterSize(13);
        input_a_text->setFillColor(sf::Color::White);
        input_a_text->setPosition(sf::Vector2f(window_width - 180, window_height - 220));

        input_b_text = std::make_unique<sf::Text>(font);
        input_b_text->setString("b: 0");
        input_b_text->setCharacterSize(13);
        input_b_text->setFillColor(sf::Color::White);
        input_b_text->setPosition(sf::Vector2f(window_width - 180, window_height - 195));

        input_c_text = std::make_unique<sf::Text>(font);
        input_c_text->setString("c: 0");
        input_c_text->setCharacterSize(13);
        input_c_text->setFillColor(sf::Color::White);
        input_c_text->setPosition(sf::Vector2f(window_width - 180, window_height - 170));

        action_button = std::make_unique<sf::RectangleShape>(sf::Vector2f(80, 28));
        action_button->setPosition(sf::Vector2f(window_width - 170, window_height - 135));
        action_button->setFillColor(sf::Color(60, 120, 60));
        action_button->setOutlineColor(sf::Color(100, 180, 100));
        action_button->setOutlineThickness(2);

        action_button_text = std::make_unique<sf::Text>(font);
        action_button_text->setString("Add");
        action_button_text->setCharacterSize(14);
        action_button_text->setFillColor(sf::Color::White);
        action_button_text->setPosition(sf::Vector2f(window_width - 145, window_height - 131));

        continuous_text = std::make_unique<sf::Text>(font);
        continuous_text->setString("Continuous: -");
        continuous_text->setCharacterSize(11);
        continuous_text->setFillColor(sf::Color(180, 180, 180));
        continuous_text->setPosition(sf::Vector2f(window_width - 180, window_height - 95));

        monotonic_text = std::make_unique<sf::Text>(font);
        monotonic_text->setString("Monotonic: -");
        monotonic_text->setCharacterSize(11);
        monotonic_text->setFillColor(sf::Color(180, 180, 180));
        monotonic_text->setPosition(sf::Vector2f(window_width - 180, window_height - 75));

        status_text = std::make_unique<sf::Text>(font);
        status_text->setCharacterSize(11);
        status_text->setFillColor(sf::Color::Yellow);
        status_text->setPosition(sf::Vector2f(window_width - 180, window_height - 45));
    }

public:
    FunctionUI(unsigned int width = 1024, unsigned int height = 768)
        : window(sf::VideoMode({width, height}), "Piecewise Function Visualizer", sf::Style::Default)
        , func(nullptr)
        , is_dragging(false)
        , x_min(0), x_max(0), y_min(0), y_max(0)
        , default_x_min(-60), default_x_max(60), default_y_min(-40), default_y_max(40)
        , coord_text(nullptr)
        , segments_text(nullptr)
        , selected_func_type(0)
        , is_typing_x1(false), is_typing_x2(false)
        , is_typing_a(false), is_typing_b(false), is_typing_c(false)
        , show_status(false), status_timer(0)
        , last_x1(""), last_x2("")
    {
        window.setFramerateLimit(60);
        ui_view = sf::View(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(width, height)));
        resetView();

        if (loadFont()) {
            coord_text = std::make_unique<sf::Text>(font);
            segments_text = std::make_unique<sf::Text>(font);

            coord_text->setCharacterSize(25);
            coord_text->setFillColor(sf::Color::White);
            coord_text->setOutlineColor(sf::Color::Black);
            coord_text->setOutlineThickness(1);

            segments_text->setCharacterSize(20);
            segments_text->setFillColor(sf::Color(180, 180, 180));
            segments_text->setOutlineColor(sf::Color::Black);
            segments_text->setOutlineThickness(1);
            segments_text->setPosition(sf::Vector2f(10, 40));

            initUIElements();
            updateSegmentsText();
        } else {
            coord_text = nullptr;
            segments_text = nullptr;
        }
    }

    void setFunction(PiecewiseFunc<double>* f) {
        func = f;
        updateSegmentsText();
    }

    void resetView() {
        view = sf::View(sf::FloatRect(sf::Vector2f(default_x_min, default_y_min),
                                      sf::Vector2f(default_x_max - default_x_min,
                                                   default_y_max - default_y_min)));
        window.setView(view);
        updateViewBounds();
    }

    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            float delta_time = clock.restart().asSeconds();

            handleInput();
            updateStatus(delta_time);

            window.clear(sf::Color(30, 30, 40));

            window.setView(view);
            drawGrid();
            drawFunction();

            window.setView(ui_view);
            if (coord_text) window.draw(*coord_text);
            if (segments_text) window.draw(*segments_text);
            drawUI();

            window.display();
        }
    }

    ~FunctionUI() = default;
};

int main() {
    PiecewiseFunc<double> pwf;

    auto *segs = new Segment<double>[200];

    FunctionUI ui(1280, 720);
    ui.setFunction(&pwf);
    ui.run();

    delete[] segs;
    return 0;
}