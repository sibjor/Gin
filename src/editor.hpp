#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "gui.hpp"
#include "project.hpp"

namespace Gin
{
    enum class EditorResult
    {
        Continue,   // Keep running the editor
        Quit,       // User wants to quit the application
        BackToHub   // User wants to return to the intro hub
    };

    class Editor
    {
    public:
        Editor(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, const ProjectInfo &project);
        ~Editor();

        EditorResult update();

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        GUI *gui;
        ProjectInfo project;
        ProjectManager projectManager;

        bool textInputWasActive;

        void handleEvents(bool &quit, bool &backToHub);
        void render(bool &backToHub);
    };
}
