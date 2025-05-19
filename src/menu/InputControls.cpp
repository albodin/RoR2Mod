#include "InputControls.h"
#include "globals/globals.h"
#include <algorithm>
#include "fonts/IconsFontAwesome6.h"

// Static key state arrays
static bool s_capturingKey = false;
static ImGuiKey* s_captureTargetKey = nullptr;
static float s_labelWidth = 180.0f;

// InputHelper implementation
bool InputHelper::IsKeyPressed(ImGuiKey key) {
    return ImGui::IsKeyPressed(key);
}

bool InputHelper::IsKeyDown(ImGuiKey key) {
    return ImGui::IsKeyDown(key);
}

const char* InputHelper::KeyToString(ImGuiKey key) {
    const char* name = ImGui::GetKeyName(key);

    if (name && strcmp(name, "Unknown") != 0)
        return name;

    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "Key %d", (int)key);
    return buffer;
}

bool InputHelper::DrawHotkeyButton(const char* id, ImGuiKey* key) {
    ImGui::PushID(id);

    // Determine button text
    const char* buttonText;
    static char buffer[64];

    if (s_capturingKey && s_captureTargetKey == key) {
        buttonText = "Press any key...";
    } else if (*key == ImGuiKey_None) {
        buttonText = "No Hotkey";
    } else {
        snprintf(buffer, sizeof(buffer), "Hotkey: %s", KeyToString(*key));
        buttonText = buffer;
    }

    // Draw the button
    bool clicked = ImGui::Button(buttonText);

    // Handle button click
    if (clicked) {
        s_capturingKey = true;
        s_captureTargetKey = key;
    }

    // Capture key if in capture mode
    if (s_capturingKey && s_captureTargetKey == key) {
        // Check for key presses using ImGui's function
        for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++) {
            if (ImGui::IsKeyPressed(ImGuiKey(i))) {
                if (i == ImGuiKey_Escape) {
                    // Cancel capture on Escape
                    s_capturingKey = false;
                } else {
                    *key = ImGuiKey(i);
                    s_capturingKey = false;
                }
                break;
            }
        }
    }

    ImGui::PopID();
    return s_capturingKey && s_captureTargetKey == key;
}


// InputControl implementation
InputControl::InputControl(const std::string& label, const std::string& id, bool enabled)
    : label(label), id(id), enabled(enabled), hotkey(ImGuiKey_None), isCapturingHotkey(false)
{
}

bool InputControl::DrawHotkeyButton() {
    return InputHelper::DrawHotkeyButton((id + "_hotkey").c_str(), &hotkey);
}


// ToggleControl implementation
ToggleControl::ToggleControl(const std::string& label, const std::string& id, bool enabled)
    : InputControl(label, id, enabled), onChange(nullptr)
{
}

void ToggleControl::Draw() {
    ImGui::PushID(id.c_str());

    bool oldValue = enabled;
    if (ImGui::Checkbox(label.c_str(), &enabled)) {
        if (onChange && oldValue != enabled) {
            onChange(enabled);
        }
    }

    ImGui::SameLine();
    DrawHotkeyButton();

    ImGui::PopID();
}

void ToggleControl::Update() {
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        enabled = !enabled;

        if (onChange) {
            onChange(enabled);
        }
    }
}


// IntControl implementation
IntControl::IntControl(const std::string& label, const std::string& id, int value,
                      int minValue, int maxValue, int step, bool enabled, bool disableValueOnToggle)
    : InputControl(label, id), value(value), minValue(minValue), maxValue(maxValue), step(step),
      incHotkey(ImGuiKey_None), decHotkey(ImGuiKey_None), isCapturingIncHotkey(false), isCapturingDecHotkey(false),
      disableValueOnToggle(disableValueOnToggle), valueProtected(false), onChange(nullptr), onToggle(nullptr)
{
}

