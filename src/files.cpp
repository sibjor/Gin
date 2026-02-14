#include "files.hpp"

Gin::FS::FS()
{
    char *prefPath = SDL_GetPrefPath(NULL, "Gin");
    if (!prefPath)
    {
        SDL_Log("FS: Failed to get preferences path: %s", SDL_GetError());
        basePath = "/projects/";
    }
    else
    {
        basePath = std::string(prefPath) + "projects"
            + std::string(1, std::filesystem::path::preferred_separator);
        SDL_free(prefPath);
        std::filesystem::create_directories(basePath);
    }
}

Gin::FS::~FS()
{
}
