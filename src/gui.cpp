#include "gui.hpp"

namespace Gin
{
    GUI::GUI(SDL_Renderer *renderer, TTF_Font *font)
        : renderer(renderer), font(font),
          mouseX(0), mouseY(0), mousePressed(false), mouseClicked(false),
          hotID(-1), activeID(-1), nextID(0)
    {
    }

    void GUI::Begin(int mouseX, int mouseY, bool mousePressed)
    {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        // Detect click (transition from not pressed to pressed)
        this->mouseClicked = mousePressed && !this->mousePressed;
        this->mousePressed = mousePressed;

        this->hotID = -1;
        this->nextID = 0;
    }

    void GUI::End()
    {
        // Empty - widgets manage their own state
    }

    int GUI::GenerateID()
    {
        return nextID++;
    }

    bool GUI::RegionHit(int x, int y, int w, int h)
    {
        return mouseX >= x && mouseX < x + w &&
               mouseY >= y && mouseY < y + h;
    }

    void GUI::Rect(int x, int y, int w, int h, SDL_Color color, bool filled)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_FRect rect = {(float)x, (float)y, (float)w, (float)h};

        if (filled)
            SDL_RenderFillRect(renderer, &rect);
        else
            SDL_RenderRect(renderer, &rect);
    }

    void GUI::Text(const char *text, int x, int y, SDL_Color color)
    {
        if (!font || !text)
            return;

        SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
        if (!surface)
            return;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture)
        {
            SDL_FRect dest = {(float)x, (float)y, (float)surface->w, (float)surface->h};
            SDL_RenderTexture(renderer, texture, nullptr, &dest);
            SDL_DestroyTexture(texture);
        }

        SDL_DestroySurface(surface);
    }

    bool GUI::Button(const char *label, int x, int y, int w, int h)
    {
        int id = GenerateID();
        bool clicked = false;

        // Check if mouse is over button
        if (RegionHit(x, y, w, h))
        {
            hotID = id;

            if (activeID == -1 && mouseClicked)
            {
                activeID = id;
            }
        }

        // If button is hot and active, and mouse is released, it was clicked
        if (hotID == id && activeID == id && !mousePressed)
        {
            clicked = true;
        }

        // Render button
        SDL_Color color;
        if (activeID == id)
            color = {100, 100, 100, 255}; // Pressed
        else if (hotID == id)
            color = {150, 150, 150, 255}; // Hover
        else
            color = {120, 120, 120, 255}; // Normal

        Rect(x, y, w, h, color, true);
        Rect(x, y, w, h, {200, 200, 200, 255}, false); // Border

        // Center text
        if (font)
        {
            int textW = 0, textH = 0;
            TTF_GetStringSize(font, label, 0, &textW, &textH);
            Text(label, x + (w - textW) / 2, y + (h - textH) / 2, {255, 255, 255, 255});
        }

        return clicked;
    }

    bool GUI::Slider(const char *label, int x, int y, int w, float *value, float min, float max)
    {
        int id = GenerateID();
        bool changed = false;

        const int sliderHeight = 20;
        const int handleWidth = 10;
        const int labelHeight = 20;

        Text(label, x, y, {255, 255, 255, 255});

        int sliderY = y + labelHeight;

        // Check if mouse is over slider
        if (RegionHit(x, sliderY, w, sliderHeight))
        {
            hotID = id;

            if (activeID == -1 && mouseClicked)
            {
                activeID = id;
            }
        }

        // If slider is active, update value (even if mouse moved outside! - BUG!)
        if (activeID == id && mousePressed)
        {
            hotID = id;
            
            float t = (float)(mouseX - x) / (float)w;
            if (t < 0.0f)
                t = 0.0f;
            if (t > 1.0f)
                t = 1.0f;
            *value = min + t * (max - min);
            changed = true;
        }

        Rect(x, sliderY + sliderHeight / 2 - 2, w, 4, {80, 80, 80, 255}, true);

        float t = (*value - min) / (max - min);
        int handleX = x + (int)(t * w) - handleWidth / 2;

        SDL_Color handleColor = (activeID == id) ? SDL_Color{200, 200, 200, 255} : (hotID == id) ? SDL_Color{180, 180, 180, 255}
                                                                                                   : SDL_Color{150, 150, 150, 255};

        Rect(handleX, sliderY, handleWidth, sliderHeight, handleColor, true);

        char valueText[32];
        snprintf(valueText, sizeof(valueText), "%.2f", *value);
        Text(valueText, x + w + 10, sliderY, {255, 255, 255, 255});

        return changed;
    }

    bool GUI::Dropdown(const char *label, int x, int y, int w, int *selected, const std::vector<std::string> &options)
    {
        int id = GenerateID();
        bool changed = false;

        const int itemHeight = 30;

        // Draw label above dropdown
        Text(label, x, y - 25, {255, 255, 255, 255});

        // Main dropdown button
        bool mainHit = RegionHit(x, y, w, itemHeight);

        if (mainHit && mouseClicked)
        {
            // Toggle dropdown
            if (activeID == id)
                activeID = -1; // Close
            else
                activeID = id; // Open
        }

        // Render main button
        SDL_Color mainColor = mainHit ? SDL_Color{150, 150, 150, 255} : SDL_Color{120, 120, 120, 255};
        Rect(x, y, w, itemHeight, mainColor, true);
        Rect(x, y, w, itemHeight, {200, 200, 200, 255}, false);

        // Draw selected option
        if (*selected >= 0 && *selected < (int)options.size())
        {
            Text(options[*selected].c_str(), x + 10, y + 7, {255, 255, 255, 255});
        }

        // Draw arrow
        const char *arrow = (activeID == id) ? "▲" : "▼";
        Text(arrow, x + w - 25, y + 5, {255, 255, 255, 255});

        // Render dropdown list if open
        if (activeID == id)
        {
            for (int i = 0; i < (int)options.size(); i++)
            {
                int itemY = y + itemHeight + i * itemHeight;
                bool itemHit = RegionHit(x, itemY, w, itemHeight);

                if (itemHit && mouseClicked)
                {
                    *selected = i;
                    activeID = -1; // Close dropdown
                    changed = true;
                }

                SDL_Color itemColor = itemHit ? SDL_Color{140, 140, 140, 255} : SDL_Color{100, 100, 100, 255};
                Rect(x, itemY, w, itemHeight, itemColor, true);
                Rect(x, itemY, w, itemHeight, {200, 200, 200, 255}, false);

                Text(options[i].c_str(), x + 10, itemY + 7, {255, 255, 255, 255});
            }
        }

        return changed;
    }

    void GUI::Label(const char *text, int x, int y)
    {
        Text(text, x, y, {255, 255, 255, 255});
    }
} // namespace Gin