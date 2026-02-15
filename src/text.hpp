#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

namespace Gin
{
    // A single measured line of text
    struct TextLine
    {
        std::string text;
        int width = 0;
        int height = 0;
    };

    // Pre-computed text layout: lines, dimensions, and cursor mapping
    struct TextLayout
    {
        std::vector<TextLine> lines;
        int totalWidth = 0;
        int totalHeight = 0;
        int lineSpacing = 4; // extra pixels between lines

        int LineCount() const { return (int)lines.size(); }
        bool Empty() const { return lines.empty(); }

        // Map a flat character index to a line/column position
        struct Cursor
        {
            int line = 0;
            int column = 0;
        };

        Cursor GetCursor(int charIndex) const;
    };

    namespace TextUtil
    {
        // Measure a string's pixel width and height
        void Measure(TTF_Font *font, const char *text, int *w, int *h);

        // Word-wrap text to fit within maxWidth pixels.
        // Breaks on spaces; if a single word exceeds maxWidth, breaks at character level.
        std::vector<std::string> WrapText(TTF_Font *font, const std::string &text, int maxWidth);

        // Truncate text to fit within maxWidth, appending "..." if truncated
        std::string Ellipsize(TTF_Font *font, const std::string &text, int maxWidth);

        // Build a full TextLayout from text and a max pixel width
        TextLayout BuildLayout(TTF_Font *font, const std::string &text, int maxWidth);
    }
} // namespace Gin