void IntControl::Draw() {
    ImGui::PushID(id.c_str());

    // Toggle checkbox
    if (ImGui::Checkbox("##toggle", &enabled)) {
        if (onToggle) {
            onToggle(enabled);
        }
    }
    if (valueProtected && ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("When checked, the game cannot reduce this item's count");
        ImGui::EndTooltip();
    }
    ImGui::SameLine();

    float curPosX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(curPosX);
    ImGui::Text("%s", label.c_str());

    ImGui::SameLine();
    ImGui::SetCursorPosX(curPosX + s_labelWidth);
    if (disableValueOnToggle && !enabled) {
        ImGui::BeginDisabled();
    }

    float availWidth = ImGui::GetContentRegionAvail().x;
    float rightControlsWidth = 30.0f;
    float inputWidth = availWidth - rightControlsWidth;
    ImGui::PushItemWidth(inputWidth);
    int tempValue = value;
    if (ImGui::InputInt("##value", &value, step, step * 10)) {
        if (tempValue != value && onChange) {
            onChange(value);
        }
    }
    ImGui::PopItemWidth();

    // Settings button
    ImGui::SameLine();
    bool showSettings = ImGui::Button(ICON_FA_GEAR);

    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s: %s", ICON_FA_TOGGLE_ON, hotkey != ImGuiKey_None ? InputHelper::KeyToString(hotkey) : "None");
        ImGui::Text("%s: %s", ICON_FA_MINUS, decHotkey != ImGuiKey_None ? InputHelper::KeyToString(decHotkey) : "None");
        ImGui::Text("%s: %s", ICON_FA_PLUS, incHotkey != ImGuiKey_None ? InputHelper::KeyToString(incHotkey) : "None");
        ImGui::Text("%s: %d", ICON_FA_FORWARD_STEP, step);
        ImGui::EndTooltip();
    }

    if (disableValueOnToggle && !enabled) {
        ImGui::EndDisabled();
    }

    // Popup for settings
    if (showSettings)
        ImGui::OpenPopup(("Settings##" + id).c_str());

    if (ImGui::BeginPopup(("Settings##" + id).c_str())) {
        ImGui::Text("Settings for %s", label.c_str());
        ImGui::Separator();

        // Hotkey settings
        ImGui::Text("Toggle Hotkey:");
        ImGui::SameLine();
        DrawHotkeyButton();

        ImGui::Text("Decrement Hotkey:");
        ImGui::SameLine();
        InputHelper::DrawHotkeyButton((id + "_dec").c_str(), &decHotkey);

        ImGui::Text("Increment Hotkey:");
        ImGui::SameLine();
        InputHelper::DrawHotkeyButton((id + "_inc").c_str(), &incHotkey);

        // Step size settings
        ImGui::Separator();
        ImGui::Text("Step Size:");
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::InputInt("##stepsize", &step, 1, 10);
        ImGui::PopItemWidth();
        if (step < 1) step = 1;

        ImGui::Separator();
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void IntControl::Update() {
    bool valueChanged = false;
    int oldValue = value;

    // Toggle hotkey
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        enabled = !enabled;
        if (onToggle) {
            onToggle(enabled);
        }
    }

    // Only process increment/decrement if enabled
    if (enabled) {
        if (incHotkey != ImGuiKey_None && InputHelper::IsKeyPressed(incHotkey)) {
            Increment();
            valueChanged = true;
        }

        if (decHotkey != ImGuiKey_None && InputHelper::IsKeyPressed(decHotkey)) {
            Decrement();
            valueChanged = true;
        }

        if (valueChanged && onChange && oldValue != value) {
            onChange(value);
        }
    }
}

void IntControl::SetValue(int newValue) {
    SetValue(newValue, false);
}

void IntControl::SetValue(int newValue, bool bypassProtection) {
    if (valueProtected && newValue < value && !bypassProtection) {
        return;
    }
    // Clamp to min/max range
    value = std::min(std::max(newValue, minValue), maxValue);
}

void IntControl::Increment() {
    SetValue(value + step);
}

void IntControl::Decrement() {
    SetValue(value - step);
}


// FloatControl implementation
FloatControl::FloatControl(const std::string& label, const std::string& id, float value,
                         float minValue, float maxValue, float step, bool enabled, bool disableValueOnToggle)
    : InputControl(label, id), value(value), minValue(minValue), maxValue(maxValue), step(step),
      incHotkey(ImGuiKey_None), decHotkey(ImGuiKey_None), isCapturingIncHotkey(false), isCapturingDecHotkey(false),
      disableValueOnToggle(disableValueOnToggle), onChange(nullptr), onToggle(nullptr)
{
}

