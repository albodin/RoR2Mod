#include "NotificationManager.hpp"
#include "fonts/FontManager.hpp"
#include "globals/globals.hpp"

namespace {
constexpr float MARGIN = 10.0f;
constexpr float PADDING_X = 5.0f;
constexpr float PADDING_Y = 5.0f;
constexpr float SPACING = 5.0f;
constexpr float CORNER_RADIUS = 5.0f;
constexpr float PROGRESS_BAR_HEIGHT = 2.0f;
constexpr int PROGRESS_ALPHA = 100;
} // namespace

std::deque<Notification> NotificationManager::Notifications;
bool NotificationManager::Initialized = false;
int NotificationManager::FontIndex = 0; // Default to first font
float NotificationManager::FontSize = 16.0f;

std::unique_ptr<ToggleControl> NotificationManager::EnabledControl = std::make_unique<ToggleControl>("Enable Notifications", "notification_enabled");
std::unique_ptr<ComboControl> NotificationManager::PositionControl = std::make_unique<ComboControl>(
    "Position", "notification_position", std::vector<std::string>{"Top Left", "Top Middle", "Top Right", "Bottom Left", "Bottom Middle", "Bottom Right"},
    static_cast<int>(NotificationPosition::TopRight), false);
std::unique_ptr<FloatControl> NotificationManager::DurationControl =
    std::make_unique<FloatControl>("Duration", "notification_duration", 3.0f, 0.1f, 60.0f, 0.1f, false, false, false);
std::unique_ptr<IntControl> NotificationManager::MaxNotificationsControl =
    std::make_unique<IntControl>("Max Notifications", "notification_max_count", 5, 1, 100, 1, false, false, false);
std::unique_ptr<ToggleControl> NotificationManager::ShowBackgroundControl =
    std::make_unique<ToggleControl>("Show Background", "notification_show_background", true, true);
ImVec4 NotificationManager::BackgroundColor = ImVec4(20.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 200.0f / 255.0f);
std::unique_ptr<ToggleControl> NotificationManager::ShowProgressBarControl =
    std::make_unique<ToggleControl>("Show Progress Bar", "notification_show_progress", true, true);
std::unique_ptr<ToggleControl> NotificationManager::ShowTextShadowControl =
    std::make_unique<ToggleControl>("Show Text Shadow", "notification_show_shadow", true, true);
std::unique_ptr<FloatControl> NotificationManager::FadeInTimeControl =
    std::make_unique<FloatControl>("Fade In Time", "notification_fade_in", 0.5f, 0.0f, 2.0f, 0.1f, false, false, false);
std::unique_ptr<FloatControl> NotificationManager::FadeOutTimeControl =
    std::make_unique<FloatControl>("Fade Out Time", "notification_fade_out", 0.5f, 0.0f, 2.0f, 0.1f, false, false, false);
std::unique_ptr<FloatControl> NotificationManager::AnimationSpeedControl =
    std::make_unique<FloatControl>("Animation Speed", "notification_anim_speed", 10.0f, 1.0f, 20.0f, 0.5f, false, false, false);
std::unique_ptr<FloatControl> NotificationManager::SlideDistanceControl =
    std::make_unique<FloatControl>("Slide Distance", "notification_slide_dist", 50.0f, 0.0f, 200.0f, 5.0f, false, false, false);

ImVec4 NotificationManager::EnableColor = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);   // Green
ImVec4 NotificationManager::DisableColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);  // Red
ImVec4 NotificationManager::ActionColor = ImVec4(0.4f, 0.6f, 1.0f, 1.0f);   // Blue
ImVec4 NotificationManager::IncreaseColor = ImVec4(0.4f, 1.0f, 1.0f, 1.0f); // Cyan
ImVec4 NotificationManager::DecreaseColor = ImVec4(1.0f, 0.6f, 0.2f, 1.0f); // Orange
ImVec4 NotificationManager::ShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);   // Black

