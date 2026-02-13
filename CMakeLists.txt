cmake_minimum_required(VERSION 3.16)

project(Gin LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_mixer EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

# Executable
add_executable(Gin
    src/main.cpp
    src/app.cpp
    src/gui.cpp
)

# Link everything you need
target_link_libraries(Gin PRIVATE
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
)

# Copy assets folder to build directory
add_custom_command(TARGET Gin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Gin>/assets
    COMMENT "Copying assets to build directory"
)