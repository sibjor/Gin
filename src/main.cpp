#include "app.hpp"
#include "intro-hub.hpp"
#include "editor.hpp"

int main(int argc, char *argv[])
{
    // --- Initialize subsystems ---
    if (!Gin::initSubsystems())
        return -1;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!Gin::createWindow("Gin", 1280, 720, &window, &renderer))
    {
        Gin::shutdownAll(window, renderer, nullptr);
        return -1;
    }

    TTF_Font *font = Gin::loadDefaultFont();
    if (!font)
    {
        Gin::shutdownAll(window, renderer, font);
        return -1;
    }

    SDL_Log("Initialized with window size: 1280x720");

    // --- Main application state machine ---
    bool running = true;

    while (running)
    {
        // === Phase 1: Intro Hub (project picker) ===
        bool startEditor = false;
        std::string projectPath;
        std::string projectName;

        {
            Gin::IntroHub hub(window, renderer, font);

            while (running && !startEditor)
            {
                Gin::IntroHubResult result = hub.update();

                switch (result)
                {
                case Gin::IntroHubResult::Quit:
                    running = false;
                    break;

                case Gin::IntroHubResult::OpenProject:
                    startEditor = true;
                    projectPath = hub.getSelectedProjectPath();
                    projectName = hub.getSelectedProjectName();
                    break;

                case Gin::IntroHubResult::Continue:
                    break;
                }
            }
        } // IntroHub destroyed here, freeing its GUI resources

        // === Phase 2: Editor ===
        if (startEditor)
        {
            SDL_Log("Opening editor for project: %s (%s)",
                    projectName.c_str(), projectPath.c_str());

            // TODO: Create and run Editor instance
            // Gin::Editor editor(window, renderer, font, projectPath, projectName);
            // while (running) {
            //     auto result = editor.update();
            //     if (result == EditorResult::Quit) { running = false; break; }
            //     if (result == EditorResult::BackToHub) { break; }
            // }
        }
    }

    Gin::shutdownAll(window, renderer, font);
    return 0;
}