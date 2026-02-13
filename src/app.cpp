#include "app.hpp"
#include "gui.hpp"

namespace Gin
{
    App::App() : window(nullptr), renderer(nullptr), isRunning(false),
                 defaultFont(nullptr), gui(nullptr), volume(0.5f), selectedOption(0)
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

        window = SDL_CreateWindow(
            title,
            width, height,
            SDL_WINDOW_RESIZABLE
        );

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

        // Set logical presentation - SDL Magic!
        if (!SDL_SetRenderLogicalPresentation(
                renderer,
                LOGICAL_WIDTH,
                LOGICAL_HEIGHT,
                SDL_LOGICAL_PRESENTATION_LETTERBOX
                ))
        {
            SDL_Log("Failed to set logical presentation: %s", SDL_GetError());
            return false;
        }

        // Enable VSync for smooth rendering
        SDL_SetRenderVSync(renderer, 1);

        if (!loadFonts())
        {
            SDL_Log("Failed to load fonts");
            return false;
        }

        gui = new GUI(renderer, defaultFont);
        isRunning = true;

        SDL_Log("Initialized with logical resolution: %dx%d", LOGICAL_WIDTH, LOGICAL_HEIGHT);

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
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        
        // Get mouse state in window coordinates
        float windowMouseX, windowMouseY;
        Uint8 mouseState = SDL_GetMouseState(&windowMouseX, &windowMouseY);
        bool mouseDown = (mouseState & SDL_BUTTON_LMASK) != 0;
        
        // Convert to logical coordinates
        float logicalMouseX, logicalMouseY;
        SDL_RenderCoordinatesFromWindow(renderer, windowMouseX, windowMouseY, &logicalMouseX, &logicalMouseY);
        
        gui->Begin((int)logicalMouseX, (int)logicalMouseY, mouseDown);
        
        gui->Label("Gin Engine", 50, 50);
        
        if (gui->Button("Click Me!", 50, 100, 200, 50))
        {
            SDL_Log("Button clicked!");
        }
        
        if (gui->Slider("Volume", 50, 180, 300, &volume, 0.0f, 1.0f))
        {
            SDL_Log("Volume changed: %.2f", volume);
        }
        
        std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
        if (gui->Dropdown("Quality", 50, 280, 300, &selectedOption, options))
        {
            SDL_Log("Quality: %s", options[selectedOption].c_str());
        }
        
        if (gui->Button("Exit", LOGICAL_WIDTH - 170, LOGICAL_HEIGHT - 70, 150, 50))
        {
            isRunning = false;
        }
        
        gui->End();
        
        SDL_RenderPresent(renderer);
    }

    void App::cleanup()
    {
        if (gui)
        {
            delete gui;
            gui = nullptr;
        }

        if (defaultFont)
        {
            TTF_CloseFont(defaultFont);
            defaultFont = nullptr;
        }
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