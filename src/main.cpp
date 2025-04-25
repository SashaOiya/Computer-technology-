#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include "driver.hpp"

enum AppState { MODE_SELECTION, INPUT_SELECTION, FILE_INPUT, DB_INPUT, RESULT_VIEW };
enum Mode { OPTIMIZED_TREE, STD_SET, BOTH };
enum InputMethod { FROM_FILE, FROM_DB };

class FileInputDialog {
   public:
    FileInputDialog(sf::Font& font) : font(font) {
        background.setSize(sf::Vector2f(500, 200));
        background.setPosition(150, 200);
        background.setFillColor(sf::Color(70, 70, 70));
        background.setOutlineThickness(2);
        background.setOutlineColor(sf::Color(120, 120, 120));

        title.setFont(font);
        title.setString("Enter filename (input.txt):");
        title.setCharacterSize(30);
        title.setFillColor(sf::Color::White);
        title.setPosition(170, 210);

        inputText.setFont(font);
        inputText.setCharacterSize(28);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(170, 260);

        button.setSize(sf::Vector2f(100, 40));
        button.setPosition(300, 320);
        button.setFillColor(sf::Color(100, 100, 100));

        buttonText.setFont(font);
        buttonText.setString("OK");
        buttonText.setCharacterSize(25);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(330, 325);
    }

    void handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b') {
                if (!currentInput.empty()) currentInput.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode != '\r') {
                currentInput += static_cast<char>(event.text.unicode);
            }
            inputText.setString(currentInput);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(title);
        window.draw(inputText);
        window.draw(button);
        window.draw(buttonText);
    }

    void clear() {
        currentInput.clear();
        inputText.setString("");
    }

    bool isButtonClicked(const sf::Vector2f& mousePos) const {
        return button.getGlobalBounds().contains(mousePos);
    }

    void highlightButton(const sf::Vector2f& mousePos) {
        button.setFillColor(button.getGlobalBounds().contains(mousePos) ? sf::Color(120, 120, 120)
                                                                        : sf::Color(100, 100, 100));
    }

    std::string getFilename() const { return currentInput.empty() ? "input.txt" : currentInput; }

   private:
    sf::RectangleShape background;
    sf::Text title;
    sf::Text inputText;
    sf::RectangleShape button;
    sf::Text buttonText;
    sf::Font& font;
    std::string currentInput;
};

// Обработка логики работы с данными
auto processData(Mode mode, InputMethod method,
                 std::pair<std::deque<char>, std::deque<int>>& inputData) {
    std::vector<int> result;

    // oss << "Mode: ";
    switch (mode) {
        case OPTIMIZED_TREE:
            result = get_answer<avl_tree::SearchTree<int>>(inputData);
            break;
        case STD_SET:
            result = get_answer<std::set<int>>(inputData);
            break;
        case BOTH:
            // oss << "Both";
            break;
    }

    std::ostringstream oss;
    for (size_t i = 0; i < result.size(); ++i) {
        oss << result[i];
        if (i != result.size() - 1) {
            oss << " ";
        }
    }

    std::cout << "Processed result: " << oss.str() << std::endl;
    std::cout << "Result size: " << result.size() << std::endl;

    return oss.str();
}

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

    std::string manualInput;
    sf::Text manualInputText("", font, 28);
    manualInputText.setPosition(170, 260);
    manualInputText.setFillColor(sf::Color::White);

    sf::RectangleShape manualInputButton(sf::Vector2f(100, 40));
    manualInputButton.setPosition(300, 320);
    manualInputButton.setFillColor(sf::Color(100, 100, 100));
    sf::Text manualInputButtonText("OK", font, 25);
    manualInputButtonText.setPosition(330, 325);
    manualInputButtonText.setFillColor(sf::Color::White);

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

    std::string resultLines;

    //-----------------------------------------------------------------

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentState == FILE_INPUT) {
                fileDialog.handleEvent(event);
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos =
                        window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                    if (fileDialog.isButtonClicked(mousePos)) {
                        std::string filename = fileDialog.getFilename();
                        auto inputData = get_data<std::ifstream>(filename);
                        resultLines = processData(selectedMode, FROM_FILE, inputData);
                        currentState = RESULT_VIEW;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos =
                    window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
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
                            } else if (i == 2) {  // data base
                                std::deque<int> dbData;
                                for (int j = 0; j < 10; ++j) dbData.push_back(rand() % 100);
                                // resultLines = processData(selectedMode, FROM_DB, dbData);
                                currentState = RESULT_VIEW;
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

        //-----------------------------------------
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
        }
        float y = 20.f;
        sf::Text text(resultLines, font, 20);
        text.setPosition(20.f, 20.f);
        text.setFillColor(sf::Color::Green);
        window.draw(text);

        if (currentState == RESULT_VIEW) {
            sf::Text resultText(resultLines, font, 20);
            resultText.setPosition(20.f, 20.f);
            resultText.setFillColor(sf::Color::Green);
            window.draw(resultText);
            window.draw(backButton);
            window.draw(backButtonText);
        }

        window.display();
    }

    return 0;
}
