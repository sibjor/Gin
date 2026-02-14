#include "app.hpp"
#include "gui.hpp"

namespace Gin
{
    App::App() : window(nullptr), renderer(nullptr), isRunning(false),
                 defaultFont(nullptr), gui(nullptr),
                 selectedProjectIndex(-1),
                 activePopup(PopupType::None),
                 textInputWasActive(false)
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

        refreshProjects();

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
            case SDL_EVENT_TEXT_INPUT:
                if (gui)
                    gui->HandleTextEvent(event.text.text);
                break;
            case SDL_EVENT_KEY_DOWN:
                if (gui)
                    gui->HandleKeyEvent(event.key.key);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (gui)
                    gui->HandleScrollEvent(event.wheel.y);
                break;
            }
        }

        if (gui)
        {
            bool hasFocus = gui->HasTextFocus();
            if (hasFocus && !textInputWasActive)
            {
                SDL_StartTextInput(window);
                textInputWasActive = true;
            }
            else if (!hasFocus && textInputWasActive)
            {
                SDL_StopTextInput(window);
                textInputWasActive = false;
            }
        }
    }

    void App::update()
    {
    }

    void App::refreshProjects()
    {
        projects = fs.listProjects();
        if (selectedProjectIndex >= (int)projects.size())
            selectedProjectIndex = -1;
    }

    void App::render()
    {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int windowW, windowH;
        SDL_GetWindowSize(window, &windowW, &windowH);
        gui->SetSize(windowW, windowH);

        float mouseXf, mouseYf;
        SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseXf, &mouseYf);
        bool mouseDown = (mouseButtons & SDL_BUTTON_LMASK) != 0;

        gui->Begin((int)mouseXf, (int)mouseYf, mouseDown);

        int W = gui->GetLogicalWidth();
        int H = gui->GetLogicalHeight();

        // === Toolbar ===
        int toolbarH = 50;
        gui->Rect(0, 0, W, toolbarH, {45, 45, 45, 255});
        gui->Label("Gin", W / 2 - 10, 15);

        if (gui->Button("New Project", 10, 5, 140, 40))
        {
            activePopup = PopupType::TextInput;
            popupInputText.clear();
        }

        // === Side panel (right, only when project selected) ===
        int panelW = 0;
        if (selectedProjectIndex >= 0 && selectedProjectIndex < (int)projects.size())
        {
            panelW = SDL_max(250, W / 4);
            const ProjectInfo &proj = projects[selectedProjectIndex];

            std::vector<std::string> details = {
                proj.name,
                proj.lastModified,
                proj.path
            };

            if (gui->SidePanel("Project Details", W - panelW, toolbarH, panelW, H - toolbarH,
                               details, "Open"))
            {
                SDL_Log("Open project: %s", proj.name.c_str());
            }
        }

        // === Project list (scrollable) ===
        int listX = 0;
        int listY = toolbarH;
        int listW = W - panelW;
        int listH = H - toolbarH;

        gui->Rect(listX, listY, listW, listH, {35, 35, 35, 255});

        const int rowH = 60;
        const int rowPad = 2;
        float contentHeight = (float)(projects.size() * (rowH + rowPad) + rowPad);

        int scrollY = gui->BeginScroll(listX, listY, listW, listH, contentHeight);

        for (int i = 0; i < (int)projects.size(); i++)
        {
            int ry = listY + rowPad + i * (rowH + rowPad) + scrollY;

            // Skip rows outside visible region
            if (ry + rowH < listY || ry > listY + listH)
                continue;

            if (gui->ProjectRow(projects[i].name.c_str(),
                                projects[i].lastModified.c_str(),
                                listX + rowPad, ry,
                                listW - rowPad * 2 - 8, rowH,
                                i == selectedProjectIndex))
            {
                selectedProjectIndex = i;
            }
        }

        if (projects.empty())
        {
            gui->Label("No projects yet. Click 'New Project' to create one.",
                       listX + 20, listY + 30 + scrollY);
        }

        gui->EndScroll();

        // === Popup ===
        if (activePopup == PopupType::TextInput)
        {
            bool open = true;
            if (gui->PopupTextInput("New Project", "Project Name", popupInputText, &open))
            {
                if (fs.createProject(popupInputText))
                {
                    SDL_Log("Created project: %s", popupInputText.c_str());
                    refreshProjects();
                }
            }
            if (!open)
                activePopup = PopupType::None;
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