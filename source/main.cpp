#include "game.hpp"
#include <exception>
#ifdef WIN32
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#else
#include <iostream>
#endif

int main(int argc, char **argv) {
#ifdef NDEBUG
    try {
#endif
        Game game;
        return game.run();
#ifdef NDEBUG
    }
    catch (const Exception &exception) {
#ifdef WIN32
        MessageBoxA(0, exception.what(), "An unhandled exception appeared in A-Maze Batz!", MB_ICONSTOP | MB_OK);
#else
        std::cerr << "An unhandled exception appeared in A-Maze Batz!\n\nException: " << exception.what() << std::endl;
#endif
        return 1;
    }
#endif
}
