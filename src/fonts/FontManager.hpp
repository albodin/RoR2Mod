#pragma once

#include "FontAwesome6Solid900.hpp"
#include "IconsFontAwesome6.hpp"
#include "JetBrainsMonoReg.hpp"
#include "imgui.h"
#include <string>
#include <vector>

class FontManager {
  public:
    struct FontInfo {
        std::string name;
        std::string path;
        ImFont* font;
    };

    static ImFont* JetBrainsMono;
    static ImFont* FontAwesomeSolid;
    static ImFont* DefaultFont;

    static float ESPFontSize;
    static int CurrentFontIndex;

    static std::vector<FontInfo> AvailableFonts;

    static void InitializeFonts(ImFontAtlas* atlas);
    static void LoadCustomFonts(ImFontAtlas* atlas);
    static ImFont* GetESPFont();
    static const ImWchar* GetGlyphRanges();
    static void SetupUnicodeRanges(ImFontGlyphRangesBuilder& builder);
};
