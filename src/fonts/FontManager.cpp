#include "FontManager.h"
#include "globals/globals.h"

void FontManager::InitializeFonts(ImFontAtlas* atlas) {
    atlas->Clear();
    
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.OversampleH = 2;
    config.OversampleV = 2;
    config.PixelSnapH = true;
    
    atlas->AddFontFromMemoryCompressedTTF(
        JetBrainsMonoReg_compressed_data,
        JetBrainsMonoReg_compressed_size,
        15.0f, &config);


    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;
    
    atlas->AddFontFromMemoryCompressedTTF(
        FontAwesome6Solid900_compressed_data,
        FontAwesome6Solid900_compressed_size,
        15.0f, &icons_config, icons_ranges);
    
    atlas->Build();
}