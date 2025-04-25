#pragma once

#include <SFML/Graphics.hpp>
#include <cstdlib>

enum AppState {
    MODE_SELECTION,
    INPUT_SELECTION,
    FILE_INPUT,
    DB_INPUT,
    RESULT_VIEW,
    DB_TEST_SELECTION
};
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
