#pragma once
#include "InputControls.hpp"
#include "imgui/imgui.h"
#include <deque>
#include <memory>
#include <string>

enum class NotificationType {
    Enable,
    Disable,
    Action,
    Increase,
    Decrease,
};

struct Notification {
    std::string text;
    NotificationType type;
    float creationTime;
    float duration;
    float currentY;
    float targetY;
    float alpha;
    bool initialized = false;
};

enum class NotificationPosition { TopLeft, TopMiddle, TopRight, BottomLeft, BottomMiddle, BottomRight };

class NotificationManager {
  private:
    static std::deque<Notification> Notifications;
    static bool Initialized;
    static int FontIndex;
    static float FontSize;

    static std::unique_ptr<ToggleControl> EnabledControl;
    static std::unique_ptr<ComboControl> PositionControl;
    static std::unique_ptr<FloatControl> DurationControl;
    static std::unique_ptr<IntControl> MaxNotificationsControl;

    static std::unique_ptr<ToggleControl> ShowBackgroundControl;
    static std::unique_ptr<ToggleControl> ShowProgressBarControl;
    static std::unique_ptr<ToggleControl> ShowTextShadowControl;

    static ImVec4 BackgroundColor;
    static ImVec4 EnableColor;
    static ImVec4 DisableColor;
    static ImVec4 ActionColor;
    static ImVec4 IncreaseColor;
    static ImVec4 DecreaseColor;
    static ImVec4 ShadowColor;

    static std::unique_ptr<FloatControl> FadeInTimeControl;
    static std::unique_ptr<FloatControl> FadeOutTimeControl;
    static std::unique_ptr<FloatControl> AnimationSpeedControl;
    static std::unique_ptr<FloatControl> SlideDistanceControl;

    static void RemoveExpiredNotifications(float currentTime);
    static ImFont* GetSelectedFont();
    static float CalculateNotificationWidth(const std::string& text, ImFont* font);
    static void EnforceMaxNotifications();

    static void DrawNotificationBackground(ImDrawList* drawList, float x, float y, float width, float height, float alpha);
    static void DrawProgressBar(ImDrawList* drawList, float x, float y, float width, float height, float progress, float alpha);
    static void DrawNotificationText(ImDrawList* drawList, ImFont* font, float x, float y, float height, const std::string& text, NotificationType type,
                                     float alpha);
    static ImU32 GetColorForNotificationType(NotificationType type, float alpha);

  public:
    NotificationManager() = delete;

    static void Initialize();
    static void Render();
    static void AddNotification(const std::string& text, NotificationType type = NotificationType::Action);
    static void ClearNotifications();

    static void DrawControls();

    static json Serialize();
    static void Deserialize(const json& data);

    static int GetFontIndex() { return FontIndex; }
    static void SetFontIndex(int index) { FontIndex = index; }
    static float GetFontSize() { return FontSize; }
    static void SetFontSize(float size) { FontSize = size; }
};
