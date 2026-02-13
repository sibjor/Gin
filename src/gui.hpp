#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

namespace Gin
{
    class GUI
    {
    public:
        GUI(SDL_Renderer* renderer, TTF_Font* font);
        
        // Call each frame
        void Begin(int mouseX, int mouseY, bool mousePressed);
        void End();
        
        // Widgets - return true if interacted with
        bool Button(const char* label, int x, int y, int w, int h);
        bool Slider(const char* label, int x, int y, int w, float* value, float min, float max);
        bool Dropdown(const char* label, int x, int y, int w, int* selected, const std::vector<std::string>& options);
        void Label(const char* text, int x, int y);
        
    private:
        SDL_Renderer* renderer;
        TTF_Font* font;
        
        // Mouse state
        int mouseX, mouseY;
        bool mousePressed;
        bool mouseClicked;
        
        // Widget tracking
        int hotID;
        int activeID;
        int nextID;
        
        // Helper functions
        int GenerateID();
        bool RegionHit(int x, int y, int w, int h);
        void Rect(int x, int y, int w, int h, SDL_Color color, bool filled = true);
        void Text(const char* text, int x, int y, SDL_Color color);
    };
} // namespace Gin