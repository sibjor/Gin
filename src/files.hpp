#pragma once
#include <SDL3/SDL.h>
#include <filesystem>
#include <string>
#include <iostream>

namespace Gin
{
    // FS - FileSystem
    class FS
    {
    public:
        FS();
        ~FS();
        std::string getBasePath() const;
    private:
        std::string basePath;
    };
} // Namespace Gin