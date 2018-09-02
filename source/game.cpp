#include "game.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Text.hpp>
#include <algorithm>
#include <cmath>

extern const char *iconData;

Game::Game() {
    mWindow.create({ 512, 288 }, L"A-Maze Batz – a GMTK Game Jam 2018 entry", sf::Style::Close | sf::Style::Titlebar);
    mWindow.setVerticalSyncEnabled(true);
    mWindow.setIcon(32, 32, reinterpret_cast<const sf::Uint8*>(iconData));

    mPlayer.setTexture(get<sf::Texture>("assets/textures/bat0.png"));
    mPlayer.setTextureRect({ 0, 0, 32, 16 });
    mPlayer.setOrigin(16, 8);

    mGoal.setTexture(get<sf::Texture>("assets/textures/bat1.png"));
    mGoal.setTextureRect({ 0, 0, 32, 16 });
    mGoal.setOrigin(16, 8);

    mSonar.reserve(5000);
}

Game::~Game() {
}

int Game::run() {
    sf::RenderTexture mScreen;
    mScreen.create(256, 144);
    sf::Sprite mShow(mScreen.getTexture());

    sf::View view({ 128, 72 }, { 256, 144 });
    sf::View sview({ 128, 72 }, { 256, 144 });
    mWindow.setView(sview);

    sf::Clock timer;
    sf::Time elapsed;
    const sf::Time frameTime(sf::seconds(1) / 60.f);
    long tick = 0;

    unsigned char movement = None;
    Direction dir = Right;
    bool lastLeft = false;

    if (!resetLevel())
        throw  Exception("Couldn't load the first level!");

    sf::Sound ping0(get<sf::SoundBuffer>("assets/sounds/ping0.wav"));
    sf::Sound ping0e(get<sf::SoundBuffer>("assets/sounds/ping0.wav"));
    ping0e.setVolume(25);
    sf::Sound ping1(get<sf::SoundBuffer>("assets/sounds/ping1.wav"));
    sf::Sound ping1e(get<sf::SoundBuffer>("assets/sounds/ping1.wav"));
    ping1e.setVolume(25);
    sf::Sound drop(get<sf::SoundBuffer>("assets/sounds/drop.wav"));
    sf::Sound drope(get<sf::SoundBuffer>("assets/sounds/drop.wav"));
    drope.setVolume(25);

    sf::RectangleShape white({ 256,144 });
    white.setFillColor(sf::Color::White);

    sf::Font &font = get<sf::Font>("assets/fonts/CompassPro.ttf");
    const_cast<sf::Texture&>(font.getTexture(16)).setSmooth(false);

    sf::Text intro("Can you help Batz to find love?\n\nUse the cursor keys or [WASD],\n and [Space] to find your way!\n\n  [Space] Start game\n  [Alt] + [Return] Fullscreen", font, 16);
    intro.setPosition(32, 8);
    intro.setOutlineThickness(1);
    intro.setOutlineColor(sf::Color::Black);
    intro.setFillColor(sf::Color::White);

    sf::Text outro("Unfortunately, this was the last level.\n\n  Hit [Space] to restart the game\n  or use [Esc] to quit the game.", font, 16);
    outro.setPosition(8, 8);
    outro.setOutlineThickness(1);
    outro.setOutlineColor(sf::Color::Black);
    outro.setFillColor(sf::Color::White);

    sf::Text caption("Level 1", font, 16);
    caption.setPosition(8, 8);
    caption.setOutlineThickness(1);
    caption.setOutlineColor(sf::Color::Black);
    caption.setFillColor(sf::Color::White);

    float fading = 0;

    while (mWindow.isOpen()) {
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                mWindow.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Return:
                    if (event.key.alt) {
                        mFullscreen = !mFullscreen;
                        if (mFullscreen) {
                            sf::VideoMode vm(sf::VideoMode::getDesktopMode());
                            mWindow.create(vm, "A-Maze Batz – a GMTK Game Jam 2018 entry", sf::Style::Close | sf::Style::Fullscreen);
                            mWindow.setMouseCursorVisible(false);

                            const float ar = static_cast<float>(vm.width) / static_cast<float>(vm.height);
                            const float tar = 16.f / 9.f;

                            if (ar < tar)
                                sview.setViewport({ 0, .5f - ar / tar / 2.f, 1, ar / tar });
                            else
                                sview.setViewport({ .5f - tar / ar / 2.f, 0, tar / ar, 1 });
                        }
                        else {
                            mWindow.create({ 512, 288 }, "A-Maze Batz – a GMTK Game Jam 2018 entry", sf::Style::Close | sf::Style::Titlebar);
                            mWindow.setMouseCursorVisible(true);
                            sview.setViewport({ 0, 0, 1, 1 });
                        }
                    }
                    break;
                case sf::Keyboard::Space:
                    if (fading < 1) {

                    }
                    if (mIntro) {
                        mIntro = false;
                        mLevelTimer.restart();
                        break;
                    }
                    else if (mEnd) {
                        mIntro = true;
                        resetLevel();
                        caption.setString("Level " + std::to_string(mLevelNumber));
                        break;
                    }
                    if (mSonarTimer.getElapsedTime() < sf::seconds(.5f))
                        break;
                    mSonarTimer.restart();
                    ping(mPlayer.getPosition(), 0, 2 * PI, 32, sf::Color::White);
                    ping0.play();
                    break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    movement |= Left;
                    dir = Left;
                    lastLeft = true;
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    movement |= Right;
                    dir = Right;
                    lastLeft = false;
                    break;
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    movement |= Up;
                    dir = Up;
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    movement |= Down;
                    dir = Down;
                    break;
                case sf::Keyboard::Escape:
                    mWindow.close();
                    break;
                }
                break;
            case sf::Event::KeyReleased:
                switch (event.key.code) {
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    movement &= ~Left;
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    movement &= ~Right;
                    break;
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    movement &= ~Up;
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    movement &= ~Down;
                    break;
                }
                break;
            }
        }

        if (mIntro || mEnd)
            fading = 0;
        else if (mWon)
            fading = std::max(0.f, 1.f - mLevelTimer.getElapsedTime() / sf::seconds(1));
        else
            fading = mLevelTimer.getElapsedTime() / sf::seconds(1);

        if (mWon && !mEnd && mLevelTimer.getElapsedTime() > sf::seconds(1)) {
            mEnd = !resetLevel(mLevelNumber + 1);
            caption.setString("Level " + std::to_string(mLevelNumber));
            if (!mEnd) {
                mLevelTimer.restart();
            }
        }


        elapsed += timer.restart();

        long ticks = 0;
        while (elapsed > frameTime) {
            elapsed -= frameTime;
            ++tick;

            if (++ticks == 10 || mIntro || mEnd || mWon) { // max 10 updates between draws
                elapsed = sf::Time::Zero;
                break;
            }
            const sf::Vector2f pos(mPlayer.getPosition());

            if (!mWon && !mIntro) {
                if (movement & Left && !solid(pos - sf::Vector2f(5, 0))) {
                    mPlayer.move(-1, 0);
                }
                if (movement & Right && !solid(pos + sf::Vector2f(5, 0))) {
                    mPlayer.move(1, 0);
                }
                if (movement & Up && !solid(pos - sf::Vector2f(0, 5))) {
                    mPlayer.move(0, -1);
                }
                if (movement & Down && !solid(pos + sf::Vector2f(0, 5))) {
                    mPlayer.move(0, 1);
                }
            }

            if (tick % 50 == 0) {
                ping(*(mDroppers.begin() + (rand() % mDroppers.size())), .5f * PI, .5f * PI, 1, sf::Color::Cyan, .5f);
            }

            unsigned char ga = mGoal.getColor().a;
            const sf::FloatRect gr = mGoal.getGlobalBounds();
            for (int i = 0; i < 2; ++i) {
                for (auto &ping : mSonar) {
                    if (ping.texCoords.x != 0 || ping.texCoords.y != 0) {
                        if (solid(ping.position + ping.texCoords)) {
                            if (ping.color.a > 64) {
                                if (ping.color.r == 0 && ping.color.g == 255 && ping.color.b == 255) // Water drops
                                    drop.play();
                            }

                            ping.color.a /= 2;
                            if (ping.color.a > 64) {
                                this->ping(ping.position, 0, 2 * PI, 32, ping.color);
                                if (ping.color.r == 255 && ping.color.g == 255 && ping.color.b == 255)
                                    ping0e.play();
                                else if (ping.color.r == 0 && ping.color.g == 255 && ping.color.b == 255)
                                    drope.play();
                                else
                                    ping1e.play();
                            }
                            ping.position += ping.texCoords;
                            ping.texCoords = { 0, 0 };
                        }
                        else
                            ping.position += ping.texCoords;
                    }
                    ping.color.a -= 1;
                    if (gr.contains(ping.position) && ping.color.r == 255 && ping.color.g == 255 && ping.color.b == 255) {
                        ga = static_cast<unsigned char>(std::min<unsigned int>(255, ga + ping.color.a / 64));
                        if (mGoalSonarTimer.getElapsedTime() > sf::seconds(.5f) && ping.color.a > 128) {
                            mGoalSonarTimer.restart();
                            this->ping(mGoal.getPosition(), 0, 2 * PI, 32, { 255, 192, 192, 127 }, .5f);
                            ping1.play();
                        }
                    }
                }
                mSonar.erase(std::remove_if(mSonar.begin(), mSonar.end(), [](const sf::Vertex &ping) { return ping.color.a == 0; }), mSonar.end());
            }

            if (mWon) {

            }
            else {

                if (ga > 4)
                    ga -= 4;
                else
                    ga = 0;

                if (ga > 192 && gr.intersects(mPlayer.getGlobalBounds())) {
                    // Win!
                    mWon = true;
                    mGoal.setColor(sf::Color::White);
                    mLevelTimer.restart();
                }

                mGoal.setColor({ 255, 255, 255, ga });
            }
        }

        mPlayer.setTextureRect({ movement & Up ? 64 : (lastLeft ? 0 : 32), 16 * std::abs(-2 + ((tick >> 3) % 6)), 32, 16 });
        mGoal.setTextureRect({ 0, 16 * std::abs(-2 + (((tick >> 3) + 4) % 6)), 32, 16 });
        view.setCenter(mPlayer.getPosition());

        mScreen.setView(view);
        mScreen.clear();

        mScreen.draw(mGoal);
        mScreen.draw(mSonar.data(), mSonar.size(), sf::Points);
        mScreen.draw(mPlayer);

        view.setCenter(view.getSize() / 2.f);
        mScreen.setView(view);

        if (!mWon) {
            if (fading < 2) {
                const sf::Uint8 a = static_cast<sf::Uint8>(255 * std::max(0.f, 2.f - fading) / 2.f);
                caption.setFillColor({ 255, 255, 255, a });
                caption.setOutlineColor({ 0, 0, 0, a });
                mScreen.draw(caption);
            }
        }

        if (fading < 1) {
            white.setFillColor({ 255, 255, 255, static_cast<sf::Uint8>(255 * (1.f - fading)) });
            mScreen.draw(white);
        }

        if (mIntro)
            mScreen.draw(intro);
        else if (mEnd)
            mScreen.draw(outro);

        mScreen.display();

        mWindow.setView(view);
        mWindow.clear();
        mWindow.setView(sview);
        mWindow.draw(mShow);
        mWindow.display();
    }
    return 0;
}

