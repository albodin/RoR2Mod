#include "RenderUtils.h"
#include "fonts/FontManager.h"
#include "hooks/hooks.h"
#include <cstdarg>
#include <cstdio>

namespace RenderUtils {
    bool WorldToScreen(Camera* camera, Vector3& worldPos, ImVec2& screenPos) {
        Vector3 screenPoint;
        Hooks::Camera_WorldToScreenPoint_Injected(camera, &worldPos, MonoOrStereoscopicEye::Mono, &screenPoint);

        if (screenPoint.z < 0) {
            return false;
        }

        if (screenPoint.x < 0 || screenPoint.x > ImGui::GetIO().DisplaySize.x ||
            screenPoint.y < 0 || screenPoint.y > ImGui::GetIO().DisplaySize.y) {
            return false;
        }

        screenPos.x = screenPoint.x;
        screenPos.y = ImGui::GetIO().DisplaySize.y - screenPoint.y;

        return true;
    }

    ImVec2 RenderText(ImVec2 pos, ImU32 color, ImU32 shadowColor, bool shadow, bool centered, const char* text, ...) {
        va_list args;
        va_start(args, text);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), text, args);
        va_end(args);

        ImFont* font = FontManager::GetESPFont();
        float scale = FontManager::ESPFontSize / font->FontSize;

        ImVec2 textSize = ImGui::CalcTextSize(buffer);
        textSize.x *= scale;
        textSize.y *= scale;
        if (centered) {
            pos.x -= textSize.x / 2;
        }

        if (shadow) {
            float offset = 1.0f * scale;
            ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize,
                ImVec2(pos.x + offset, pos.y + offset), shadowColor, buffer);
            ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize,
                ImVec2(pos.x - offset, pos.y - offset), shadowColor, buffer);
            ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize,
                ImVec2(pos.x + offset, pos.y - offset), shadowColor, buffer);
            ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize,
                ImVec2(pos.x - offset, pos.y + offset), shadowColor, buffer);
        }

        ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, pos, color, buffer);

        return textSize;
    }

    void RenderBox(ImVec2 pos, ImVec2 size, ImU32 color, float thickness) {
        ImGui::GetBackgroundDrawList()->AddRect(
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            color,
            0.0f, 0, thickness
        );
    }

    void RenderLine(ImVec2 start, ImVec2 end, ImU32 color, float thickness) {
        ImGui::GetBackgroundDrawList()->AddLine(start, end, color, thickness);
    }

    void RenderCircle(ImVec2 center, float radius, ImU32 color, int segments, float thickness) {
        ImGui::GetBackgroundDrawList()->AddCircle(center, radius, color, segments, thickness);
    }
}