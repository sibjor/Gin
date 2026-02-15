#include "editor.hpp"

namespace Gin
{
    Editor::Editor(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, const ProjectInfo &project)
        : window(window), renderer(renderer), gui(nullptr),
          project(project), textInputWasActive(false)
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        gui = new GUI(renderer, font, w, h);
    }

    Editor::~Editor()
    {
        if (gui)
        {
            delete gui;
            gui = nullptr;
        }
    }

    void Editor::handleEvents(bool &quit, bool &backToHub)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                quit = true;
                return;
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

    void Editor::render(bool &backToHub)
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

        if (gui->Button("Back", 10, 5, 80, 40))
        {
            backToHub = true;
        }

        gui->Label(project.projectName.c_str(), W / 2 - 40, 15);

        if (gui->Button("Save", W - 90, 5, 80, 40))
        {
            projectManager.saveProject(project);
            SDL_Log("Project saved: %s", project.projectName.c_str());
        }

        // === Side panel (right) ===
        int panelW = SDL_max(250, W / 4);
        std::vector<std::string> details = {
            "Name: " + project.projectName,
            "Version: " + project.version.toString(),
            "Author: " + (project.authorName.empty() ? "(none)" : project.authorName),
            "Path: " + project.path};

        gui->SidePanel("Project Info", W - panelW, toolbarH, panelW, H - toolbarH,
                       details, "");

        // === Workspace area (center) ===
        int workX = 0;
        int workY = toolbarH;
        int workW = W - panelW;
        int workH = H - toolbarH;

        gui->Rect(workX, workY, workW, workH, {35, 35, 35, 255});
        gui->Label("Editor workspace", workX + workW / 2 - 60, workY + workH / 2 - 10);

        gui->End();
        SDL_RenderPresent(renderer);
    }

    EditorResult Editor::update()
    {
        bool quit = false;
        bool backToHub = false;

        handleEvents(quit, backToHub);
        if (quit)
            return EditorResult::Quit;

        render(backToHub);
        if (backToHub)
            return EditorResult::BackToHub;

        return EditorResult::Continue;
    }

} // namespace Gin