void Game::ping(const sf::Vector2f &pos, const float from, const float to, const unsigned char count, const sf::Color &color, const float speed) {
    /*switch (dir) {
    case None:
        from = 0;
        to = 2 * PI;
        break;
    case Up:
        from = PI;
        to = 2 * PI;
        break;
    case Down:
        from = 0;
        to = PI;
        break;
    case Left:
        from = .5f * PI;
        to = 1.5f * PI;
        break;
    case Right:
        from = -.5f * PI;
        to = .5f * PI;
        break;
    }*/

    //for (float a = from; a <= to; a += 2.f * PI / count) {
    for (unsigned int i = 0; i < count; ++i) {
        const float a = from + i * (to - from) / count;
        mSonar.push_back({ pos, color, {speed * cos(a), speed * sin(a)} });
    }
}

bool Game::solid(const sf::Vector2f &pos) const {
    if (pos.x < 0 || pos.x >= mWidth || pos.y < 0 || pos.y >= mHeight)
        return true;
    return mLevel.getPixel(static_cast<unsigned int>(pos.x), static_cast<unsigned int>(pos.y)) == sf::Color::Black;
}

bool Game::resetLevel(unsigned int newLevel) {
    const std::string file(std::string("assets/levels/level") + std::to_string(newLevel) + ".png");
    if (!mLevel.loadFromFile(file)) {
        mEnd = true;
        return false;
    }

    mLevelNumber = newLevel;
    mWon = false;
    mEnd = false;
    const sf::Vector2i size(mLevel.getSize());
    mWidth = size.x;
    mHeight = size.y;

    std::vector<sf::Vector2f> starts;
    std::vector<sf::Vector2f> goals;
    for (std::size_t y = 0; y < mHeight; ++y)
        for (std::size_t x = 0; x < mWidth; ++x) {
            sf::Color pixel = mLevel.getPixel(x, y);
            if (pixel == sf::Color(255, 0, 0, 255)) { // goal
                goals.push_back({ static_cast<float>(x), static_cast<float>(y) });
            }
            else if (pixel == sf::Color(0, 255, 0, 255)) { // start
                starts.push_back({ static_cast<float>(x), static_cast<float>(y) });
            }
            else if (pixel == sf::Color(0, 0, 255, 255)) { // water dripping
                mDroppers.push_back({ static_cast<float>(x), static_cast<float>(y) });
            }
        }

    if (starts.empty())
        throw Exception("The level doesn't contain valid start pixels");
    if (goals.empty())
        throw Exception("The level doesn't contain valid goal pixels");

    mGoal.setPosition(*(goals.begin() + (rand() % goals.size())));
    mPlayer.setPosition(*(starts.begin() + (rand() % starts.size())));

    mLevelTimer.restart();
    mSonarTimer.restart();
    mSonar.clear();
    return true;
}
