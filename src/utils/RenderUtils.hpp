#pragma once
#include "Math.hpp"
#include "game/GameStructs.hpp"
#include <imgui.h>

namespace RenderUtils {
struct CachedCameraData {
    Matrix4x4 viewProj;
    float halfViewportX;
    float halfViewportY;
    float displayWidth;
    float displayHeight;
};

void PrecomputeViewProjection(Camera* camera);
bool WorldToScreen(const Vector3& worldPos, ImVec2& screenPos, bool& onScreen);
bool WorldToScreen(const Vector3& worldPos, ImVec2& screenPos);
ImVec2 RenderText(ImVec2 pos, ImU32 color, ImU32 shadowColor, bool shadow, bool centered, const char* text, ...);
void RenderBox(ImVec2 pos, ImVec2 size, ImU32 color, float thickness = 1.0f);
void RenderLine(ImVec2 start, ImVec2 end, ImU32 color, float thickness = 1.0f);
void RenderCircle(ImVec2 center, float radius, ImU32 color, int segments = 12, float thickness = 1.0f);
void RenderHealthbar(ImVec2 pos, ImVec2 size, float health, float maxHealth, ImU32 fillColor, ImU32 bgColor);
} // namespace RenderUtils
