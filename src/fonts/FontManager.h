#pragma once

#include "FontAwesome6Solid900.h"
#include "IconsFontAwesome6.h"
#include "JetBrainsMonoReg.h"
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
};