#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <string>
#include <map>
#include <exception>

constexpr float PI = 3.1415f;

enum Direction {
    None = 0,
    Left = 1,
    Right = 2,
    Up = 4,
    Down = 8
};

class Exception : public std::exception {
public:
    Exception(const std::string &what) : mWhat(what) { }
    const char *what() const noexcept { return mWhat.c_str(); }
private:
    std::string mWhat;
};

class Game {
public:
    Game();
    ~Game();
    int run();
private:
    sf::RenderWindow mWindow;
    std::vector<sf::Vertex> mSonar;
    std::vector<sf::Vector2f> mDroppers;
    sf::Sprite mPlayer;
    sf::Sprite mGoal;
    sf::Clock mSonarTimer;
    sf::Clock mGoalSonarTimer;
    sf::Image mLevel;
    std::size_t mLevelNumber = 0;
    std::size_t mWidth;
    std::size_t mHeight;
    bool mWon = false;
    bool mIntro = true;
    bool mEnd = false;
    bool mFullscreen = false;
    sf::Clock mLevelTimer;

    template<class T> T& get(const std::string &file) const {
        static std::map<const std::string, std::unique_ptr<T>> mCache;
        auto &entry = mCache[file];
        if (!entry) {
            entry.reset(new T());
            if (!entry->loadFromFile(file))
                throw Exception("Failed to load file '" + file + "'!");
        }
        return *entry;
    }

    void ping(const sf::Vector2f &pos, const float from, const float to, const unsigned char count, const sf::Color &color, const float speed = 1.f);
    bool solid(const sf::Vector2f &pos) const;
    bool resetLevel(unsigned int newLevel = 1);
};
