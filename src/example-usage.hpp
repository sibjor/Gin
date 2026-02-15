#pragma once

#include "app.hpp"
#include "gui.hpp"
#include "project.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Gin
{
    // =========================================================================
    // Tutorial: How to use the Gin GUI system
    // This file is NOT compiled into the app — it's a developer reference.
    //
    // This example follows the same patterns used in IntroHub and Editor:
    //   - Constructor takes (SDL_Window*, SDL_Renderer*, TTF_Font*)
    //   - Owns a heap‐allocated GUI* internally
    //   - Has update() returning a result enum
    //   - Private handleEvents() + render() split
    //   - Responsive layout via gui->SetSize()
    //   - SDL_StartTextInput / SDL_StopTextInput toggling
    //   - Popup state machine (activePopup enum)
    // =========================================================================

    enum class ExampleResult
    {
        Continue,
        Quit
    };

    enum class ExamplePopup
    {
        None,
        TextInput,
        Confirm,
        Info
    };

    class ExampleGUIUsage
    {
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        GUI *gui;

        // Widget state
        float volume = 0.5f;
        int selectedOption = 0;
        std::string playerName;
        int selectedRow = -1;

        // Popup state machine
        ExamplePopup activePopup = ExamplePopup::None;
        std::string popupInputText;

        // Text input activation tracking
        bool textInputWasActive = false;

    public:
        ExampleGUIUsage(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font)
            : window(window), renderer(renderer), gui(nullptr)
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            gui = new GUI(renderer, font, w, h);
        }

        ~ExampleGUIUsage()
        {
            if (gui)
            {
                delete gui;
                gui = nullptr;
            }
        }

        ExampleResult update()
        {
            bool quit = false;
            handleEvents(quit);
            if (quit)
                return ExampleResult::Quit;
            render();
            return ExampleResult::Continue;
        }

    private:
        void handleEvents(bool &quit)
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
                    gui->HandleTextEvent(event.text.text);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    gui->HandleKeyEvent(event.key.key);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    gui->HandleScrollEvent(event.wheel.y);
                    break;
                }
            }

            // Toggle SDL text input based on GUI focus
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

        void render()
        {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);

            // Responsive sizing — call every frame
            int windowW, windowH;
            SDL_GetWindowSize(window, &windowW, &windowH);
            gui->SetSize(windowW, windowH);

            float mouseXf, mouseYf;
            SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseXf, &mouseYf);
            bool mouseDown = (mouseButtons & SDL_BUTTON_LMASK) != 0;

            gui->Begin((int)mouseXf, (int)mouseYf, mouseDown);

            int W = gui->GetLogicalWidth();
            int H = gui->GetLogicalHeight();

            // ----- Rect — filled or outlined rectangle -----
            gui->Rect(0, 0, W, 50, {45, 45, 45, 255});           // filled (default)
            gui->Rect(10, 60, 200, 100, {255, 0, 0, 255}, false); // outline only

            // ----- Label — simple white text -----
            gui->Label("Hello, Gin!", 20, 70);

            // ----- Button — returns true on click -----
            if (gui->Button("Click Me", 10, 5, 140, 40))
                SDL_Log("Button clicked!");

            // ----- Slider — returns true when value changes -----
            if (gui->Slider("Volume", 10, 180, 200, &volume, 0.0f, 1.0f))
                SDL_Log("Volume: %.2f", volume);

            // ----- Dropdown — returns true when selection changes -----
            std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
            if (gui->Dropdown("Quality", 10, 250, 200, &selectedOption, options))
                SDL_Log("Quality: %s", options[selectedOption].c_str());

            // ----- TextInput — returns true when Enter is pressed -----
            if (gui->TextInput("Player Name", 10, 330, 200, playerName))
                SDL_Log("Name: %s", playerName.c_str());

            // ----- ProjectRow — clickable row with name and date -----
            if (gui->ProjectRow("My Game", "2026-02-14", 10, 400, 300, 60, selectedRow == 0))
                selectedRow = 0;

            // ----- SidePanel — panel with title, detail lines, action button -----
            std::vector<std::string> details = {"Name: My Game", "Path: /projects/my-game"};
            if (gui->SidePanel("Details", W - 260, 50, 250, 400, details, "Open"))
                SDL_Log("Open clicked!");

            // ----- Scrollable Region -----
            // int scrollY = gui->BeginScroll(x, y, w, h, contentHeight);
            // for (int i = 0; i < count; i++)
            // {
            //     int iy = y + i * 60 + scrollY;
            //     gui->ProjectRow(names[i], dates[i], x, iy, w, 58, i == selected);
            // }
            // gui->EndScroll();

            // ----- Popup trigger buttons -----
            if (gui->Button("Show TextInput Popup", 10, 480, 200, 35))
            {
                activePopup = ExamplePopup::TextInput;
                popupInputText.clear();
            }

            if (gui->Button("Show Confirm Popup", 10, 525, 200, 35))
                activePopup = ExamplePopup::Confirm;

            if (gui->Button("Show Info Popup", 10, 570, 200, 35))
                activePopup = ExamplePopup::Info;

            // ----- Popup state machine -----
            if (activePopup == ExamplePopup::TextInput)
            {
                bool open = true;
                if (gui->PopupTextInput("New Project", "Name", popupInputText, &open))
                    SDL_Log("Submitted: %s", popupInputText.c_str());
                if (!open)
                    activePopup = ExamplePopup::None;
            }

            if (activePopup == ExamplePopup::Confirm)
            {
                bool open = true;
                if (gui->PopupConfirm("Delete?", "Are you sure?", &open))
                    SDL_Log("Confirmed!");
                if (!open)
                    activePopup = ExamplePopup::None;
            }

            if (activePopup == ExamplePopup::Info)
            {
                bool open = true;
                gui->PopupInfo("Notice", "Something happened.", &open);
                if (!open)
                    activePopup = ExamplePopup::None;
            }

            gui->End();
            SDL_RenderPresent(renderer);
        }
    };
} // namespace Gin