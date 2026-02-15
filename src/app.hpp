#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace Gin
{
    /// Initializes all SDL subsystems (video, audio, gamepad, TTF, mixer).
    bool initSubsystems();

    /// Creates a resizable window and renderer with VSync enabled.
    bool createWindow(const char *title, int width, int height,
                      SDL_Window **window, SDL_Renderer **renderer);

    /// Loads the default UI font. Returns nullptr on failure.
    TTF_Font *loadDefaultFont();

    /// Shuts down all SDL subsystems and frees window/renderer/font.
    void shutdownAll(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font);

} // namespace Gin