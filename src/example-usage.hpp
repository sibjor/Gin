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
            // Get mouse input
            float windowMouseX, windowMouseY;
            Uint8 mouseState = SDL_GetMouseState(&windowMouseX, &windowMouseY);
            bool mouseDown = (mouseState & SDL_BUTTON_LMASK) != 0;

            // Convert to logical coordinates
            float logicalMouseX, logicalMouseY;
            SDL_RenderCoordinatesFromWindow(renderer, windowMouseX, windowMouseY, 
                                           &logicalMouseX, &logicalMouseY);

            // Begin GUI frame
            gui->Begin((int)logicalMouseX, (int)logicalMouseY, mouseDown);

            // Label example
            gui->Label("Gin Engine Demo", 50, 50);

            // Button example
            if (gui->Button("New Project", 50, 100, 200, 50))
            {
                SDL_Log("New Project button clicked!");
            }

            // Slider example
            if (gui->Slider("Volume", 50, 180, 300, &volume, 0.0f, 1.0f))
            {
                SDL_Log("Volume changed: %.2f", volume);
            }

            // Dropdown example
            std::vector<std::string> options = {"Low", "Medium", "High", "Ultra"};
            if (gui->Dropdown("Quality", 50, 280, 300, &selectedOption, options))
            {
                SDL_Log("Quality changed to: %s", options[selectedOption].c_str());
            }

            // End GUI frame
            gui->End();
        }
    };
} // namespace Gin