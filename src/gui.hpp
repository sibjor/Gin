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
        GUI(SDL_Renderer* renderer, TTF_Font* font, const int width, const int height);
        
        // Call each frame
        void Begin(int mouseX, int mouseY, bool mousePressed);
        void End();

        // Update on window resize
        void SetSize(int width, int height);
        
        // Widgets - return true if interacted with
        bool Button(const char* label, int x, int y, int w, int h);
        bool Slider(const char* label, int x, int y, int w, float* value, float min, float max);
        bool Dropdown(const char* label, int x, int y, int w, int* selected, const std::vector<std::string>& options);
        void Label(const char* text, int x, int y);
        void Rect(int x, int y, int w, int h, SDL_Color color, bool filled = true);
        
        // Helper methods for responsive positioning
        int ScaleX(float percent) const { return (int)(logicalWidth * percent); }
        int ScaleY(float percent) const { return (int)(logicalHeight * percent); }
        int GetLogicalWidth() const { return logicalWidth; }
        int GetLogicalHeight() const { return logicalHeight; }
        
    private:
        SDL_Renderer* renderer;
        TTF_Font* font;
        int logicalWidth;
        int logicalHeight;
        
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
        void Text(const char* text, int x, int y, SDL_Color color);
    };
} // namespace Gin