void FloatControl::Draw() {
    ImGui::PushID(id.c_str());

    // Toggle checkbox
    if (ImGui::Checkbox("##toggle", &enabled)) {
        if (onToggle) {
            onToggle(enabled);
        }
    }
    ImGui::SameLine();

    float curPosX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(curPosX);
    ImGui::Text("%s", label.c_str());

    ImGui::SameLine();
    ImGui::SetCursorPosX(curPosX + s_labelWidth);
    if (disableValueOnToggle && !enabled) {
        ImGui::BeginDisabled();
    }

    float availWidth = ImGui::GetContentRegionAvail().x;
    float rightControlsWidth = 30.0f;
    float inputWidth = availWidth - rightControlsWidth;
    ImGui::PushItemWidth(inputWidth);
    if (ImGui::InputFloat("##value", &value, step, step * 10)) {
        SetValue(value);
        if (onChange) onChange(value);
    }
    ImGui::PopItemWidth();

    // Settings button
    ImGui::SameLine();
    bool showSettings = ImGui::Button(ICON_FA_GEAR);

    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::BeginTooltip();
        ImGui::Text("%s: %s", ICON_FA_TOGGLE_ON, hotkey != ImGuiKey_None ? InputHelper::KeyToString(hotkey) : "None");
        ImGui::Text("%s: %s", ICON_FA_MINUS, decHotkey != ImGuiKey_None ? InputHelper::KeyToString(decHotkey) : "None");
        ImGui::Text("%s: %s", ICON_FA_PLUS, incHotkey != ImGuiKey_None ? InputHelper::KeyToString(incHotkey) : "None");
        ImGui::Text("%s: %.3f", ICON_FA_FORWARD_STEP, step);
        ImGui::EndTooltip();
    }

    if (disableValueOnToggle && !enabled) {
        ImGui::EndDisabled();
    }

    // Popup for settings
    if (showSettings)
        ImGui::OpenPopup(("Settings##" + id).c_str());

    if (ImGui::BeginPopup(("Settings##" + id).c_str())) {
        ImGui::Text("Settings for %s", label.c_str());
        ImGui::Separator();

        // Hotkey settings
        ImGui::Text("Toggle Hotkey:");
        ImGui::SameLine();
        DrawHotkeyButton();

        ImGui::Text("Decrement Hotkey:");
        ImGui::SameLine();
        InputHelper::DrawHotkeyButton((id + "_dec").c_str(), &decHotkey);

        ImGui::Text("Increment Hotkey:");
        ImGui::SameLine();
        InputHelper::DrawHotkeyButton((id + "_inc").c_str(), &incHotkey);

        // Step size settings
        ImGui::Separator();
        ImGui::Text("Step Size:");
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::InputFloat("##stepsize", &step, 1, 10);
        ImGui::PopItemWidth();
        if (step < 1.0f) step = 1.0f;

        ImGui::Separator();
        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void FloatControl::Update() {
    bool valueChanged = false;
    float oldValue = value;

    // Toggle hotkey
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        enabled = !enabled;
        if (onToggle) {
            onToggle(enabled);
        }
    }

    // Only process increment/decrement if enabled
    if (enabled) {
        if (incHotkey != ImGuiKey_None && InputHelper::IsKeyPressed(incHotkey)) {
            Increment();
            valueChanged = true;
        }

        if (decHotkey != ImGuiKey_None && InputHelper::IsKeyPressed(decHotkey)) {
            Decrement();
            valueChanged = true;
        }

        if (valueChanged && onChange && oldValue != value) {
            onChange(value);
        }
    }
}

void FloatControl::SetValue(float newValue) {
    // Clamp to min/max range
    value = std::min(std::max(newValue, minValue), maxValue);
}

void FloatControl::Increment() {
    SetValue(value + step);
}

void FloatControl::Decrement() {
    SetValue(value - step);
}


// ButtonControl implementation
ButtonControl::ButtonControl(const std::string& label, const std::string& id, const std::string& buttonText, std::function<void()> callback)
    : InputControl(label, id), buttonText(buttonText.empty() ? label : buttonText),
      onClick(callback), highlighted(false), highlightTimer(0.0f)
{
}

void ButtonControl::Draw() {
    ImGui::PushID(id.c_str());

    // Label + button on same line
    ImGui::Text("%s:", label.c_str());
    ImGui::SameLine();

    // Style for highlight effect when triggered by hotkey
    if (highlighted) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
        highlightTimer -= ImGui::GetIO().DeltaTime;
        if (highlightTimer <= 0.0f) {
            highlighted = false;
        }
    }

    // Main button
    if (ImGui::Button(buttonText.c_str())) {
        if (onClick) {
            onClick();
        }
    }

    if (highlighted) {
        ImGui::PopStyleColor();
    }

    // Hotkey button
    ImGui::SameLine();
    DrawHotkeyButton();

    ImGui::PopID();
}

void ButtonControl::Update() {
    // Check for hotkey trigger
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        if (onClick) {
            onClick();
            // Visual feedback when triggered by hotkey
            highlighted = true;
            highlightTimer = 0.2f; // Highlight for 0.2 seconds
        }
    }
}


