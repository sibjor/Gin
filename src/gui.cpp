#include "gui.hpp"

namespace Gin
{
    GUI::GUI(SDL_Renderer *renderer, TTF_Font *font, const int width, const int height)
        : renderer(renderer), font(font),
          logicalWidth(width), logicalHeight(height),
          mouseX(0), mouseY(0), mousePressed(false), mouseClicked(false),
          hotID(-1), activeID(-1), nextID(0),
          focusedID(-1), cursorPos(0),
          textSubmitted(false), textCancelled(false),
          scrollOffset(0.0f), scrollDelta(0.0f), scrollActive(false),
          scrollRegionX(0), scrollRegionY(0), scrollRegionW(0), scrollRegionH(0),
          scrollContentHeight(0.0f)
    {
    }

    void GUI::SetSize(int width, int height)
    {
        logicalWidth = width;
        logicalHeight = height;
    }

    void GUI::Begin(int mouseX, int mouseY, bool mousePressed)
    {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        this->mouseClicked = mousePressed && !this->mousePressed;
        this->mousePressed = mousePressed;

        this->hotID = -1;
        this->nextID = 0;

        // Reset per-frame text flags
        this->textSubmitted = false;
        this->textCancelled = false;

        // Scroll is not active until BeginScroll
        this->scrollActive = false;
    }

