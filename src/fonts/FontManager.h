#pragma once

#include "FontAwesome6Solid900.h"
#include "IconsFontAwesome6.h"
#include "JetBrainsMonoReg.h"
#include "imgui.h"

class FontManager {
public:
    static ImFont* JetBrainsMono;
    static ImFont* FontAwesomeSolid;
    static ImFont* DefaultFont;
    static void InitializeFonts(ImFontAtlas* atlas);
};