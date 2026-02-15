#include "app.hpp"

namespace Gin
{
    bool initSubsystems()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
        {
            SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
            return false;
        }

        if (!TTF_Init())
        {
            SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
            return false;
        }

        if (!MIX_Init())
        {
            SDL_Log("Failed to initialize SDL_mixer: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    bool createWindow(const char *title, int width, int height,
                      SDL_Window **window, SDL_Renderer **renderer)
    {
        *window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
        if (!*window)
        {
            SDL_Log("Failed to create window: %s", SDL_GetError());
            return false;
        }

        *renderer = SDL_CreateRenderer(*window, nullptr);
        if (!*renderer)
        {
            SDL_Log("Failed to create renderer: %s", SDL_GetError());
            return false;
        }

        SDL_SetRenderVSync(*renderer, 1);
        return true;
    }

    TTF_Font *loadDefaultFont()
    {
        TTF_Font *font = TTF_OpenFont(
            "assets/fonts/Montserrat/static/Montserrat-Regular.ttf", 16);
        if (!font)
        {
            SDL_Log("Failed to load font: %s", SDL_GetError());
        }
        return font;
    }

    void shutdownAll(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font)
    {
        if (font)
            TTF_CloseFont(font);
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);

        MIX_Quit();
        TTF_Quit();
        SDL_Quit();
    }

} // namespace Gin