    void GUI::End()
    {
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
            activeID = -1; // Reset activeID after click
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

        // If slider is active, update value (even if mouse is outside)
        if (activeID == id && mousePressed)
        {
            float t = (float)(mouseX - x) / (float)w;
            if (t < 0.0f)
                t = 0.0f;
            if (t > 1.0f)
                t = 1.0f;
            *value = min + t * (max - min);
            changed = true;
        }

        // Release slider when mouse button is released
        if (activeID == id && !mousePressed)
        {
            activeID = -1;
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

    int GUI::LabelWrapped(const char *text, int x, int y, int maxWidth)
    {
        TextLayout layout = TextUtil::BuildLayout(font, text, maxWidth);

        int cy = y;
        for (int i = 0; i < layout.LineCount(); i++)
        {
            Text(layout.lines[i].text.c_str(), x, cy, {255, 255, 255, 255});
            cy += layout.lines[i].height + layout.lineSpacing;
        }
        return cy - y;
    }

    // === Scroll ===

    void GUI::HandleScrollEvent(float scrollY)
    {
        scrollDelta += scrollY;
    }

    int GUI::BeginScroll(int x, int y, int w, int h, float contentHeight)
    {
        scrollActive = true;
        scrollRegionX = x;
        scrollRegionY = y;
        scrollRegionW = w;
        scrollRegionH = h;
        scrollContentHeight = contentHeight;

        // Only scroll when mouse is inside the region
        if (RegionHit(x, y, w, h))
            scrollOffset -= scrollDelta * 40.0f;
        scrollDelta = 0.0f;

        // Clamp
        float maxScroll = contentHeight - (float)h;
        if (maxScroll < 0.0f) maxScroll = 0.0f;
        if (scrollOffset < 0.0f) scrollOffset = 0.0f;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;

        // Clip to scroll region
        SDL_Rect clip = {x, y, w, h};
        SDL_SetRenderClipRect(renderer, &clip);

        return -(int)scrollOffset;
    }

    void GUI::EndScroll()
    {
        // Draw scrollbar thumb if content exceeds view
        if (scrollContentHeight > (float)scrollRegionH)
        {
            float viewRatio = (float)scrollRegionH / scrollContentHeight;
            int barH = SDL_max(20, (int)(scrollRegionH * viewRatio));
            float scrollRatio = scrollOffset / (scrollContentHeight - (float)scrollRegionH);
            int barY = scrollRegionY + (int)(scrollRatio * (scrollRegionH - barH));
            int barX = scrollRegionX + scrollRegionW - 6;
            Rect(barX, barY, 4, barH, {100, 100, 100, 200}, true);
        }

        SDL_SetRenderClipRect(renderer, nullptr);
        scrollActive = false;
    }

    // === ProjectRow ===

    bool GUI::ProjectRow(const char *name, const char *date, int x, int y, int w, int h, bool selected)
    {
        int id = GenerateID();
        bool clicked = false;

        // Hit test
        if (RegionHit(x, y, w, h))
        {
            hotID = id;
            if (activeID == -1 && mouseClicked)
                activeID = id;
        }

        // Click detection
        if (hotID == id && activeID == id && !mousePressed)
        {
            clicked = true;
            activeID = -1;
        }

        // Background color: selected > hover > default
        SDL_Color bg;
        if (selected)
            bg = {60, 80, 120, 255};
        else if (hotID == id)
            bg = {55, 55, 55, 255};
        else
            bg = {45, 45, 45, 255};

        Rect(x, y, w, h, bg, true);

        // Bottom border
        Rect(x, y + h - 1, w, 1, {55, 55, 55, 255}, true);

        // Project name (left, vertically offset)
        Text(name, x + 15, y + 10, {255, 255, 255, 255});

        // Date (below name, dimmer)
        Text(date, x + 15, y + 32, {160, 160, 160, 255});

        return clicked;
    }

    // === SidePanel ===

    bool GUI::SidePanel(const char *title, int x, int y, int w, int h,
                        const std::vector<std::string> &details,
                        const char *actionLabel)
    {
        // Panel background
        Rect(x, y, w, h, {40, 40, 40, 255}, true);

        // Left border accent
        Rect(x, y, 2, h, {80, 80, 80, 255}, true);

        int pad = 15;
        int cy = y + pad;

        // Title
        Text(title, x + pad, cy, {255, 255, 255, 255});
        cy += 30;

        // Separator
        Rect(x + pad, cy, w - pad * 2, 1, {70, 70, 70, 255}, true);
        cy += 15;

        // Detail lines (wrapped to fit panel width)
        int wrapWidth = w - pad * 2;
        for (const auto &line : details)
        {
            TextLayout layout = TextUtil::BuildLayout(font, line, wrapWidth);

            for (int i = 0; i < layout.LineCount(); i++)
            {
                Text(layout.lines[i].text.c_str(), x + pad, cy, {180, 180, 180, 255});
                cy += layout.lines[i].height + layout.lineSpacing;
            }
            cy += 4; // extra gap between detail entries
        }

        cy += 10;

        // Action button (skip if label is empty or null)
        bool clicked = false;
        if (actionLabel && actionLabel[0] != '\0')
            clicked = Button(actionLabel, x + pad, cy, w - pad * 2, 35);

        return clicked;
    }

    // === Popup (internal chrome) ===

    SDL_Rect GUI::DrawPopupChrome(const char *title, int w, int h)
    {
        // Dim overlay
        Rect(0, 0, logicalWidth, logicalHeight, {0, 0, 0, 150}, true);

        // Center dialog
        int dx = (logicalWidth - w) / 2;
        int dy = (logicalHeight - h) / 2;

        Rect(dx, dy, w, h, {50, 50, 50, 255}, true);
        Rect(dx, dy, w, h, {100, 100, 100, 255}, false);

        // Title
        Label(title, dx + 20, dy + 15);

        return {dx, dy, w, h};
    }

    // === Popup widgets ===

    bool GUI::PopupTextInput(const char *title, const char *inputLabel,
                             std::string &value, bool *open)
    {
        SDL_Rect dlg = DrawPopupChrome(title, 400, 200);

        bool submitted = TextInput(inputLabel,
                                   dlg.x + 20, dlg.y + 50,
                                   dlg.w - 40, value);

        int btnY = dlg.y + dlg.h - 55;

        bool confirmed = Button("Create", dlg.x + dlg.w - 220, btnY, 95, 35) || submitted;
        bool cancelled = Button("Cancel", dlg.x + dlg.w - 115, btnY, 95, 35) || TextInputCancelled();

        if (cancelled)
        {
            *open = false;
            return false;
        }

        if (confirmed && !value.empty())
        {
            *open = false;
            return true;
        }

        return false;
    }

    bool GUI::PopupConfirm(const char *title, const char *message, bool *open)
    {
        SDL_Rect dlg = DrawPopupChrome(title, 400, 180);

        Label(message, dlg.x + 20, dlg.y + 60);

        int btnY = dlg.y + dlg.h - 55;

        bool ok = Button("OK", dlg.x + dlg.w - 220, btnY, 95, 35);
        bool cancel = Button("Cancel", dlg.x + dlg.w - 115, btnY, 95, 35);

        if (cancel)
        {
            *open = false;
            return false;
        }

        if (ok)
        {
            *open = false;
            return true;
        }

        return false;
    }

    void GUI::PopupInfo(const char *title, const char *message, bool *open)
    {
        SDL_Rect dlg = DrawPopupChrome(title, 400, 160);

        Label(message, dlg.x + 20, dlg.y + 55);

        int btnY = dlg.y + dlg.h - 55;

        if (Button("OK", dlg.x + dlg.w - 115, btnY, 95, 35))
        {
            *open = false;
        }
    }

    void GUI::HandleTextEvent(const char *text)
    {
        if (focusedID == -1 || !text)
            return;

        textBuffer.insert(cursorPos, text);
        cursorPos += (int)strlen(text);
    }

    void GUI::HandleKeyEvent(SDL_Keycode key)
    {
        if (focusedID == -1)
            return;

        switch (key)
        {
        case SDLK_BACKSPACE:
            if (cursorPos > 0)
            {
                textBuffer.erase(cursorPos - 1, 1);
                cursorPos--;
            }
            break;
        case SDLK_DELETE:
            if (cursorPos < (int)textBuffer.size())
                textBuffer.erase(cursorPos, 1);
            break;
        case SDLK_LEFT:
            if (cursorPos > 0)
                cursorPos--;
            break;
        case SDLK_RIGHT:
            if (cursorPos < (int)textBuffer.size())
                cursorPos++;
            break;
        case SDLK_HOME:
            cursorPos = 0;
            break;
        case SDLK_END:
            cursorPos = (int)textBuffer.size();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            textSubmitted = true;
            break;
        case SDLK_ESCAPE:
            textCancelled = true;
            focusedID = -1;
            break;
        }
    }

    bool GUI::TextInput(const char *label, int x, int y, int w, std::string &value)
    {
        int id = GenerateID();
        bool submitted = false;
        const int inputHeight = 30;
        const int labelHeight = 20;
        const int padding = 8;

        // Label above input
        Text(label, x, y, {200, 200, 200, 255});
        int inputY = y + labelHeight;

        bool isFocused = (focusedID == id);

        // Click to focus
        if (RegionHit(x, inputY, w, inputHeight))
        {
            hotID = id;
            if (mouseClicked)
            {
                focusedID = id;
                textBuffer = value;
                cursorPos = (int)textBuffer.size();
                isFocused = true;
            }
        }
        else if (mouseClicked && isFocused)
        {
            // Clicked outside — commit and lose focus
            value = textBuffer;
            focusedID = -1;
            isFocused = false;
        }

        // Enter pressed
        if (isFocused && textSubmitted)
        {
            value = textBuffer;
            focusedID = -1;
            submitted = true;
        }

        // What to display
        const std::string &displayText = isFocused ? textBuffer : value;

        // Background + border
        SDL_Color bg = isFocused ? SDL_Color{60, 60, 60, 255} : SDL_Color{50, 50, 50, 255};
        SDL_Color border = isFocused ? SDL_Color{100, 150, 255, 255} : SDL_Color{80, 80, 80, 255};
        Rect(x, inputY, w, inputHeight, bg, true);
        Rect(x, inputY, w, inputHeight, border, false);

        // Clip text to input area
        SDL_Rect clipRect = {x + padding, inputY, w - padding * 2, inputHeight};
        SDL_SetRenderClipRect(renderer, &clipRect);

        // Draw text or placeholder
        if (!displayText.empty())
            Text(displayText.c_str(), x + padding, inputY + 7, {255, 255, 255, 255});
        else if (!isFocused)
            Text("...", x + padding, inputY + 7, {100, 100, 100, 255});

        // Draw cursor when focused
        if (isFocused)
        {
            int cursorX = x + padding;
            if (cursorPos > 0 && font)
            {
                std::string beforeCursor = textBuffer.substr(0, cursorPos);
                int tw = 0, th = 0;
                TTF_GetStringSize(font, beforeCursor.c_str(), 0, &tw, &th);
                cursorX += tw;
            }

            // Simple blinking: toggle every ~500ms
            if ((SDL_GetTicks() / 500) % 2 == 0)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderLine(renderer, (float)cursorX, (float)(inputY + 4),
                               (float)cursorX, (float)(inputY + inputHeight - 4));
            }
        }

        SDL_SetRenderClipRect(renderer, nullptr);
        return submitted;
    }
} // namespace Gin