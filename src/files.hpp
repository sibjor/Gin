#pragma once
#include <SDL3/SDL.h>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include "project.hpp"

namespace Gin
{
    // FS - FileSystem utilities (generic file operations)
    class FS
    {
    public:
        FS();
        ~FS();
        std::string getBasePath() const;
        bool createProject(const std::string& projectName);
        std::vector<ProjectInfo> listProjects() const;

    private:
        std::string basePath;
        static std::string formatTime(std::filesystem::file_time_type tp);
    };
} // Namespace Gin