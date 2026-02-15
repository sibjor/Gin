#include "files.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

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
        basePath = std::string(prefPath) + "projects" + std::string(1, std::filesystem::path::preferred_separator);
        SDL_free(prefPath);
        std::filesystem::create_directories(basePath);
    }
}

Gin::FS::~FS()
{
}

std::string Gin::FS::getBasePath() const
{
    return basePath;
}

bool Gin::FS::createProject(const std::string &projectName)
{
    for (char c : projectName)
    {
        if (c == '/' || c == '\\' || c == '.' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
        {
            SDL_Log("FS: Invalid project name: '%s'", projectName.c_str());
            return false;
        }
    }

    const std::string projectPath = basePath + projectName;

    if (std::filesystem::exists(projectPath))
    {
        SDL_Log("FS: Project already exists: '%s'", projectName.c_str());
        return false;
    }

    std::filesystem::create_directories(projectPath);
    SDL_Log("FS: Created project at: %s", projectPath.c_str());
    return true;
}

std::vector<Gin::ProjectInfo> Gin::FS::listProjects() const
{
    std::vector<ProjectInfo> projects;

    if (!std::filesystem::exists(basePath))
        return projects;

    for (const auto &entry : std::filesystem::directory_iterator(basePath))
    {
        if (!entry.is_directory())
            continue;

        ProjectInfo info;
        info.projectName = entry.path().filename().string();
        info.path = entry.path().string();
        info.lastModified = formatTime(entry.last_write_time());
        projects.push_back(info);
    }

    // Newest first
    std::sort(projects.begin(), projects.end(), [](const ProjectInfo &a, const ProjectInfo &b)
              { return a.lastModified > b.lastModified; });

    return projects;
}

std::string Gin::FS::formatTime(std::filesystem::file_time_type tp)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        tp - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}