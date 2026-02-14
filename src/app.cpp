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
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
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

        // No SDL_SetRenderLogicalPresentation - editor UI adapts to window size

        // Enable VSync for smooth rendering
        SDL_SetRenderVSync(renderer, 1);

        if (!loadFonts())
        {
            SDL_Log("Failed to load fonts");
            return false;
        }

        // Get actual window size
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        gui = new GUI(renderer, defaultFont, w, h);
        isRunning = true;

        SDL_Log("Initialized with window size: %dx%d", w, h);

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

        // Update GUI size on every frame (handles window resize)
        int windowW, windowH;
        SDL_GetWindowSize(window, &windowW, &windowH);
        gui->SetSize(windowW, windowH);

        // Mouse coordinates - direct, no conversion needed
        float mouseXf, mouseYf;
        SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseXf, &mouseYf);
        bool mouseDown = (mouseButtons & SDL_BUTTON_LMASK) != 0;

        gui->Begin((int)mouseXf, (int)mouseYf, mouseDown);

        // === Editor-style layout ===
        int W = gui->GetLogicalWidth();
        int H = gui->GetLogicalHeight();

        // Toolbar at top (fixed height)
        int toolbarH = 50;
        gui->Rect(0, 0, W, toolbarH, {45, 45, 45, 255});

        if (gui->Button("New Project", 10, 5, 140, 40))
        {
            SDL_Log("New Project clicked!");
        }

        if (gui->Button("Open", 160, 5, 100, 40))
        {
            SDL_Log("Open clicked!");
        }

        if (gui->Button("Save", 270, 5, 100, 40))
        {
            SDL_Log("Save clicked!");
        }

        // Left side panel (proportional width)
        int panelW = SDL_max(200, W / 5);  // At least 200px, otherwise 20% of window
        int panelY = toolbarH;
        int panelH = H - toolbarH;
        gui->Rect(0, panelY, panelW, panelH, {40, 40, 40, 255});

        gui->Label("Inspector", 10, panelY + 10);

        if (gui->Slider("Volume", 10, panelY + 50, panelW - 20, &volume, 0.0f, 1.0f))
        {
            SDL_Log("Volume: %.2f", volume);
        }

        std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
        if (gui->Dropdown("Quality", 10, panelY + 130, panelW - 20, &selectedOption, options))
        {
            SDL_Log("Quality: %s", options[selectedOption].c_str());
        }

        // Viewport (rest of the area)
        int vpX = panelW;
        int vpY = toolbarH;
        int vpW = W - panelW;
        int vpH = H - toolbarH;
        gui->Rect(vpX, vpY, vpW, vpH, {35, 35, 35, 255});
        gui->Label("Viewport", vpX + vpW / 2 - 30, vpY + vpH / 2);

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