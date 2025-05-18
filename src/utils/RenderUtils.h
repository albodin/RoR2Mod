#pragma once
#include <imgui.h>
#include "Math.h"
#include "game/GameStructs.h"

namespace RenderUtils {
    bool WorldToScreen(Camera* camera, Vector3& worldPos, ImVec2& screenPos);
    ImVec2 RenderText(ImVec2 pos, ImU32 color, ImU32 shadowColor, bool shadow, bool centered, const char* text, ...);
    void RenderBox(ImVec2 pos, ImVec2 size, ImU32 color, float thickness = 1.0f);
    void RenderLine(ImVec2 start, ImVec2 end, ImU32 color, float thickness = 1.0f);
    void RenderCircle(ImVec2 center, float radius, ImU32 color, int segments = 12, float thickness = 1.0f);
}