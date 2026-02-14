#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>
#include "text.hpp"

namespace Gin
{
    // Popup type enum — controls which kind of popup the app shows
    enum class PopupType
    {
        None,
        TextInput,  // Popup with a text input field + Create/Cancel
        Confirm,    // Popup with a message + OK/Cancel
        Info        // Popup with a message + OK
    };

    class GUI
    {
    public:
        GUI(SDL_Renderer *renderer, TTF_Font *font, const int width, const int height);

        // Call each frame
        void Begin(int mouseX, int mouseY, bool mousePressed);
        void End();

        // Update on window resize
        void SetSize(int width, int height);

        // --- Basic widgets ---
        bool Button(const char *label, int x, int y, int w, int h);
        bool Slider(const char *label, int x, int y, int w, float *value, float min, float max);
        bool Dropdown(const char *label, int x, int y, int w, int *selected, const std::vector<std::string> &options);
        bool TextInput(const char *label, int x, int y, int w, std::string &value);
        void Label(const char *text, int x, int y);
        // Label with word-wrapping. Returns the total height used (pixels).
        int LabelWrapped(const char *text, int x, int y, int maxWidth);
        void Rect(int x, int y, int w, int h, SDL_Color color, bool filled = true);

        // --- Compound widgets ---

        // Scrollable region. Call BeginScroll before items, EndScroll after.
        // Returns Y offset to add to each item's Y coordinate.
        int BeginScroll(int x, int y, int w, int h, float contentHeight);
        void EndScroll();

        // A clickable project row (name + date). Returns true if clicked.
        bool ProjectRow(const char *name, const char *date, int x, int y, int w, int h, bool selected);

        // A side panel with title, detail lines, and an action button.
        // Returns true if the action button is clicked.
        bool SidePanel(const char *title, int x, int y, int w, int h,
                       const std::vector<std::string> &details,
                       const char *actionLabel);

        // Popup widgets — self-contained popups drawn as overlays.
        // PopupTextInput: title + text input + Create/Cancel. Returns true on submit.
        bool PopupTextInput(const char *title, const char *inputLabel,
                            std::string &value, bool *open);

        // PopupConfirm: title + message + OK/Cancel. Returns true on OK.
        bool PopupConfirm(const char *title, const char *message, bool *open);

        // PopupInfo: title + message + OK. Closes on OK.
        void PopupInfo(const char *title, const char *message, bool *open);

        // --- Input forwarding ---
        void HandleTextEvent(const char *text);
        void HandleKeyEvent(SDL_Keycode key);
        void HandleScrollEvent(float scrollY);

        // TextInput state queries
        bool HasTextFocus() const { return focusedID != -1; }
        bool TextInputSubmitted() const { return textSubmitted; }
        bool TextInputCancelled() const { return textCancelled; }

        // Helper methods for responsive positioning
        int ScaleX(float percent) const { return (int)(logicalWidth * percent); }
        int ScaleY(float percent) const { return (int)(logicalHeight * percent); }
        int GetLogicalWidth() const { return logicalWidth; }
        int GetLogicalHeight() const { return logicalHeight; }

    private:
        SDL_Renderer *renderer;
        TTF_Font *font;
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

        // TextInput state (separate from activeID)
        int focusedID;
        std::string textBuffer;
        int cursorPos;
        bool textSubmitted;
        bool textCancelled;

        // Scroll state
        float scrollOffset;
        float scrollDelta;
        bool scrollActive;
        int scrollRegionX, scrollRegionY, scrollRegionW, scrollRegionH;
        float scrollContentHeight;

        // Helper functions
        int GenerateID();
        bool RegionHit(int x, int y, int w, int h);
        void Text(const char *text, int x, int y, SDL_Color color);

        // Internal popup chrome (dimmed overlay + centered box)
        SDL_Rect DrawPopupChrome(const char *title, int w, int h);
    };
} // namespace Gin