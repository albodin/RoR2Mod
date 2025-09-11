#include "RenderUtils.hpp"
#include "fonts/FontManager.hpp"
#include "hooks/hooks.hpp"
#include <cstdarg>
#include <cstdio>

namespace RenderUtils {
static CachedCameraData g_cameraCache;
void PrecomputeViewProjection(Camera* camera) {
    if (!Hooks::Camera_get_worldToCameraMatrix_Injected || !Hooks::Camera_get_projectionMatrix_Injected || !camera) {
        return;
    }
    Matrix4x4 view, proj;
    Hooks::Camera_get_worldToCameraMatrix_Injected(camera, &view);
    Hooks::Camera_get_projectionMatrix_Injected(camera, &proj);

    auto& displaySize = ImGui::GetIO().DisplaySize;
    g_cameraCache.displayWidth = displaySize.x;
    g_cameraCache.displayHeight = displaySize.y;
    g_cameraCache.halfViewportX = displaySize.x * 0.5f;
    g_cameraCache.halfViewportY = displaySize.y * 0.5f;

    g_cameraCache.viewProj.m00 = proj.m00 * view.m00 + proj.m10 * view.m01 + proj.m20 * view.m02 + proj.m30 * view.m03;
    g_cameraCache.viewProj.m01 = proj.m01 * view.m00 + proj.m11 * view.m01 + proj.m21 * view.m02 + proj.m31 * view.m03;
    g_cameraCache.viewProj.m02 = proj.m02 * view.m00 + proj.m12 * view.m01 + proj.m22 * view.m02 + proj.m32 * view.m03;
    g_cameraCache.viewProj.m03 = proj.m03 * view.m00 + proj.m13 * view.m01 + proj.m23 * view.m02 + proj.m33 * view.m03;

    g_cameraCache.viewProj.m10 = proj.m00 * view.m10 + proj.m10 * view.m11 + proj.m20 * view.m12 + proj.m30 * view.m13;
    g_cameraCache.viewProj.m11 = proj.m01 * view.m10 + proj.m11 * view.m11 + proj.m21 * view.m12 + proj.m31 * view.m13;
    g_cameraCache.viewProj.m12 = proj.m02 * view.m10 + proj.m12 * view.m11 + proj.m22 * view.m12 + proj.m32 * view.m13;
    g_cameraCache.viewProj.m13 = proj.m03 * view.m10 + proj.m13 * view.m11 + proj.m23 * view.m12 + proj.m33 * view.m13;

    g_cameraCache.viewProj.m20 = proj.m00 * view.m20 + proj.m10 * view.m21 + proj.m20 * view.m22 + proj.m30 * view.m23;
    g_cameraCache.viewProj.m21 = proj.m01 * view.m20 + proj.m11 * view.m21 + proj.m21 * view.m22 + proj.m31 * view.m23;
    g_cameraCache.viewProj.m22 = proj.m02 * view.m20 + proj.m12 * view.m21 + proj.m22 * view.m22 + proj.m32 * view.m23;
    g_cameraCache.viewProj.m23 = proj.m03 * view.m20 + proj.m13 * view.m21 + proj.m23 * view.m22 + proj.m33 * view.m23;

    g_cameraCache.viewProj.m30 = proj.m00 * view.m30 + proj.m10 * view.m31 + proj.m20 * view.m32 + proj.m30 * view.m33;
    g_cameraCache.viewProj.m31 = proj.m01 * view.m30 + proj.m11 * view.m31 + proj.m21 * view.m32 + proj.m31 * view.m33;
    g_cameraCache.viewProj.m32 = proj.m02 * view.m30 + proj.m12 * view.m31 + proj.m22 * view.m32 + proj.m32 * view.m33;
    g_cameraCache.viewProj.m33 = proj.m03 * view.m30 + proj.m13 * view.m31 + proj.m23 * view.m32 + proj.m33 * view.m33;
}

bool WorldToScreen(const Vector3& worldPos, ImVec2& screenPos, bool& onScreen) {
    __m128 world = _mm_set_ps(1.0f, worldPos.z, worldPos.y, worldPos.x);

    __m128 col0 = _mm_load_ps(&g_cameraCache.viewProj.m16[0]);
    __m128 col1 = _mm_load_ps(&g_cameraCache.viewProj.m16[4]);
    __m128 col2 = _mm_load_ps(&g_cameraCache.viewProj.m16[8]);
    __m128 col3 = _mm_load_ps(&g_cameraCache.viewProj.m16[12]);

    __m128 x = _mm_shuffle_ps(world, world, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 y = _mm_shuffle_ps(world, world, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 z = _mm_shuffle_ps(world, world, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 w = _mm_shuffle_ps(world, world, _MM_SHUFFLE(3, 3, 3, 3));

    __m128 clipPos = _mm_add_ps(_mm_add_ps(_mm_mul_ps(col0, x), _mm_mul_ps(col1, y)), _mm_add_ps(_mm_mul_ps(col2, z), _mm_mul_ps(col3, w)));
    alignas(16) float clip[4];
    _mm_store_ps(clip, clipPos);

    if (clip[2] < 0) {
        onScreen = false;
        return false;
    }

    const float inv_w = 1.0f / clip[3];
    screenPos.x = (clip[0] * inv_w * 0.5f + 0.5f) * g_cameraCache.displayWidth;
    screenPos.y = (1.0f - (clip[1] * inv_w * 0.5f + 0.5f)) * g_cameraCache.displayHeight;

    onScreen = (screenPos.x >= 0 && screenPos.x <= g_cameraCache.displayWidth && screenPos.y >= 0 && screenPos.y <= g_cameraCache.displayHeight);

    return true;
}

bool WorldToScreen(const Vector3& worldPos, ImVec2& screenPos) {
    bool onScreen = false;
    return WorldToScreen(worldPos, screenPos, onScreen);
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
        ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, ImVec2(pos.x + offset, pos.y + offset), shadowColor, buffer);
        ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, ImVec2(pos.x - offset, pos.y - offset), shadowColor, buffer);
        ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, ImVec2(pos.x + offset, pos.y - offset), shadowColor, buffer);
        ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, ImVec2(pos.x - offset, pos.y + offset), shadowColor, buffer);
    }

    ImGui::GetBackgroundDrawList()->AddText(font, FontManager::ESPFontSize, pos, color, buffer);

    return textSize;
}

void RenderBox(ImVec2 pos, ImVec2 size, ImU32 color, float thickness) {
    ImGui::GetBackgroundDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), color, 0.0f, 0, thickness);
}

void RenderLine(ImVec2 start, ImVec2 end, ImU32 color, float thickness) { ImGui::GetBackgroundDrawList()->AddLine(start, end, color, thickness); }

void RenderCircle(ImVec2 center, float radius, ImU32 color, int segments, float thickness) {
    ImGui::GetBackgroundDrawList()->AddCircle(center, radius, color, segments, thickness);
}

void RenderHealthbar(ImVec2 pos, ImVec2 size, float health, float maxHealth, ImU32 fillColor, ImU32 bgColor) {
    if (maxHealth <= 0)
        return;

    float healthPercent = std::min(health / maxHealth, 1.0f);

    // Background
    ImGui::GetBackgroundDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor);

    float fillHeight = size.y * healthPercent;
    ImVec2 fillStart(pos.x, pos.y + size.y - fillHeight);
    ImVec2 fillEnd(pos.x + size.x, pos.y + size.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(fillStart, fillEnd, fillColor);

    // Border
    ImGui::GetBackgroundDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(0, 0, 0, 255));
}

} // namespace RenderUtils
