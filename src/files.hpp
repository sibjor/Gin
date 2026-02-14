#pragma once
#include <SDL3/SDL.h>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

namespace Gin
{
    // Info about a project on disk
    struct ProjectInfo
    {
        std::string name;
        std::string path;
        std::string lastModified; // human-readable "YYYY-MM-DD HH:MM"
    };

    // FS - FileSystem
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