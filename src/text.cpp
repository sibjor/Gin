#include "text.hpp"
#include <sstream>

namespace Gin
{
    namespace TextUtil
    {
        void Measure(TTF_Font *font, const char *text, int *w, int *h)
        {
            if (!font || !text || text[0] == '\0')
            {
                if (w) *w = 0;
                if (h) *h = 0;
                return;
            }
            TTF_GetStringSize(font, text, 0, w, h);
        }

        std::vector<std::string> WrapText(TTF_Font *font, const std::string &text, int maxWidth)
        {
            std::vector<std::string> lines;
            if (!font || text.empty() || maxWidth <= 0)
                return lines;

            std::istringstream stream(text);
            std::string word;
            std::string currentLine;

            while (stream >> word)
            {
                int wordW = 0;
                Measure(font, word.c_str(), &wordW, nullptr);

                // If a single word exceeds maxWidth, break it by character
                if (wordW > maxWidth)
                {
                    if (!currentLine.empty())
                    {
                        lines.push_back(currentLine);
                        currentLine.clear();
                    }

                    std::string part;
                    for (char c : word)
                    {
                        std::string test = part + c;
                        int testW = 0;
                        Measure(font, test.c_str(), &testW, nullptr);
                        if (testW > maxWidth && !part.empty())
                        {
                            lines.push_back(part);
                            part.clear();
                        }
                        part += c;
                    }
                    currentLine = part;
                    continue;
                }

                // Try appending word to current line
                std::string test = currentLine.empty() ? word : currentLine + " " + word;
                int testW = 0;
                Measure(font, test.c_str(), &testW, nullptr);

                if (testW > maxWidth)
                {
                    if (!currentLine.empty())
                        lines.push_back(currentLine);
                    currentLine = word;
                }
                else
                {
                    currentLine = test;
                }
            }

            if (!currentLine.empty())
                lines.push_back(currentLine);

            return lines;
        }

        std::string Ellipsize(TTF_Font *font, const std::string &text, int maxWidth)
        {
            if (!font || text.empty())
                return text;

            int textW = 0;
            Measure(font, text.c_str(), &textW, nullptr);
            if (textW <= maxWidth)
                return text;

            std::string ellipsis = "...";
            int ellipsisW = 0;
            Measure(font, ellipsis.c_str(), &ellipsisW, nullptr);

            int availW = maxWidth - ellipsisW;
            if (availW <= 0)
                return ellipsis;

            for (int i = (int)text.size() - 1; i >= 0; i--)
            {
                std::string sub = text.substr(0, i);
                int subW = 0;
                Measure(font, sub.c_str(), &subW, nullptr);
                if (subW <= availW)
                    return sub + ellipsis;
            }

            return ellipsis;
        }
    }
} // namespace Gin
