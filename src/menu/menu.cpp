#include "menu.h"
#include "globals/globals.h"
#include <imgui.h>

ImVec2 RenderText(ImVec2 pos, ImU32 color, ImU32 shadowColor, bool centered, const char* text, ...) {
    va_list args;
    va_start(args, text);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);

    ImVec2 textSize = ImGui::CalcTextSize(buffer);
    if (centered) {
        pos.x -= textSize.x / 2;
    }

    ImGui::GetBackgroundDrawList()->AddText(pos, shadowColor, buffer);

    return textSize;
}

void DrawPlayerTab() {
    ImGui::Checkbox("Godmode", &G::godMode);
    ImGui::InputFloat("Base Move Speed", &G::baseMoveSpeed, 1.0f, 100.0f);
    ImGui::InputFloat("Base Damage", &G::baseDamage, 10.0f, 1000000000.0f);
    ImGui::InputFloat("Base Attack Speed", &G::baseAttackSpeed, 10.0f, 1000000000.0f);
    ImGui::InputFloat("Base Crit", &G::baseCrit, 10.0f, 1000000000.0f);
    ImGui::InputInt("Base Jump Count", &G::baseJumpCount);
}

void DrawESPTab() {

}

void DrawAimbotTab() {

}

void DrawEnemiesTab() {

}

void DrawConfigTab() {

}

void DrawMenu() {
    ImGui::Begin("Risk of Rain 2 Mod");

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_DrawSelectedOverline)) {
        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
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