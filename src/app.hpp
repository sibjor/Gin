#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "gui.hpp"
#include "files.hpp"

namespace Gin
{
    class App
    {
    public:
        App();
        ~App();
        bool initialize(const char *title, int width, int height);
        void run();
        void cleanup();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        bool isRunning;
        TTF_Font *defaultFont;
        GUI* gui;
        FS fs;

        // Project list
        std::vector<ProjectInfo> projects;
        int selectedProjectIndex;

        // Popup
        PopupType activePopup;
        std::string popupInputText;
        bool textInputWasActive;

        void handleEvents();
        void update();
        void render();
        bool loadFonts();
        void refreshProjects();
    };
} // namespace Gin