namespace {
float CalculateFadeAlpha(float elapsed, float duration, float fadeInTime, float fadeOutTime) {
    if (fadeInTime > 0 && elapsed < fadeInTime) {
        return elapsed / fadeInTime;
    }

    float fadeOutStart = duration - fadeOutTime;
    if (fadeOutTime > 0 && elapsed > fadeOutStart) {
        float fadeProgress = (elapsed - fadeOutStart) / fadeOutTime;
        return std::max(0.0f, 1.0f - fadeProgress);
    }

    return 1.0f;
}

bool IsTopPosition(NotificationPosition pos) {
    return pos == NotificationPosition::TopLeft || pos == NotificationPosition::TopMiddle || pos == NotificationPosition::TopRight;
}

float CalculateXPosition(NotificationPosition pos, float screenWidth, float notifWidth) {
    switch (pos) {
    case NotificationPosition::TopLeft:
    case NotificationPosition::BottomLeft:
        return MARGIN;

    case NotificationPosition::TopMiddle:
    case NotificationPosition::BottomMiddle:
        return (screenWidth - notifWidth) / 2.0f;

    case NotificationPosition::TopRight:
    case NotificationPosition::BottomRight:
        return screenWidth - notifWidth - MARGIN;

    default:
        return MARGIN;
    }
}
} // namespace

void NotificationManager::Initialize() {
    if (Initialized)
        return;

    Initialized = true;
    Notifications.clear();
}

void NotificationManager::Render() {
    if (!Initialized || !EnabledControl->IsEnabled() || Notifications.empty()) {
        return;
    }

    float currentTime = ImGui::GetTime();
    RemoveExpiredNotifications(currentTime);

    if (Notifications.empty())
        return;

    ImFont* font = GetSelectedFont();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    NotificationPosition position = static_cast<NotificationPosition>(PositionControl->GetSelectedIndex());

    float notifHeight = FontSize + PADDING_Y * 2;
    bool isTop = IsTopPosition(position);
    float y = isTop ? MARGIN : screenSize.y - MARGIN - notifHeight;

    auto* drawList = ImGui::GetBackgroundDrawList();

    for (auto& notif : Notifications) {
        float elapsed = currentTime - notif.creationTime;

        notif.alpha = CalculateFadeAlpha(elapsed, notif.duration, FadeInTimeControl->GetValue(), FadeOutTimeControl->GetValue());
        notif.targetY = y;

        // Initialize position on first frame
        if (!notif.initialized) {
            float slideDistance = SlideDistanceControl->GetValue();
            if (slideDistance > 0) {
                notif.currentY = isTop ? (y - slideDistance) : (y + slideDistance);
            } else {
                notif.currentY = y;
            }
            notif.initialized = true;
        }

        notif.currentY += (notif.targetY - notif.currentY) * AnimationSpeedControl->GetValue() * ImGui::GetIO().DeltaTime;

        float notifWidth = CalculateNotificationWidth(notif.text, font);
        float x = CalculateXPosition(position, screenSize.x, notifWidth);

        if (ShowBackgroundControl->IsEnabled()) {
            DrawNotificationBackground(drawList, x, notif.currentY, notifWidth, notifHeight, notif.alpha);
        }

        if (ShowProgressBarControl->IsEnabled()) {
            DrawProgressBar(drawList, x, notif.currentY, notifWidth, notifHeight, elapsed / notif.duration, notif.alpha);
        }

        DrawNotificationText(drawList, font, x, notif.currentY, notifHeight, notif.text, notif.type, notif.alpha);

        // Update y for next notification
        y += isTop ? (notifHeight + SPACING) : -(notifHeight + SPACING);
    }
}

ImU32 NotificationManager::GetColorForNotificationType(NotificationType type, float alpha) {
    ImVec4 color;
    switch (type) {
    case NotificationType::Enable:
        color = EnableColor;
        break;
    case NotificationType::Disable:
        color = DisableColor;
        break;
    case NotificationType::Action:
        color = ActionColor;
        break;
    case NotificationType::Increase:
        color = IncreaseColor;
        break;
    case NotificationType::Decrease:
        color = DecreaseColor;
        break;
    default:
        color = ActionColor;
        break;
    }

    return IM_COL32((int)(color.x * 255), (int)(color.y * 255), (int)(color.z * 255), (int)(color.w * 255 * alpha));
}

void NotificationManager::RemoveExpiredNotifications(float currentTime) {
    Notifications.erase(std::remove_if(Notifications.begin(), Notifications.end(),
                                       [currentTime](const Notification& notif) { return (currentTime - notif.creationTime) > notif.duration; }),
                        Notifications.end());
}

