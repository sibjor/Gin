#pragma once

#include <SDL3/SDL.h>
#include <filesystem>
#include <string>
#include <vector>

namespace Gin
{
    struct ProjectVersion
    {
        int major = 1;
        int minor = 0;
        int patch = 0;

        std::string toString() const;
        static ProjectVersion fromString(const std::string &str);
    };

    struct ProjectInfo
    {
        std::string projectName;
        std::string authorName;
        std::string path;           // absolute path to project root directory
        std::string lastModified;   // human-readable "YYYY-MM-DD HH:MM"
        ProjectVersion version;
        std::vector<std::string> metaData;
    };

    class ProjectManager
    {
    public:
        ProjectManager();
        ~ProjectManager() = default;

        /// Returns the base projects directory (e.g. AppData/Gin/projects/).
        const std::string &getBasePath() const { return basePath; }

        /// Lists all projects in the base directory, newest first.
        std::vector<ProjectInfo> listProjects() const;

        /// Creates a new project directory and metadata file. Returns info (empty name on failure).
        ProjectInfo createProject(const std::string &name, const std::string &author = "");

        /// Deletes the project directory at the given path.
        bool deleteProject(const std::string &projectPath);

        /// Renames a project (directory + metadata). Returns true on success.
        bool renameProject(const std::string &projectPath, const std::string &newName);

        /// Saves project metadata to disk (project.gin file).
        bool saveProject(const ProjectInfo &project) const;

        /// Loads project info from a project directory.
        ProjectInfo loadProject(const std::string &projectPath) const;

    private:
        std::string basePath;

        /// Returns the path to the metadata file inside a project directory.
        static std::string metaFilePath(const std::string &projectPath);

        /// Validates a project name (no illegal filesystem characters).
        static bool isValidName(const std::string &name);

        /// Formats a file_time_type to "YYYY-MM-DD HH:MM".
        static std::string formatTime(std::filesystem::file_time_type tp);
    };

} // namespace Gin