// ESP Control implementation
ESPControl::ESPControl(const std::string& label, const std::string& id,
                     bool enabled, float defaultDistance,
                     float maxDistance, ImVec4 defaultColor,
                     ImVec4 defaultOutlineColor, bool defaultEnableOutline)
    : InputControl(label, id, enabled),
      distance(defaultDistance),
      maxDistance(maxDistance),
      color(defaultColor),
      outlineColor(defaultOutlineColor),
      enableOutline(defaultEnableOutline)
{
}

void ESPControl::Draw() {
    ImGui::PushID(id.c_str());

    ImGui::Checkbox(("##" + id + "_enable").c_str(), &enabled);
    ImGui::SameLine();

    float curPosX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(curPosX);
    ImGui::Text("%s", label.c_str());

    ImGui::SameLine();
    ImGui::SetCursorPosX(curPosX + s_labelWidth);

    float availWidth = ImGui::GetContentRegionAvail().x;
    float rightControlsWidth = 180.0f; // Color pickers + hotkey + spacing
    float sliderWidth = availWidth - rightControlsWidth;

    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderFloat(("##" + id + "_distance").c_str(), &distance, 0.0f, maxDistance, "%.0f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::ColorEdit4(("##" + id + "_color").c_str(), (float*)&color,
                     ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();

    ImGui::Checkbox(("O##" + id + "_outline_toggle").c_str(), &enableOutline);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Toggle outline");
    ImGui::SameLine();

    ImGui::ColorEdit4(("##" + id + "_outline_color").c_str(), (float*)&outlineColor,
                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();

    InputHelper::DrawHotkeyButton((id + "_hotkey").c_str(), &hotkey);

    ImGui::PopID();
}

void ESPControl::Update() {
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        enabled = !enabled;
    }
}

void ESPControl::SetDistance(float newDistance) {
    distance = std::min(std::max(newDistance, 0.0f), maxDistance);
}

void ESPControl::SetColor(const ImVec4& newColor) {
    color = newColor;
}


void ESPControl::SetOutlineColor(const ImVec4& newOutlineColor) {
    outlineColor = newOutlineColor;
}

void ESPControl::SetEnableOutline(bool enable) {
    enableOutline = enable;
}


// ToggleButtonControl implementation
ToggleButtonControl::ToggleButtonControl(const std::string& label, const std::string& id,
                                     const std::string& buttonText, bool enabled)
    : InputControl(label, id, enabled), buttonText(buttonText), actionHotkey(ImGuiKey_None),
      isCapturingActionHotkey(false), actionHighlighted(false), actionHighlightTimer(0.0f),
      onAction(nullptr)
{
}

void ToggleButtonControl::Draw() {
    ImGui::PushID(id.c_str());

    ImGui::Checkbox(("##" + id + "_toggle").c_str(), &enabled);
    ImGui::SameLine();

    float curPosX = ImGui::GetCursorPosX();
    ImGui::Text("%s", label.c_str());

    ImGui::SameLine();
    ImGui::SetCursorPosX(curPosX + s_labelWidth);

    if (!enabled) {
        ImGui::BeginDisabled();
    }

    // Highlight effect for action button
    bool pushedColor = false;
    if (actionHighlighted) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
        pushedColor = true;
        actionHighlightTimer -= ImGui::GetIO().DeltaTime;
        if (actionHighlightTimer <= 0.0f) {
            actionHighlighted = false;
        }
    }

    if (ImGui::Button(buttonText.c_str())) {
        ExecuteAction();
    }

    if (pushedColor) {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    ImGui::Text("Toggle:");
    ImGui::SameLine();
    DrawHotkeyButton();

    ImGui::SameLine();
    ImGui::Text("Action:");
    ImGui::SameLine();
    InputHelper::DrawHotkeyButton((id + "_actionhotkey").c_str(), &actionHotkey);

    if (!enabled) {
        ImGui::EndDisabled();
    }

    ImGui::PopID();
}

void ToggleButtonControl::Update() {
    if (hotkey != ImGuiKey_None && InputHelper::IsKeyPressed(hotkey)) {
        enabled = !enabled;
    }

    if (enabled && actionHotkey != ImGuiKey_None && InputHelper::IsKeyPressed(actionHotkey)) {
        ExecuteAction();
    }
}

void ToggleButtonControl::ExecuteAction() {
    if (onAction) {
        onAction();
        // Visual feedback
        actionHighlighted = true;
        actionHighlightTimer = 0.2f;
    }
}