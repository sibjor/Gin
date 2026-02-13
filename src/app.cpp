#include "app.hpp"
#include <iostream>

bool Gin::App::initialize(const char* title, int width, int height)
{
    if(!SDL_Init(NULL)){
        std::cerr << "SDL was not initialized... " << SDL_GetError() << std::endl;
    }
    std::cout << "SDL initialized! " << std::endl;
    if(!TTF_Init())
    {
        std::cerr << "SDL_ttf was not initialized... " << SDL_GetError() << std::endl;
    }
    std::cout << "SDL_ttf initialized! " << std::endl; 
    if(!MIX_Init())
    {
        std::cerr << "SDL_mixer was not initialized... " << SDL_GetError() << std::endl;
    }
    std::cout << "SDL_mixer initialized! " << std::endl;

};