ImFont* NotificationManager::GetSelectedFont() {
    if (FontIndex >= 0 && FontIndex < static_cast<int>(FontManager::AvailableFonts.size())) {
        return FontManager::AvailableFonts[FontIndex].font;
    }
    return FontManager::DefaultFont;
}

float NotificationManager::CalculateNotificationWidth(const std::string& text, ImFont* font) {
    ImVec2 textSize;

    float scale = FontSize / font->FontSize;

    ImGui::PushFont(font);
    textSize = ImGui::CalcTextSize(text.c_str());
    ImGui::PopFont();

    textSize.x *= scale;
    textSize.y *= scale;

    float padding = ShowBackgroundControl->IsEnabled() ? (PADDING_X * 2) : 0;
    float width = textSize.x + padding;
    constexpr float MIN_WIDTH = 200.0f;
    constexpr float MAX_WIDTH = 600.0f;

    return std::clamp(width, MIN_WIDTH, MAX_WIDTH);
}

void NotificationManager::DrawNotificationBackground(ImDrawList* drawList, float x, float y, float width, float height, float alpha) {
    ImU32 bgColor =
        IM_COL32((int)(BackgroundColor.x * 255), (int)(BackgroundColor.y * 255), (int)(BackgroundColor.z * 255), (int)(BackgroundColor.w * 255 * alpha));
    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), bgColor, CORNER_RADIUS);
}

void NotificationManager::DrawProgressBar(ImDrawList* drawList, float x, float y, float width, float height, float progress, float alpha) {
    ImU32 progressColor = IM_COL32(100, 100, 100, (int)(PROGRESS_ALPHA * alpha));
    float barY = y + height - PROGRESS_BAR_HEIGHT;

    drawList->AddRectFilled(ImVec2(x, barY), ImVec2(x + width * (1.0f - progress), y + height), progressColor);
}

void NotificationManager::DrawNotificationText(ImDrawList* drawList, ImFont* font, float x, float y, float height, const std::string& text,
                                               NotificationType type, float alpha) {
    ImU32 textColor = GetColorForNotificationType(type, alpha);

    float textX = ShowBackgroundControl->IsEnabled() ? (x + PADDING_X) : x;
    ImVec2 textPos(textX, y + (height - FontSize) / 2);

    if (ShowTextShadowControl->IsEnabled()) {
        ImU32 shadowColorU32 = IM_COL32((int)(ShadowColor.x * 255), (int)(ShadowColor.y * 255), (int)(ShadowColor.z * 255), (int)(ShadowColor.w * 255 * alpha));
        static const float shadowOffset = 1.0f;

        drawList->AddText(font, FontSize, ImVec2(textPos.x + shadowOffset, textPos.y + shadowOffset), shadowColorU32, text.c_str());
        drawList->AddText(font, FontSize, ImVec2(textPos.x - shadowOffset, textPos.y - shadowOffset), shadowColorU32, text.c_str());
        drawList->AddText(font, FontSize, ImVec2(textPos.x + shadowOffset, textPos.y - shadowOffset), shadowColorU32, text.c_str());
        drawList->AddText(font, FontSize, ImVec2(textPos.x - shadowOffset, textPos.y + shadowOffset), shadowColorU32, text.c_str());
    }

    drawList->AddText(font, FontSize, textPos, textColor, text.c_str());
}

void NotificationManager::AddNotification(const std::string& text, NotificationType type) {
    if (!Initialized || !EnabledControl->IsEnabled())
        return;

    Notification notif;
    notif.text = text;
    notif.type = type;
    notif.creationTime = ImGui::GetTime();
    notif.duration = DurationControl->GetValue();
    notif.alpha = 0.0f;
    notif.initialized = false;
    // currentY and targetY will be set in Render on first frame

    Notifications.push_back(notif);
    EnforceMaxNotifications();
}

void NotificationManager::EnforceMaxNotifications() {
    size_t maxCount = static_cast<size_t>(MaxNotificationsControl->GetValue());
    while (Notifications.size() > maxCount) {
        Notifications.pop_front();
    }
}

void NotificationManager::ClearNotifications() { Notifications.clear(); }

