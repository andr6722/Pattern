#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <chrono>

class Graphic {
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void move(int x, int y) = 0;
    virtual ~Graphic() {}
};

class RealImage : public Graphic {
    std::string filename;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    RealImage(const std::string& filename) : filename(filename) {
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Error loading " << filename << std::endl;
        }
        sprite.setTexture(texture);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    void move(int x, int y) override {
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
    }

    sf::Vector2u getSize() const {
        return texture.getSize();
    }
};

class ProxyImage : public Graphic {
    std::string filename;
    RealImage* realImage = nullptr;
    sf::RectangleShape placeholder;
    int x, y;

    // Для обработки двойного клика
    std::chrono::steady_clock::time_point lastClickTime;
    bool isDragging = false; // Для обработки перетаскивания

public:
    ProxyImage(const std::string& filename) : filename(filename), x(0), y(0) {
        // Предварительная загрузка для определения размера
        sf::Texture tempTexture;
        if (tempTexture.loadFromFile(filename)) {
            sf::Vector2u size = tempTexture.getSize();
            placeholder.setSize(sf::Vector2f(size.x, size.y));
        }
        placeholder.setFillColor(sf::Color::Transparent);
        placeholder.setOutlineColor(sf::Color::Green);
        placeholder.setOutlineThickness(1.0f);
    }

    ~ProxyImage() {
        delete realImage;
    }

    void draw(sf::RenderWindow& window) override {
        if (!realImage) {
            window.draw(placeholder);
        }
        else {
            realImage->draw(window);
        }
    }

    void move(int x, int y) override {
        this->x = x;
        this->y = y;
        placeholder.setPosition(static_cast<float>(x), static_cast<float>(y));
        if (realImage) {
            realImage->move(x, y);
        }
    }

    void loadImage() {
        if (!realImage) {
            realImage = new RealImage(filename);
            realImage->move(x, y);
        }
    }

    void handleClick(const sf::Mouse::Button button, bool isPressed, int mouseX, int mouseY) {
        if (button == sf::Mouse::Right && isPressed) {
            auto now = std::chrono::steady_clock::now();
            if (lastClickTime.time_since_epoch().count() != 0 &&
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count() < 500) {
                // Двойной клик
                loadImage();
            }
            lastClickTime = now;
        }
        else if (button == sf::Mouse::Left) {
            isDragging = isPressed;
            if (isPressed) { 
                move(mouseX - placeholder.getSize().x / 2, mouseY - placeholder.getSize().y / 2);
            }
        }
    }

    bool isDragged() const {
        return isDragging;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Proxy Pattern with SFML");
    ProxyImage image("C:\\Users\\andre\\OneDrive\\Изображения\\TestImage.jpg"); 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
                image.handleClick(event.mouseButton.button, event.type == sf::Event::MouseButtonPressed,
                    event.mouseButton.x, event.mouseButton.y);
            }

            if (event.type == sf::Event::MouseMoved && image.isDragged()) {
                // Обновление позиции изображения при перемещении
                image.move(event.mouseMove.x, event.mouseMove.y);
            }
        }

        window.clear();
        image.draw(window);
        window.display();
    }

    return 0;
}
