#include "app.hpp"

namespace Gin
{
    App::App() : window(nullptr), renderer(nullptr), isRunning(false)
    {
    }

    App::~App()
    {
        cleanup();
    }

    bool App::loadFonts()
    {
        defaultFont = TTF_OpenFont("assets/fonts/Montserrat/static/Montserrat-Regular.ttf", 16);
        if (!defaultFont)
        {
            SDL_Log("Failed to load font: %s", SDL_GetError());
            return false;
        }
        return true;
    }
    
    bool App::initialize(const char *title, int width, int height)
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

        window = SDL_CreateWindow(title, width, height, 0);
        if (!window)
        {
            SDL_Log("Failed to create window: %s", SDL_GetError());
            return false;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer)
        {
            SDL_Log("Failed to create renderer: %s", SDL_GetError());
            return false;
        }

        if(!loadFonts())
        {
            SDL_Log("Failed to load fonts: %s", SDL_GetError());
        }

        isRunning = true;
        return true;
    }

    void App::run()
    {
        while (isRunning)
        {
            handleEvents();
            update();
            render();
        }
    }

    void App::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
                break;
            }
        }
    }

    void App::update()
    {
        
    }

    void App::render()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw objects here
        
        SDL_RenderPresent(renderer);
    }

    void App::cleanup()
    {
        if (renderer)
        {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        MIX_Quit();
        TTF_Quit();
        SDL_Quit();
    }
} // namespace Gin