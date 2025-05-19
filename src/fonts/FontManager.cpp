#include "FontManager.h"
#include "globals/globals.h"
#include <filesystem>

static float BaseFontSize = 15.0f;

ImFont* FontManager::JetBrainsMono = nullptr;
ImFont* FontManager::FontAwesomeSolid = nullptr;
ImFont* FontManager::DefaultFont = nullptr;

float FontManager::ESPFontSize = 15.0f;
int FontManager::CurrentFontIndex = 0;

std::vector<FontManager::FontInfo> FontManager::AvailableFonts;

void FontManager::InitializeFonts(ImFontAtlas* atlas) {
    atlas->Clear();

    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.OversampleH = 2;
    config.OversampleV = 2;
    config.PixelSnapH = true;

    DefaultFont = atlas->AddFontDefault();
    FontInfo defaultFontInfo = {"Default Font", "", DefaultFont};
    AvailableFonts.push_back(defaultFontInfo);

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;

    FontAwesomeSolid = atlas->AddFontFromMemoryCompressedTTF(
        FontAwesome6Solid900_compressed_data,
        FontAwesome6Solid900_compressed_size,
        BaseFontSize, &icons_config, icons_ranges);

    JetBrainsMono = atlas->AddFontFromMemoryCompressedTTF(
        JetBrainsMonoReg_compressed_data,
        JetBrainsMonoReg_compressed_size,
        BaseFontSize, &config);
    FontInfo jetBrainsMonoInfo = {"JetBrains Mono", "", JetBrainsMono};
    AvailableFonts.push_back(jetBrainsMonoInfo);

    LoadCustomFonts(atlas);

    atlas->Build();

    // Default to JetBrains Mono
    CurrentFontIndex = 1;
}

void FontManager::LoadCustomFonts(ImFontAtlas* atlas) {
    std::string fontDir = "ror2modfonts";

    if (!std::filesystem::exists(fontDir)) {
        try {
            std::filesystem::create_directory(fontDir);
            G::logger.LogInfo("Created fonts directory: %s", fontDir.c_str());
        } catch (const std::exception& e) {
            G::logger.LogError("Failed to create fonts directory: %s", e.what());
            return;
        }
    }

    std::vector<std::string> fontExtensions = {".ttf", ".otf"};

    ImFontConfig config;
    config.FontDataOwnedByAtlas = true;
    config.OversampleH = 2;
    config.OversampleV = 2;
    config.PixelSnapH = true;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(fontDir)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (std::find(fontExtensions.begin(), fontExtensions.end(), ext) != fontExtensions.end()) {
                std::string fontPath = entry.path().string();
                std::string fontName = entry.path().filename().string();

                size_t extPos = fontName.find_last_of('.');
                if (extPos != std::string::npos) {
                    fontName = fontName.substr(0, extPos);
                }

                ImFont* font = atlas->AddFontFromFileTTF(fontPath.c_str(), BaseFontSize, &config);

                if (font) {
                    FontInfo fontInfo = {fontName, fontPath, font};
                    AvailableFonts.push_back(fontInfo);
                    G::logger.LogInfo("Loaded font: %s", fontName.c_str());
                } else {
                    G::logger.LogError("Failed to load font: %s", fontPath.c_str());
                }
            }
        }
    } catch (const std::exception& e) {
        G::logger.LogError("Error scanning font directory: %s", e.what());
    }

    G::logger.LogInfo("Loaded %zu fonts total", AvailableFonts.size());
}

ImFont* FontManager::GetESPFont() {
    if (CurrentFontIndex >= 0 && CurrentFontIndex < AvailableFonts.size()) {
        return AvailableFonts[CurrentFontIndex].font;
    }
    return DefaultFont;
}