void NotificationManager::DrawControls() {
    ImGui::TextWrapped("Notifications will appear in the selected position and fade out after the specified duration.");

    EnabledControl->Draw();
    PositionControl->Draw();
    DurationControl->Draw();
    MaxNotificationsControl->Draw();

    ImGui::Separator();
    ImGui::Text("Visual Settings");

    ShowBackgroundControl->Draw();
    if (ShowBackgroundControl->IsEnabled()) {
        ImGui::SameLine();
        ImGui::ColorEdit4("##notification_bg_color", (float*)&BackgroundColor,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
    }
    ShowProgressBarControl->Draw();

    ShowTextShadowControl->Draw();
    if (ShowTextShadowControl->IsEnabled()) {
        ImGui::SameLine();
        ImGui::ColorEdit4("##notification_shadow_color", (float*)&ShadowColor,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
    }

    if (ImGui::BeginCombo("Font##notification", FontManager::AvailableFonts[FontIndex].name.c_str())) {
        for (int i = 0; i < FontManager::AvailableFonts.size(); i++) {
            const bool is_selected = (FontIndex == i);
            if (ImGui::Selectable(FontManager::AvailableFonts[i].name.c_str(), is_selected)) {
                FontIndex = i;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SliderFloat("Font Size##notification", &FontSize, 1.0f, 100.0f, "%.1f");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Notification Type Colors")) {
        ImGui::Text("Enable:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##notif_enable_color", (float*)&EnableColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::SameLine();
        ImGui::Text("Disable:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##notif_disable_color", (float*)&DisableColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::SameLine();
        ImGui::Text("Action:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##notif_action_color", (float*)&ActionColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::Text("Increase:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##notif_increase_color", (float*)&IncreaseColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::SameLine();
        ImGui::Text("Decrease:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##notif_decrease_color", (float*)&DecreaseColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }

    ImGui::Separator();
    ImGui::Text("Animation Settings");

    FadeInTimeControl->Draw();
    FadeOutTimeControl->Draw();
    AnimationSpeedControl->Draw();
    SlideDistanceControl->Draw();

    ImGui::Separator();
    ImGui::Text("Test Notifications:");
    if (ImGui::Button("Test Enable")) {
        AddNotification("Feature Enabled", NotificationType::Enable);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Disable")) {
        AddNotification("Feature Disabled", NotificationType::Disable);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Action")) {
        AddNotification("Action Performed", NotificationType::Action);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Increase")) {
        AddNotification("Value Increased", NotificationType::Increase);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Decrease")) {
        AddNotification("Value Decreased", NotificationType::Decrease);
    }
}

json NotificationManager::Serialize() {
    json data;

    data["EnableColor"] = Vec4ToJson(EnableColor);
    data["DisableColor"] = Vec4ToJson(DisableColor);
    data["ActionColor"] = Vec4ToJson(ActionColor);
    data["IncreaseColor"] = Vec4ToJson(IncreaseColor);
    data["DecreaseColor"] = Vec4ToJson(DecreaseColor);
    data["ShadowColor"] = Vec4ToJson(ShadowColor);
    data["BackgroundColor"] = Vec4ToJson(BackgroundColor);

    data["FontIndex"] = FontIndex;
    data["FontSize"] = FontSize;

    return data;
}

void NotificationManager::Deserialize(const json& data) {
    if (data.contains("EnableColor"))
        EnableColor = JsonToVec4(data["EnableColor"], EnableColor);
    if (data.contains("DisableColor"))
        DisableColor = JsonToVec4(data["DisableColor"], DisableColor);
    if (data.contains("ActionColor"))
        ActionColor = JsonToVec4(data["ActionColor"], ActionColor);
    if (data.contains("IncreaseColor"))
        IncreaseColor = JsonToVec4(data["IncreaseColor"], IncreaseColor);
    if (data.contains("DecreaseColor"))
        DecreaseColor = JsonToVec4(data["DecreaseColor"], DecreaseColor);
    if (data.contains("ShadowColor"))
        ShadowColor = JsonToVec4(data["ShadowColor"], ShadowColor);
    if (data.contains("BackgroundColor"))
        BackgroundColor = JsonToVec4(data["BackgroundColor"], BackgroundColor);

    if (data.contains("FontIndex") && data["FontIndex"].is_number()) {
        FontIndex = data["FontIndex"];
    }
    if (data.contains("FontSize") && data["FontSize"].is_number()) {
        FontSize = data["FontSize"];
    }
}
