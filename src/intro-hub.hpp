#pragma once
#include <string>
#include <vector>
#include "gui.hpp"
#include "project.hpp"

namespace Gin
{
    enum class IntroHubResult
    {
        Continue,    // Keep showing the hub
        OpenProject, // User selected a project to open
        Quit         // User wants to quit
    };

    class IntroHub
    {
    public:
        IntroHub(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font);
        ~IntroHub();

        IntroHubResult update();

        const std::string &getSelectedProjectPath() const { return selectedProjectPath; }
        const std::string &getSelectedProjectName() const { return selectedProjectName; }

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        GUI *gui;
        ProjectManager projectManager;

        std::vector<ProjectInfo> projects;
        int selectedProjectIndex;

        PopupType activePopup;
        std::string popupInputText;
        bool textInputWasActive;

        std::string selectedProjectPath;
        std::string selectedProjectName;

        void refreshProjects();
        void handleEvents(bool &quit, bool &openProject);
        void render(bool &openProject);
    };
} // namespace Gin
