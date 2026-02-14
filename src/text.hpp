#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

namespace Gin
{
    namespace TextUtil
    {
        // Measure a string's pixel width and height
        void Measure(TTF_Font *font, const char *text, int *w, int *h);

        // Word-wrap text to fit within maxWidth pixels.
        // Breaks on spaces; if a single word exceeds maxWidth, breaks at character level.
        std::vector<std::string> WrapText(TTF_Font *font, const std::string &text, int maxWidth);

        // Truncate text to fit within maxWidth, appending "..." if truncated
        std::string Ellipsize(TTF_Font *font, const std::string &text, int maxWidth);
    }
} // namespace Gin
