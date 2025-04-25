#include <sqlite3.h>

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "front.hpp"
#include "visual.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Data Structure Comparator");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Error loading font! Try: sudo apt install fonts-dejavu\n";
        return 1;
    }

    AppState currentState = MODE_SELECTION;
    Mode selectedMode;
    InputMethod selectedInputMethod;
    FileInputDialog fileDialog(font);

    std::string resultLines;

    auto createButton = [&](float y, const std::string& text) {
        sf::RectangleShape btn(sf::Vector2f(350, 80));
        btn.setPosition(225, y);
        btn.setFillColor(sf::Color(70, 70, 70));
        btn.setOutlineThickness(2);
        btn.setOutlineColor(sf::Color(120, 120, 120));

        sf::Text btnText(text, font, 30);
        btnText.setFillColor(sf::Color::White);
        sf::FloatRect bounds = btnText.getLocalBounds();
        btnText.setOrigin(bounds.width / 2, bounds.height / 2);
        btnText.setPosition(400, y + 40);

        return std::make_pair(btn, btnText);
    };

    std::vector<std::pair<sf::RectangleShape, sf::Text>> modeButtons = {
        createButton(200, "Optimized Tree"), createButton(320, "std::set"),
        createButton(440, "Both")};

    std::vector<std::pair<sf::RectangleShape, sf::Text>> inputButtons = {
        createButton(200, "From File"), createButton(440, "From Database")};

    sf::Text modeTitle("Choose comparison mode:", font, 40);
    modeTitle.setPosition(200, 80);
    modeTitle.setFillColor(sf::Color::White);

    sf::Text inputTitle("Select input method:", font, 40);
    inputTitle.setPosition(200, 80);
    inputTitle.setFillColor(sf::Color::White);

    sf::RectangleShape backButton(sf::Vector2f(200, 50));
    backButton.setPosition(300, 500);
    backButton.setFillColor(sf::Color(100, 100, 100));

    sf::Text backButtonText("Back to Menu", font, 25);
    backButtonText.setPosition(320, 510);
    backButtonText.setFillColor(sf::Color::White);

    std::vector<std::string> dbTests = {"Test 1", "Test 2", "Test 3"};

    // Кнопки для выбора тестов
    std::vector<std::pair<sf::RectangleShape, sf::Text>> dbTestButtons;
    for (size_t i = 0; i < dbTests.size(); ++i) {
        auto btn = createButton(200 + i * 100, dbTests[i]);
        dbTestButtons.push_back(btn);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            sf::Vector2f mousePos =
                window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            if (currentState == FILE_INPUT) {
                fileDialog.handleEvent(event);
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    if (fileDialog.isButtonClicked(mousePos)) {
                        std::string filename = fileDialog.getFilename();
                        if (std::filesystem::exists(filename)) {
                            auto inputData = get_data_from_file<std::ifstream>(filename);
                            resultLines = processData(selectedMode, inputData);
                        } else {
                            resultLines = "Invalid file name";
                        }
                        currentState = RESULT_VIEW;
                    }
                }
            } else if (currentState == DB_TEST_SELECTION) {
                for (size_t i = 0; i < dbTestButtons.size(); ++i) {
                    if (dbTestButtons[i].first.getGlobalBounds().contains(mousePos)) {
                        auto inputData = get_data_from_db(dbTests[i]);
                        resultLines = processData(selectedMode, inputData);
                        currentState = RESULT_VIEW;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Left) {
                if (currentState == MODE_SELECTION) {
                    for (int i = 0; i < modeButtons.size(); ++i) {
                        if (modeButtons[i].first.getGlobalBounds().contains(mousePos)) {
                            selectedMode = static_cast<Mode>(i);
                            currentState = INPUT_SELECTION;
                        }
                    }
                } else if (currentState == INPUT_SELECTION) {
                    for (int i = 0; i < inputButtons.size(); ++i) {
                        if (inputButtons[i].first.getGlobalBounds().contains(mousePos)) {
                            if (i == 0) {
                                currentState = FILE_INPUT;
                            } else if (i == 1) {
                                currentState = DB_TEST_SELECTION;
                            }
                        }
                    }
                } else if (currentState == RESULT_VIEW) {
                    if (backButton.getGlobalBounds().contains(mousePos)) {
                        currentState = MODE_SELECTION;
                        resultLines.clear();
                    }
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (currentState == FILE_INPUT) {
            fileDialog.highlightButton(mousePos);
        } else if (currentState == MODE_SELECTION) {
            for (auto& btn : modeButtons) {
                btn.first.setFillColor(btn.first.getGlobalBounds().contains(mousePos)
                                           ? sf::Color(100, 100, 100)
                                           : sf::Color(70, 70, 70));
            }
        } else if (currentState == INPUT_SELECTION) {
            for (auto& btn : inputButtons) {
                btn.first.setFillColor(btn.first.getGlobalBounds().contains(mousePos)
                                           ? sf::Color(100, 100, 100)
                                           : sf::Color(70, 70, 70));
            }
        }

        window.clear(sf::Color(50, 50, 50));

        if (currentState == MODE_SELECTION) {
            window.draw(modeTitle);
            for (auto& btn : modeButtons) {
                window.draw(btn.first);
                window.draw(btn.second);
            }
        } else if (currentState == INPUT_SELECTION) {
            window.draw(inputTitle);
            for (auto& btn : inputButtons) {
                window.draw(btn.first);
                window.draw(btn.second);
            }
        } else if (currentState == FILE_INPUT) {
            window.draw(modeTitle);
            fileDialog.draw(window);
        } else if (currentState == DB_TEST_SELECTION) {
            sf::Text dbTestTitle("Select a test from the database:", font, 40);
            dbTestTitle.setPosition(200, 80);
            dbTestTitle.setFillColor(sf::Color::White);
            window.draw(dbTestTitle);
            for (auto& btn : dbTestButtons) {
                window.draw(btn.first);
                window.draw(btn.second);
            }
        }

        if (currentState == RESULT_VIEW) {
            sf::Text resultText("Results:\n" + resultLines, font, 20);
            resultText.setPosition(20.f, 120.f);
            resultText.setFillColor(sf::Color::Green);
            window.draw(resultText);
            window.draw(backButton);
            window.draw(backButtonText);
        }

        window.display();
    }

    return 0;
}
