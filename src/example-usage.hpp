#pragma once

#include "app.hpp"
#include "gui.hpp"
#include <SDL3/SDL.h>

namespace Gin
{
    // Example of how to use the GUI system
    class ExampleGUIUsage
    {
    private:
        GUI* gui;
        SDL_Renderer* renderer;
        
        // Example state variables
        float volume = 0.5f;
        int selectedOption = 0;
        
    public:
        ExampleGUIUsage(SDL_Renderer* renderer, GUI* gui)
            : renderer(renderer), gui(gui) {}
        
        void render()
        {
            // Get mouse input - direct coordinates, no conversion needed
            float mouseXf, mouseYf;
            SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseXf, &mouseYf);
            bool mouseDown = (mouseButtons & SDL_BUTTON_LMASK) != 0;

            // Begin GUI frame
            gui->Begin((int)mouseXf, (int)mouseYf, mouseDown);

            // Get current window dimensions
            int W = gui->GetLogicalWidth();
            int H = gui->GetLogicalHeight();

            // Toolbar (fixed height)
            int toolbarH = 50;
            gui->Rect(0, 0, W, toolbarH, {45, 45, 45, 255});

            if (gui->Button("New Project", 10, 5, 140, 40))
            {
                SDL_Log("New Project button clicked!");
            }

            // Side panel (proportional)
            int panelW = SDL_max(200, W / 5);
            int panelY = toolbarH;
            int panelH = H - toolbarH;
            gui->Rect(0, panelY, panelW, panelH, {40, 40, 40, 255});

            gui->Label("Inspector", 10, panelY + 10);

            // Slider example
            if (gui->Slider("Volume", 10, panelY + 50, panelW - 20, &volume, 0.0f, 1.0f))
            {
                SDL_Log("Volume changed: %.2f", volume);
            }

            // Dropdown example
            std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
            if (gui->Dropdown("Quality", 10, panelY + 130, panelW - 20, &selectedOption, options))
            {
                SDL_Log("Quality changed to: %s", options[selectedOption].c_str());
            }

            // Viewport
            int vpX = panelW;
            int vpY = toolbarH;
            int vpW = W - panelW;
            int vpH = H - toolbarH;
            gui->Rect(vpX, vpY, vpW, vpH, {35, 35, 35, 255});
            gui->Label("Viewport", vpX + vpW / 2 - 30, vpY + vpH / 2);

            // End GUI frame
            gui->End();
        }
    };
} // namespace Gin