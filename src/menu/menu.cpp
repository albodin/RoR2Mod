#include "menu.h"
#include "globals/globals.h"
#include "utils/MonoApi.h"
#include "fonts/FontManager.h"
#include <imgui.h>
#include <filesystem>

void DrawPlayerTab() {
    G::localPlayer->DrawUI();
}

void DrawWorldTab() {
    G::worldModule->DrawUI();
}

void DrawESPTab() {
    G::espModule->DrawUI();
}

void DrawAimbotTab() {

}

void DrawEnemiesTab() {

}

void DumpGameToDirectory(std::string directoryName) {
    static bool initialized = false;
    static MonoAPI g_mono;
    if (!initialized) {
        if (g_mono.Initialize()) {
            G::logger.LogInfo("Mono runtime initialized successfully");
            initialized = true;

        } else {
            G::logger.LogError("Failed to initialize Mono API");
        }
    }
    if (initialized) {
        g_mono.DumpAllClassesToStructs(directoryName);
    }
}

void DrawConfigTab() {
    G::showMenuControl->Draw();
    G::runningButtonControl->Draw();

    ImGui::Separator();

    if (ImGui::CollapsingHeader("ESP Font Settings", ImGuiTreeNodeFlags_None)) {
        if (ImGui::BeginCombo("Font", FontManager::AvailableFonts[FontManager::CurrentFontIndex].name.c_str())) {
            for (int i = 0; i < FontManager::AvailableFonts.size(); i++) {
                const bool is_selected = (FontManager::CurrentFontIndex == i);
                if (ImGui::Selectable(FontManager::AvailableFonts[i].name.c_str(), is_selected)) {
                    FontManager::CurrentFontIndex = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SliderFloat("Font Size", &FontManager::ESPFontSize, 1.0f, 100.0f, "%.1fx");

        ImGui::TextWrapped("Place .ttf or .otf font files in the 'ror2modfonts' folder to add more fonts.");

        ImGui::Text("Preview:");
        ImGui::PushFont(FontManager::GetESPFont());
        float scale = FontManager::ESPFontSize / FontManager::GetESPFont()->FontSize;
        ImGui::SetWindowFontScale(scale);
        ImGui::Text("This is how ESP text will appear");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Dump Game", ImGuiTreeNodeFlags_None)) {
        static char directoryName[256] = "gameDump";
        static bool showSuccessMessage = false;
        static bool showErrorMessage = false;
        static std::string statusMessage;
        static float messageTimer = 0.0f;

        ImGui::Text("Output directory name (relative path):");
        ImGui::InputText("##dirInput", directoryName, IM_ARRAYSIZE(directoryName));

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted("Enter a directory name for output files.\nDirectory must be empty or non-existent.");
            ImGui::EndTooltip();
        }

        if (ImGui::Button("Dump Game to C++")) {
            if (strlen(directoryName) > 0) {
                try {
                    std::filesystem::path dirPath(directoryName);

                    if (!std::filesystem::exists(dirPath)) {
                        if (std::filesystem::create_directories(dirPath)) {
                            DumpGameToDirectory(directoryName);
                            showSuccessMessage = true;
                            showErrorMessage = false;
                            messageTimer = 15.0f;
                            statusMessage = "Game data dumped to '" + std::string(directoryName) + "'";
                        } else {
                            // Failed to create directory
                            showErrorMessage = true;
                            showSuccessMessage = false;
                            messageTimer = 15.0f;
                            statusMessage = "Error: Failed to create directory '" + std::string(directoryName) + "'";
                        }


                    } else {
                        bool isEmpty = true;
                        for (const auto& _ : std::filesystem::directory_iterator(dirPath)) {
                            isEmpty = false;
                            break;
                        }

                        if (isEmpty) {
                            DumpGameToDirectory(directoryName);
                            showSuccessMessage = true;
                            showErrorMessage = false;
                            messageTimer = 15.0f;
                            statusMessage = "Game data dumped to '" + std::string(directoryName) + "'";
                        } else {
                            showErrorMessage = true;
                            showSuccessMessage = false;
                            messageTimer = 15.0f;
                            statusMessage = "Error: Directory '" + std::string(directoryName) +
                                            "' is not empty. Please empty it first.";
                        }
                    }
                }
                catch (const std::filesystem::filesystem_error& e) {
                    showErrorMessage = true;
                    showSuccessMessage = false;
                    messageTimer = 15.0f;
                    statusMessage = "Filesystem error: " + std::string(e.what());
                }
                catch (const std::exception& e) {
                    showErrorMessage = true;
                    showSuccessMessage = false;
                    messageTimer = 15.0f;
                    statusMessage = "Error: " + std::string(e.what());
                }
            }
        }

        // Show status messages with timer
        if (showSuccessMessage || showErrorMessage) {
            messageTimer -= ImGui::GetIO().DeltaTime;
            if (messageTimer > 0.0f) {
                ImVec4 color = showSuccessMessage ?
                    ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :  // Green for success
                    ImVec4(1.0f, 0.0f, 0.0f, 1.0f);   // Red for error
                ImGui::TextColored(color, "%s", statusMessage.c_str());
            }
            else {
                showSuccessMessage = false;
                showErrorMessage = false;
            }
        }
    }

    ImGui::Separator();


}

void DrawMenu() {
    ImGui::Begin("Risk of Rain 2 Mod");

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World")) {
            DrawWorldTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("ESP")) {
            DrawESPTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Aimbot")) {
            DrawAimbotTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Enemies")) {
            DrawEnemiesTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            DrawConfigTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}