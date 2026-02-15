#include "project.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Gin
{
    // --- Metadata filename inside each project directory ---
    static constexpr const char *PROJECT_META_FILE = "project.gin";

    // --- ProjectVersion ---

    std::string ProjectVersion::toString() const
    {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }

    ProjectVersion ProjectVersion::fromString(const std::string &str)
    {
        ProjectVersion v;
        int parts[3] = {0, 0, 0};
        int idx = 0;
        std::istringstream ss(str);
        std::string token;
        while (std::getline(ss, token, '.') && idx < 3)
        {
            try { parts[idx] = std::stoi(token); }
            catch (...) { parts[idx] = 0; }
            idx++;
        }
        v.major = parts[0];
        v.minor = parts[1];
        v.patch = parts[2];
        return v;
    }

    // --- ProjectManager ---

    ProjectManager::ProjectManager()
    {
        char *prefPath = SDL_GetPrefPath(NULL, "Gin");
        if (!prefPath)
        {
            SDL_Log("ProjectManager: Failed to get preferences path: %s", SDL_GetError());
            basePath = "projects/";
        }
        else
        {
            basePath = std::string(prefPath) + "projects"
                     + std::string(1, std::filesystem::path::preferred_separator);
            SDL_free(prefPath);
        }
        std::filesystem::create_directories(basePath);
    }

    bool ProjectManager::isValidName(const std::string &name)
    {
        if (name.empty())
            return false;
        for (char c : name)
        {
            if (c == '/' || c == '\\' || c == '.' || c == ':' ||
                c == '*' || c == '?' || c == '"' || c == '<' ||
                c == '>' || c == '|')
                return false;
        }
        return true;
    }

    std::string ProjectManager::metaFilePath(const std::string &projectPath)
    {
        return (std::filesystem::path(projectPath) / PROJECT_META_FILE).string();
    }

    std::vector<ProjectInfo> ProjectManager::listProjects() const
    {
        std::vector<ProjectInfo> projects;

        if (!std::filesystem::exists(basePath))
            return projects;

        for (const auto &entry : std::filesystem::directory_iterator(basePath))
        {
            if (!entry.is_directory())
                continue;

            std::string dirPath = entry.path().string();
            std::string meta = metaFilePath(dirPath);

            ProjectInfo info;
            info.path = dirPath;
            info.lastModified = formatTime(entry.last_write_time());

            // Try to load metadata from project.gin
            if (std::filesystem::exists(meta))
            {
                ProjectInfo loaded = loadProject(dirPath);
                info.projectName = loaded.projectName;
                info.authorName = loaded.authorName;
                info.version = loaded.version;

                // Use metadata file timestamp for better accuracy
                info.lastModified = formatTime(std::filesystem::last_write_time(meta));
            }

            // Fallback: derive name from directory
            if (info.projectName.empty())
                info.projectName = entry.path().filename().string();

            projects.push_back(info);
        }

        // Newest first
        std::sort(projects.begin(), projects.end(),
                  [](const ProjectInfo &a, const ProjectInfo &b)
                  { return a.lastModified > b.lastModified; });

        return projects;
    }

    ProjectInfo ProjectManager::createProject(const std::string &name, const std::string &author)
    {
        ProjectInfo info;

        if (!isValidName(name))
        {
            SDL_Log("ProjectManager: Invalid project name: '%s'", name.c_str());
            return info; // empty projectName signals failure
        }

        std::string projectPath = basePath + name;

        if (std::filesystem::exists(projectPath))
        {
            SDL_Log("ProjectManager: Project already exists: '%s'", name.c_str());
            return info;
        }

        // Create project directory
        std::filesystem::create_directories(projectPath);

        // Populate info
        info.projectName = name;
        info.authorName = author;
        info.path = projectPath;
        info.version = {1, 0, 0};

        // Write metadata file
        if (!saveProject(info))
        {
            SDL_Log("ProjectManager: Created directory but failed to save metadata");
        }

        // Set last modified
        info.lastModified = formatTime(std::filesystem::last_write_time(projectPath));

        SDL_Log("ProjectManager: Created project '%s' at %s", name.c_str(), projectPath.c_str());
        return info;
    }

    bool ProjectManager::deleteProject(const std::string &projectPath)
    {
        if (!std::filesystem::exists(projectPath))
        {
            SDL_Log("ProjectManager: Path does not exist: %s", projectPath.c_str());
            return false;
        }

        std::error_code ec;
        std::filesystem::remove_all(projectPath, ec);
        if (ec)
        {
            SDL_Log("ProjectManager: Failed to delete '%s': %s",
                    projectPath.c_str(), ec.message().c_str());
            return false;
        }

        SDL_Log("ProjectManager: Deleted project at %s", projectPath.c_str());
        return true;
    }

    bool ProjectManager::saveProject(const ProjectInfo &project) const
    {
        std::string meta = metaFilePath(project.path);

        std::ofstream file(meta, std::ios::trunc);
        if (!file.is_open())
        {
            SDL_Log("ProjectManager: Could not write metadata: %s", meta.c_str());
            return false;
        }

        file << "# Gin Project Metadata\n";
        file << "name=" << project.projectName << "\n";
        file << "author=" << project.authorName << "\n";
        file << "version=" << project.version.toString() << "\n";

        for (const auto &entry : project.metaData)
            file << "meta=" << entry << "\n";

        file.close();
        return true;
    }

    ProjectInfo ProjectManager::loadProject(const std::string &projectPath) const
    {
        ProjectInfo info;
        info.path = projectPath;

        // Derive default name from directory
        info.projectName = std::filesystem::path(projectPath).filename().string();

        std::string meta = metaFilePath(projectPath);
        if (!std::filesystem::exists(meta))
            return info;

        std::ifstream file(meta);
        if (!file.is_open())
            return info;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            auto eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);

            if (key == "name")
                info.projectName = value;
            else if (key == "author")
                info.authorName = value;
            else if (key == "version")
                info.version = ProjectVersion::fromString(value);
            else if (key == "meta")
                info.metaData.push_back(value);
        }

        file.close();

        // Set last modified from metadata file
        if (std::filesystem::exists(meta))
            info.lastModified = formatTime(std::filesystem::last_write_time(meta));

        return info;
    }

    std::string ProjectManager::formatTime(std::filesystem::file_time_type tp)
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

} // namespace Gin
