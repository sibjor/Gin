#pragma once

#include "app.hpp"
#include "gui.hpp"
#include <SDL3/SDL.h>

namespace Gin
{
    // =========================================================================
    // Tutorial: How to use the Gin GUI system
    // This file is NOT compiled into the app — it's a developer reference.
    // =========================================================================
    class ExampleGUIUsage
    {
    private:
        GUI* gui;
        SDL_Renderer* renderer;
        
        // Example state variables
        float volume = 0.5f;
        int selectedOption = 0;
        std::string playerName;
        int selectedRow = -1;
        
    public:
        ExampleGUIUsage(SDL_Renderer* renderer, GUI* gui)
            : renderer(renderer), gui(gui) {}
        
        void render()
        {
            // Get mouse input - direct coordinates, no conversion needed
            float mouseXf, mouseYf;
            SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseXf, &mouseYf);
            bool mouseDown = (mouseButtons & SDL_BUTTON_LMASK) != 0;

            // Begin GUI frame (call once per frame)
            gui->Begin((int)mouseXf, (int)mouseYf, mouseDown);

            int W = gui->GetLogicalWidth();
            int H = gui->GetLogicalHeight();

            // ----- Basic Widgets -----

            // Rect — filled or outlined rectangle
            gui->Rect(0, 0, W, 50, {45, 45, 45, 255});        // filled (default)
            gui->Rect(10, 60, 200, 100, {255, 0, 0, 255}, false); // outline only

            // Label — simple white text
            gui->Label("Hello, Gin!", 20, 70);

            // Button — returns true on click
            if (gui->Button("Click Me", 10, 5, 140, 40))
                SDL_Log("Button clicked!");

            // Slider — returns true when value changes
            if (gui->Slider("Volume", 10, 180, 200, &volume, 0.0f, 1.0f))
                SDL_Log("Volume: %.2f", volume);

            // Dropdown — returns true when selection changes
            std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
            if (gui->Dropdown("Quality", 10, 250, 200, &selectedOption, options))
                SDL_Log("Quality: %s", options[selectedOption].c_str());

            // TextInput — returns true when Enter is pressed
            if (gui->TextInput("Player Name", 10, 330, 200, playerName))
                SDL_Log("Name: %s", playerName.c_str());

            // ----- Compound Widgets -----

            // ProjectRow — a clickable row with name and date
            // bool ProjectRow(name, date, x, y, w, h, selected)
            // Returns true if clicked.
            //
            // if (gui->ProjectRow("My Game", "2026-02-14", 10, 400, 300, 60, false))
            //     SDL_Log("Row clicked!");

            // SidePanel — panel with title, detail lines, and action button
            // bool SidePanel(title, x, y, w, h, details, actionLabel)
            // Returns true if the action button is clicked.
            //
            // std::vector<std::string> details = {"Name: My Game", "Path: /projects/my-game"};
            // if (gui->SidePanel("Details", 500, 50, 250, 400, details, "Open"))
            //     SDL_Log("Open clicked!");

            // ----- Scrollable Region -----
            // Use BeginScroll / EndScroll to create a scrollable list.
            // BeginScroll returns a Y offset to add to item coordinates.
            // Note: forward SDL_EVENT_MOUSE_WHEEL to gui->HandleScrollEvent(event.wheel.y)
            //
            // int scrollY = gui->BeginScroll(x, y, w, h, contentHeight);
            // for (int i = 0; i < count; i++)
            // {
            //     int iy = y + i * 60 + scrollY;
            //     gui->ProjectRow(names[i], dates[i], x, iy, w, 58, i == selected);
            // }
            // gui->EndScroll();

            // ----- Popup Widgets -----
            // PopupType enum: None, TextInput, Confirm, Info
            //
            // PopupTextInput — text input popup, returns true on submit:
            //   bool open = true;
            //   if (gui->PopupTextInput("New Project", "Name", text, &open))
            //       SDL_Log("Submitted: %s", text.c_str());
            //   if (!open) activePopup = PopupType::None;
            //
            // PopupConfirm — OK/Cancel popup, returns true on OK:
            //   bool open = true;
            //   if (gui->PopupConfirm("Delete?", "Are you sure?", &open))
            //       SDL_Log("Confirmed!");
            //   if (!open) activePopup = PopupType::None;
            //
            // PopupInfo — info popup with OK button:
            //   bool open = true;
            //   gui->PopupInfo("Error", "Something went wrong.", &open);
            //   if (!open) activePopup = PopupType::None;

            // End GUI frame (call once per frame)
            gui->End();
        }
    };
} // namespace Gin