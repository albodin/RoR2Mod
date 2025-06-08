#include <algorithm>
#include "InputControls.h"
#include "globals/globals.h"
#include "fonts/IconsFontAwesome6.h"
#include "config/ConfigManager.h"

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
                    // Set to no key on Escape
                    *key = ImGuiKey_None;
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
    : label(label), id(id), enabled(enabled), hotkey(ImGuiKey_None), isCapturingHotkey(false), saveEnabledState(true)
{
}

bool InputControl::DrawHotkeyButton() {
    return InputHelper::DrawHotkeyButton((id + "_hotkey").c_str(), &hotkey);
}

json InputControl::Serialize() const {
    json data;
    if (saveEnabledState) {
        data["enabled"] = enabled;
    }
    data["hotkey"] = static_cast<int>(hotkey);
    return data;
}

void InputControl::Deserialize(const json& data) {
    if (data.contains("enabled")) enabled = data["enabled"];
    if (data.contains("hotkey")) hotkey = static_cast<ImGuiKey>(data["hotkey"]);
}


// ToggleControl implementation
ToggleControl::ToggleControl(const std::string& label, const std::string& id, bool enabled, bool autoRegister, bool showHotkey)
    : InputControl(label, id, enabled), onChange(nullptr), showHotkey(showHotkey)
{
    if (autoRegister) {
        ConfigManager::RegisterControl(this);
    }
}

ToggleControl::~ToggleControl() {
    ConfigManager::UnregisterControl(this);
}

void ToggleControl::Draw() {
    ImGui::PushID(id.c_str());

    bool oldValue = enabled;
    if (ImGui::Checkbox(label.c_str(), &enabled)) {
        if (onChange && oldValue != enabled) {
            onChange(enabled);
        }
    }

    if (showHotkey) {
        ImGui::SameLine();
        DrawHotkeyButton();
    }

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

json ToggleControl::Serialize() const {
    return InputControl::Serialize();
}

void ToggleControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
}


// IntControl implementation
IntControl::IntControl(const std::string& label, const std::string& id, int value,
                      int minValue, int maxValue, int step, bool enabled, bool disableValueOnToggle)
    : InputControl(label, id), value(value), frozenValue(value), minValue(minValue), maxValue(maxValue), step(step),
      incHotkey(ImGuiKey_None), decHotkey(ImGuiKey_None), isCapturingIncHotkey(false), isCapturingDecHotkey(false),
      disableValueOnToggle(disableValueOnToggle), onChange(nullptr), onToggle(nullptr)
{
    ConfigManager::RegisterControl(this);
}

IntControl::~IntControl() {
    ConfigManager::UnregisterControl(this);
}

void IntControl::Draw() {
    ImGui::PushID(id.c_str());

    // Toggle checkbox
    if (ImGui::Checkbox("##toggle", &enabled)) {
        if (enabled) {
            // When enabling protection, freeze the current value
            frozenValue = value;
        }
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

    // Store button position for popup positioning
    ImVec2 buttonPos = ImGui::GetItemRectMin();
    ImVec2 buttonSize = ImGui::GetItemRectSize();

    // Popup for settings
    if (showSettings) {
        ImGui::OpenPopup(("Settings##" + id).c_str());
    }

    // Use modal popup when capturing keys to prevent ESC from closing it
    bool isModal = s_capturingKey;
    bool popupOpen = false;

    // Always set position before opening popup to ensure consistency
    if (ImGui::IsPopupOpen(("Settings##" + id).c_str())) {
        ImGui::SetNextWindowPos(ImVec2(buttonPos.x + buttonSize.x + 5, buttonPos.y));
    }

    if (isModal) {
        popupOpen = ImGui::BeginPopupModal(("Settings##" + id).c_str(), nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    } else {
        popupOpen = ImGui::BeginPopup(("Settings##" + id).c_str(),
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    }

    if (popupOpen) {

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
    // Clamp to min/max range
    value = std::min(std::max(newValue, minValue), maxValue);
}

void IntControl::Increment() {
    SetValue(value + step);
}

void IntControl::Decrement() {
    SetValue(value - step);
}

json IntControl::Serialize() const {
    // Only save item controls if they're enabled
    if (id.find("item_") == 0 && !enabled) {
        return json(); // Return empty json for disabled item controls
    }

    json data = InputControl::Serialize();
    // For enabled item controls, save the frozen value instead of current value
    if (id.find("item_") == 0 && enabled) {
        data["value"] = frozenValue;
    } else {
        data["value"] = value;
    }
    data["incHotkey"] = static_cast<int>(incHotkey);
    data["decHotkey"] = static_cast<int>(decHotkey);
    data["step"] = step;
    return data;
}

void IntControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("value")) {
        SetValue(data["value"]);
        // For item controls, also set frozen value when loading
        if (id.find("item_") == 0) {
            frozenValue = data["value"];
        }
    }
    if (data.contains("incHotkey")) incHotkey = static_cast<ImGuiKey>(data["incHotkey"]);
    if (data.contains("decHotkey")) decHotkey = static_cast<ImGuiKey>(data["decHotkey"]);
    if (data.contains("step")) step = data["step"];
}


// FloatControl implementation
FloatControl::FloatControl(const std::string& label, const std::string& id, float value,
                         float minValue, float maxValue, float step, bool enabled, bool disableValueOnToggle)
    : InputControl(label, id), value(value), minValue(minValue), maxValue(maxValue), step(step),
      incHotkey(ImGuiKey_None), decHotkey(ImGuiKey_None), isCapturingIncHotkey(false), isCapturingDecHotkey(false),
      disableValueOnToggle(disableValueOnToggle), onChange(nullptr), onToggle(nullptr)
{
    ConfigManager::RegisterControl(this);
}

FloatControl::~FloatControl() {
    ConfigManager::UnregisterControl(this);
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

    // Store button position for popup positioning
    ImVec2 buttonPos = ImGui::GetItemRectMin();
    ImVec2 buttonSize = ImGui::GetItemRectSize();

    // Popup for settings
    if (showSettings) {
        ImGui::OpenPopup(("Settings##" + id).c_str());
    }

    // Use modal popup when capturing keys to prevent ESC from closing it
    bool isModal = s_capturingKey;
    bool popupOpen = false;

    // Always set position before opening popup to ensure consistency
    if (ImGui::IsPopupOpen(("Settings##" + id).c_str())) {
        ImGui::SetNextWindowPos(ImVec2(buttonPos.x + buttonSize.x + 5, buttonPos.y));
    }

    if (isModal) {
        popupOpen = ImGui::BeginPopupModal(("Settings##" + id).c_str(), nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    } else {
        popupOpen = ImGui::BeginPopup(("Settings##" + id).c_str(),
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    }

    if (popupOpen) {

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

json FloatControl::Serialize() const {
    json data = InputControl::Serialize();
    data["value"] = value;
    data["incHotkey"] = static_cast<int>(incHotkey);
    data["decHotkey"] = static_cast<int>(decHotkey);
    data["step"] = step;
    return data;
}

void FloatControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("value")) SetValue(data["value"]);
    if (data.contains("incHotkey")) incHotkey = static_cast<ImGuiKey>(data["incHotkey"]);
    if (data.contains("decHotkey")) decHotkey = static_cast<ImGuiKey>(data["decHotkey"]);
    if (data.contains("step")) step = data["step"];
}


// ButtonControl implementation
ButtonControl::ButtonControl(const std::string& label, const std::string& id, const std::string& buttonText, std::function<void()> callback)
    : InputControl(label, id), buttonText(buttonText.empty() ? label : buttonText),
      onClick(callback), highlighted(false), highlightTimer(0.0f)
{
    ConfigManager::RegisterControl(this);
}

ButtonControl::~ButtonControl() {
    ConfigManager::UnregisterControl(this);
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

json ButtonControl::Serialize() const {
    return InputControl::Serialize();
}

void ButtonControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
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
    ConfigManager::RegisterControl(this);
}

ESPControl::~ESPControl() {
    ConfigManager::UnregisterControl(this);
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

json ESPControl::Serialize() const {
    json data = InputControl::Serialize();
    data["distance"] = distance;
    data["color"] = {color.x, color.y, color.z, color.w};
    data["outlineColor"] = {outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w};
    data["enableOutline"] = enableOutline;
    return data;
}

void ESPControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("distance")) distance = data["distance"];
    if (data.contains("color") && data["color"].is_array() && data["color"].size() == 4) {
        color = ImVec4(data["color"][0], data["color"][1], data["color"][2], data["color"][3]);
    }
    if (data.contains("outlineColor") && data["outlineColor"].is_array() && data["outlineColor"].size() == 4) {
        outlineColor = ImVec4(data["outlineColor"][0], data["outlineColor"][1], data["outlineColor"][2], data["outlineColor"][3]);
    }
    if (data.contains("enableOutline")) enableOutline = data["enableOutline"];
}


// SliderControl implementation
SliderControl::SliderControl(const std::string& label, const std::string& id, float value,
                           float minValue, float maxValue, bool autoRegister)
    : InputControl(label, id, false), value(value), minValue(minValue), maxValue(maxValue), onChange(nullptr)
{
    if (autoRegister) {
        ConfigManager::RegisterControl(this);
    }
}

SliderControl::~SliderControl() {
    ConfigManager::UnregisterControl(this);
}

void SliderControl::Draw() {
    ImGui::PushID(id.c_str());

    ImGui::Text("%s", label.c_str());
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

    if (ImGui::SliderFloat(("##" + id).c_str(), &value, minValue, maxValue, "%.1f")) {
        if (onChange) {
            onChange(value);
        }
    }

    ImGui::PopID();
}

void SliderControl::Update() {
    // No hotkey support for simple sliders
}

void SliderControl::SetValue(float newValue) {
    value = std::min(std::max(newValue, minValue), maxValue);
}

json SliderControl::Serialize() const {
    json data = InputControl::Serialize();
    data["value"] = value;
    return data;
}

void SliderControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("value")) SetValue(data["value"]);
}

// ToggleButtonControl implementation
ToggleButtonControl::ToggleButtonControl(const std::string& label, const std::string& id,
                                     const std::string& buttonText, bool enabled)
    : InputControl(label, id, enabled), buttonText(buttonText), actionHotkey(ImGuiKey_None),
      isCapturingActionHotkey(false), actionHighlighted(false), actionHighlightTimer(0.0f),
      onAction(nullptr)
{
    ConfigManager::RegisterControl(this);
}

ToggleButtonControl::~ToggleButtonControl() {
    ConfigManager::UnregisterControl(this);
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

json ToggleButtonControl::Serialize() const {
    json data = InputControl::Serialize();
    data["actionHotkey"] = static_cast<int>(actionHotkey);
    return data;
}

void ToggleButtonControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("actionHotkey")) actionHotkey = static_cast<ImGuiKey>(data["actionHotkey"]);
}


// EntityESPSubControl implementation
EntityESPSubControl::EntityESPSubControl(const std::string& label, const std::string& id)
    : label(label), id(id),
      nameColor(1.0f, 1.0f, 1.0f, 1.0f),
      distanceColor(0.8f, 0.8f, 0.8f, 1.0f),
      healthColor(0.0f, 1.0f, 0.0f, 1.0f),
      maxHealthColor(0.0f, 0.8f, 0.0f, 1.0f),
      boxColor(1.0f, 0.0f, 0.0f, 1.0f),
      tracelineColor(1.0f, 1.0f, 0.0f, 1.0f) {

    enabled = new ToggleControl("Enabled", id + "_enabled", false, false);
    showName = new ToggleControl("Show Name", id + "_showName", true, false);
    showDistance = new ToggleControl("Show Distance", id + "_showDistance", true, false);
    showHealth = new ToggleControl("Show Health", id + "_showHealth", false, false);
    showMaxHealth = new ToggleControl("Show Max Health", id + "_showMaxHealth", false, false);
    showHealthbar = new ToggleControl("Show Healthbar", id + "_showHealthbar", false, false);
    showBox = new ToggleControl("Show Box", id + "_showBox", false, false);
    showTraceline = new ToggleControl("Show Traceline", id + "_showTraceline", false, false);
    maxDistance = new SliderControl("Max Distance", id + "_maxDistance", 100.0f, 0.0f, 1000.0f, false);
}

EntityESPSubControl::~EntityESPSubControl() {
    delete enabled;
    delete showName;
    delete showDistance;
    delete showHealth;
    delete showMaxHealth;
    delete showHealthbar;
    delete showBox;
    delete showTraceline;
    delete maxDistance;
}

void EntityESPSubControl::Draw() {
    if (ImGui::CollapsingHeader(label.c_str())) {
        enabled->Draw();

        // Text options with color pickers
        showName->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Name Color##" + id).c_str(), (float*)&nameColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        showDistance->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Distance Color##" + id).c_str(), (float*)&distanceColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        showHealth->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Health Color##" + id).c_str(), (float*)&healthColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        showMaxHealth->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Max Health Color##" + id).c_str(), (float*)&maxHealthColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        showHealthbar->Draw();

        showBox->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Box Color##" + id).c_str(), (float*)&boxColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        showTraceline->Draw();
        ImGui::SameLine();
        ImGui::ColorEdit4(("Traceline Color##" + id).c_str(), (float*)&tracelineColor,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        // Sliders
        maxDistance->Draw();
    }
}

void EntityESPSubControl::Update() {
    enabled->Update();
    showName->Update();
    showDistance->Update();
    showHealth->Update();
    showMaxHealth->Update();
    showHealthbar->Update();
    showBox->Update();
    showTraceline->Update();
    maxDistance->Update();
}

// Getters
bool EntityESPSubControl::IsEnabled() const { return enabled->IsEnabled(); }
bool EntityESPSubControl::ShouldShowName() const { return showName->IsEnabled(); }
bool EntityESPSubControl::ShouldShowDistance() const { return showDistance->IsEnabled(); }
bool EntityESPSubControl::ShouldShowHealth() const { return showHealth->IsEnabled(); }
bool EntityESPSubControl::ShouldShowMaxHealth() const { return showMaxHealth->IsEnabled(); }
bool EntityESPSubControl::ShouldShowHealthbar() const { return showHealthbar->IsEnabled(); }
bool EntityESPSubControl::ShouldShowBox() const { return showBox->IsEnabled(); }
bool EntityESPSubControl::ShouldShowTraceline() const { return showTraceline->IsEnabled(); }
float EntityESPSubControl::GetMaxDistance() const { return maxDistance->GetValue(); }
ImVec4 EntityESPSubControl::GetNameColor() const { return nameColor; }
ImVec4 EntityESPSubControl::GetDistanceColor() const { return distanceColor; }
ImVec4 EntityESPSubControl::GetHealthColor() const { return healthColor; }
ImVec4 EntityESPSubControl::GetMaxHealthColor() const { return maxHealthColor; }
ImVec4 EntityESPSubControl::GetBoxColor() const { return boxColor; }
ImVec4 EntityESPSubControl::GetTracelineColor() const { return tracelineColor; }
ImU32 EntityESPSubControl::GetNameColorU32() const { return ImGui::ColorConvertFloat4ToU32(nameColor); }
ImU32 EntityESPSubControl::GetDistanceColorU32() const { return ImGui::ColorConvertFloat4ToU32(distanceColor); }
ImU32 EntityESPSubControl::GetHealthColorU32() const { return ImGui::ColorConvertFloat4ToU32(healthColor); }
ImU32 EntityESPSubControl::GetMaxHealthColorU32() const { return ImGui::ColorConvertFloat4ToU32(maxHealthColor); }
ImU32 EntityESPSubControl::GetBoxColorU32() const { return ImGui::ColorConvertFloat4ToU32(boxColor); }
ImU32 EntityESPSubControl::GetTracelineColorU32() const { return ImGui::ColorConvertFloat4ToU32(tracelineColor); }

json EntityESPSubControl::Serialize() const {
    json data;
    data["enabled"] = enabled->Serialize();
    data["showName"] = showName->Serialize();
    data["showDistance"] = showDistance->Serialize();
    data["showHealth"] = showHealth->Serialize();
    data["showMaxHealth"] = showMaxHealth->Serialize();
    data["showHealthbar"] = showHealthbar->Serialize();
    data["showBox"] = showBox->Serialize();
    data["showTraceline"] = showTraceline->Serialize();
    data["maxDistance"] = maxDistance->Serialize();
    data["nameColor"] = {nameColor.x, nameColor.y, nameColor.z, nameColor.w};
    data["distanceColor"] = {distanceColor.x, distanceColor.y, distanceColor.z, distanceColor.w};
    data["healthColor"] = {healthColor.x, healthColor.y, healthColor.z, healthColor.w};
    data["maxHealthColor"] = {maxHealthColor.x, maxHealthColor.y, maxHealthColor.z, maxHealthColor.w};
    data["boxColor"] = {boxColor.x, boxColor.y, boxColor.z, boxColor.w};
    data["tracelineColor"] = {tracelineColor.x, tracelineColor.y, tracelineColor.z, tracelineColor.w};
    return data;
}

void EntityESPSubControl::Deserialize(const json& data) {
    if (data.contains("enabled")) enabled->Deserialize(data["enabled"]);
    if (data.contains("showName")) showName->Deserialize(data["showName"]);
    if (data.contains("showDistance")) showDistance->Deserialize(data["showDistance"]);
    if (data.contains("showHealth")) showHealth->Deserialize(data["showHealth"]);
    if (data.contains("showMaxHealth")) showMaxHealth->Deserialize(data["showMaxHealth"]);
    if (data.contains("showHealthbar")) showHealthbar->Deserialize(data["showHealthbar"]);
    if (data.contains("showBox")) showBox->Deserialize(data["showBox"]);
    if (data.contains("showTraceline")) showTraceline->Deserialize(data["showTraceline"]);
    if (data.contains("maxDistance")) maxDistance->Deserialize(data["maxDistance"]);
    if (data.contains("nameColor") && data["nameColor"].is_array() && data["nameColor"].size() == 4) {
        nameColor = ImVec4(data["nameColor"][0], data["nameColor"][1], data["nameColor"][2], data["nameColor"][3]);
    }
    if (data.contains("distanceColor") && data["distanceColor"].is_array() && data["distanceColor"].size() == 4) {
        distanceColor = ImVec4(data["distanceColor"][0], data["distanceColor"][1], data["distanceColor"][2], data["distanceColor"][3]);
    }
    if (data.contains("healthColor") && data["healthColor"].is_array() && data["healthColor"].size() == 4) {
        healthColor = ImVec4(data["healthColor"][0], data["healthColor"][1], data["healthColor"][2], data["healthColor"][3]);
    }
    if (data.contains("maxHealthColor") && data["maxHealthColor"].is_array() && data["maxHealthColor"].size() == 4) {
        maxHealthColor = ImVec4(data["maxHealthColor"][0], data["maxHealthColor"][1], data["maxHealthColor"][2], data["maxHealthColor"][3]);
    }
    if (data.contains("boxColor") && data["boxColor"].is_array() && data["boxColor"].size() == 4) {
        boxColor = ImVec4(data["boxColor"][0], data["boxColor"][1], data["boxColor"][2], data["boxColor"][3]);
    }
    if (data.contains("tracelineColor") && data["tracelineColor"].is_array() && data["tracelineColor"].size() == 4) {
        tracelineColor = ImVec4(data["tracelineColor"][0], data["tracelineColor"][1], data["tracelineColor"][2], data["tracelineColor"][3]);
    }
}

// EntityESPControl implementation
EntityESPControl::EntityESPControl(const std::string& label, const std::string& id)
    : InputControl(label, id, false) {

    masterEnabled = new ToggleControl("Master Enabled", id + "_master", false, false);
    visibleControl = new EntityESPSubControl("Visible " + label, id + "_visible");
    nonVisibleControl = new EntityESPSubControl("Non-Visible " + label, id + "_nonvisible");

    ConfigManager::RegisterControl(this);
}

EntityESPControl::~EntityESPControl() {
    delete masterEnabled;
    delete visibleControl;
    delete nonVisibleControl;
    ConfigManager::UnregisterControl(this);
}

void EntityESPControl::Draw() {
    ImGui::PushID(id.c_str());

    if (ImGui::CollapsingHeader(label.c_str())) {
        masterEnabled->Draw();
        visibleControl->Draw();
        nonVisibleControl->Draw();
    }

    ImGui::PopID();
}

void EntityESPControl::Update() {
    masterEnabled->Update();
    visibleControl->Update();
    nonVisibleControl->Update();
}

EntityESPSubControl* EntityESPControl::GetVisibleControl() { return visibleControl; }
EntityESPSubControl* EntityESPControl::GetNonVisibleControl() { return nonVisibleControl; }
bool EntityESPControl::IsMasterEnabled() const { return masterEnabled->IsEnabled(); }

json EntityESPControl::Serialize() const {
    json data = InputControl::Serialize();
    data["masterEnabled"] = masterEnabled->Serialize();
    data["visible"] = visibleControl->Serialize();
    data["nonVisible"] = nonVisibleControl->Serialize();
    return data;
}

void EntityESPControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("masterEnabled")) masterEnabled->Deserialize(data["masterEnabled"]);
    if (data.contains("visible")) visibleControl->Deserialize(data["visible"]);
    if (data.contains("nonVisible")) nonVisibleControl->Deserialize(data["nonVisible"]);
}

// ChestESPSubControl implementation
ChestESPSubControl::ChestESPSubControl(const std::string& label, const std::string& id)
    : InputControl(label, id, false) {

    enabled = new ToggleControl("Enabled", id + "_enabled", true, false);
    showName = new ToggleControl("Show Name", id + "_showName", true, false);
    showDistance = new ToggleControl("Show Distance", id + "_showDistance", true, false);
    showCost = new ToggleControl("Show Cost", id + "_showCost", true, false);
    showUnavailable = new ToggleControl("Show Unavailable", id + "_showUnavailable", true, false);
    showTraceline = new ToggleControl("Show Traceline", id + "_showTraceline", false, false);
    enableNameShadow = new ToggleControl("O", id + "_enableNameShadow", true, false, false);
    enableDistanceShadow = new ToggleControl("O", id + "_enableDistanceShadow", true, false, false);
    enableCostShadow = new ToggleControl("O", id + "_enableCostShadow", true, false, false);
    maxDistance = new SliderControl("Max Distance", id + "_maxDistance", 500.0f, 0.0f, 1000.0f, false);

    nameColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
    distanceColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
    costColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
    tracelineColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    nameShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
    distanceShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
    costShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
}

ChestESPSubControl::~ChestESPSubControl() {
    delete enabled;
    delete showName;
    delete showDistance;
    delete showCost;
    delete showUnavailable;
    delete showTraceline;
    delete enableNameShadow;
    delete enableDistanceShadow;
    delete enableCostShadow;
    delete maxDistance;
}

void ChestESPSubControl::Draw() {
    ImGui::PushID(id.c_str());

    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Indent();
        enabled->Draw();
        if (enabled->IsEnabled()) {
            // Show Name with color picker and shadow controls
            showName->Draw();
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_name_color").c_str(), (float*)&nameColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            enableNameShadow->Draw();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle outline");
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_name_shadow_color").c_str(), (float*)&nameShadowColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            // Show Distance with color picker and shadow controls
            showDistance->Draw();
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_distance_color").c_str(), (float*)&distanceColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            enableDistanceShadow->Draw();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle outline");
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_distance_shadow_color").c_str(), (float*)&distanceShadowColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            // Show Cost with color picker and shadow controls
            showCost->Draw();
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_cost_color").c_str(), (float*)&costColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            enableCostShadow->Draw();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle outline");
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_cost_shadow_color").c_str(), (float*)&costShadowColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            // Show Unavailable (no color picker needed)
            showUnavailable->Draw();

            // Show Traceline with color picker
            showTraceline->Draw();
            ImGui::SameLine();
            ImGui::ColorEdit4(("##" + id + "_traceline_color").c_str(), (float*)&tracelineColor,
                             ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            // Max Distance slider
            maxDistance->Draw();
        }
        ImGui::Unindent();
    }

    ImGui::PopID();
}

void ChestESPSubControl::Update() {
    enabled->Update();
    showName->Update();
    showDistance->Update();
    showCost->Update();
    showUnavailable->Update();
    showTraceline->Update();
    enableNameShadow->Update();
    enableDistanceShadow->Update();
    enableCostShadow->Update();
    maxDistance->Update();
}

bool ChestESPSubControl::IsEnabled() const { return enabled->IsEnabled(); }
bool ChestESPSubControl::ShouldShowName() const { return showName->IsEnabled(); }
bool ChestESPSubControl::ShouldShowDistance() const { return showDistance->IsEnabled(); }
bool ChestESPSubControl::ShouldShowCost() const { return showCost->IsEnabled(); }
bool ChestESPSubControl::ShouldShowUnavailable() const { return showUnavailable->IsEnabled(); }
bool ChestESPSubControl::ShouldShowTraceline() const { return showTraceline->IsEnabled(); }
bool ChestESPSubControl::IsNameShadowEnabled() const { return enableNameShadow->IsEnabled(); }
bool ChestESPSubControl::IsDistanceShadowEnabled() const { return enableDistanceShadow->IsEnabled(); }
bool ChestESPSubControl::IsCostShadowEnabled() const { return enableCostShadow->IsEnabled(); }
float ChestESPSubControl::GetMaxDistance() const { return maxDistance->GetValue(); }

ImVec4 ChestESPSubControl::GetNameColor() const { return nameColor; }
ImVec4 ChestESPSubControl::GetDistanceColor() const { return distanceColor; }
ImVec4 ChestESPSubControl::GetCostColor() const { return costColor; }
ImVec4 ChestESPSubControl::GetTracelineColor() const { return tracelineColor; }
ImVec4 ChestESPSubControl::GetNameShadowColor() const { return nameShadowColor; }
ImVec4 ChestESPSubControl::GetDistanceShadowColor() const { return distanceShadowColor; }
ImVec4 ChestESPSubControl::GetCostShadowColor() const { return costShadowColor; }

ImU32 ChestESPSubControl::GetNameColorU32() const { return ImGui::GetColorU32(nameColor); }
ImU32 ChestESPSubControl::GetDistanceColorU32() const { return ImGui::GetColorU32(distanceColor); }
ImU32 ChestESPSubControl::GetCostColorU32() const { return ImGui::GetColorU32(costColor); }
ImU32 ChestESPSubControl::GetTracelineColorU32() const { return ImGui::GetColorU32(tracelineColor); }
ImU32 ChestESPSubControl::GetNameShadowColorU32() const { return ImGui::GetColorU32(nameShadowColor); }
ImU32 ChestESPSubControl::GetDistanceShadowColorU32() const { return ImGui::GetColorU32(distanceShadowColor); }
ImU32 ChestESPSubControl::GetCostShadowColorU32() const { return ImGui::GetColorU32(costShadowColor); }

json ChestESPSubControl::Serialize() const {
    json data;
    data["enabled"] = enabled->Serialize();
    data["showName"] = showName->Serialize();
    data["showDistance"] = showDistance->Serialize();
    data["showCost"] = showCost->Serialize();
    data["showUnavailable"] = showUnavailable->Serialize();
    data["showTraceline"] = showTraceline->Serialize();
    data["enableNameShadow"] = enableNameShadow->Serialize();
    data["enableDistanceShadow"] = enableDistanceShadow->Serialize();
    data["enableCostShadow"] = enableCostShadow->Serialize();
    data["maxDistance"] = maxDistance->Serialize();

    data["nameColor"] = {nameColor.x, nameColor.y, nameColor.z, nameColor.w};
    data["distanceColor"] = {distanceColor.x, distanceColor.y, distanceColor.z, distanceColor.w};
    data["costColor"] = {costColor.x, costColor.y, costColor.z, costColor.w};
    data["tracelineColor"] = {tracelineColor.x, tracelineColor.y, tracelineColor.z, tracelineColor.w};
    data["nameShadowColor"] = {nameShadowColor.x, nameShadowColor.y, nameShadowColor.z, nameShadowColor.w};
    data["distanceShadowColor"] = {distanceShadowColor.x, distanceShadowColor.y, distanceShadowColor.z, distanceShadowColor.w};
    data["costShadowColor"] = {costShadowColor.x, costShadowColor.y, costShadowColor.z, costShadowColor.w};

    return data;
}

void ChestESPSubControl::Deserialize(const json& data) {
    // Don't call base class deserialize - we don't use those fields

    if (data.contains("enabled")) enabled->Deserialize(data["enabled"]);
    if (data.contains("showName")) showName->Deserialize(data["showName"]);
    if (data.contains("showDistance")) showDistance->Deserialize(data["showDistance"]);
    if (data.contains("showCost")) showCost->Deserialize(data["showCost"]);
    if (data.contains("showUnavailable")) showUnavailable->Deserialize(data["showUnavailable"]);
    if (data.contains("showTraceline")) showTraceline->Deserialize(data["showTraceline"]);
    if (data.contains("enableNameShadow")) enableNameShadow->Deserialize(data["enableNameShadow"]);
    if (data.contains("enableDistanceShadow")) enableDistanceShadow->Deserialize(data["enableDistanceShadow"]);
    if (data.contains("enableCostShadow")) enableCostShadow->Deserialize(data["enableCostShadow"]);
    if (data.contains("maxDistance")) maxDistance->Deserialize(data["maxDistance"]);

    if (data.contains("nameColor") && data["nameColor"].is_array() && data["nameColor"].size() == 4) {
        nameColor = ImVec4(data["nameColor"][0], data["nameColor"][1], data["nameColor"][2], data["nameColor"][3]);
    }
    if (data.contains("distanceColor") && data["distanceColor"].is_array() && data["distanceColor"].size() == 4) {
        distanceColor = ImVec4(data["distanceColor"][0], data["distanceColor"][1], data["distanceColor"][2], data["distanceColor"][3]);
    }
    if (data.contains("costColor") && data["costColor"].is_array() && data["costColor"].size() == 4) {
        costColor = ImVec4(data["costColor"][0], data["costColor"][1], data["costColor"][2], data["costColor"][3]);
    }
    if (data.contains("tracelineColor") && data["tracelineColor"].is_array() && data["tracelineColor"].size() == 4) {
        tracelineColor = ImVec4(data["tracelineColor"][0], data["tracelineColor"][1], data["tracelineColor"][2], data["tracelineColor"][3]);
    }
    if (data.contains("nameShadowColor") && data["nameShadowColor"].is_array() && data["nameShadowColor"].size() == 4) {
        nameShadowColor = ImVec4(data["nameShadowColor"][0], data["nameShadowColor"][1], data["nameShadowColor"][2], data["nameShadowColor"][3]);
    }
    if (data.contains("distanceShadowColor") && data["distanceShadowColor"].is_array() && data["distanceShadowColor"].size() == 4) {
        distanceShadowColor = ImVec4(data["distanceShadowColor"][0], data["distanceShadowColor"][1], data["distanceShadowColor"][2], data["distanceShadowColor"][3]);
    }
    if (data.contains("costShadowColor") && data["costShadowColor"].is_array() && data["costShadowColor"].size() == 4) {
        costShadowColor = ImVec4(data["costShadowColor"][0], data["costShadowColor"][1], data["costShadowColor"][2], data["costShadowColor"][3]);
    }
}

// ChestESPControl implementation
ChestESPControl::ChestESPControl(const std::string& label, const std::string& id)
    : InputControl(label, id, false) {

    masterEnabled = new ToggleControl("Master Enabled", id + "_master", false, false);
    subControl = new ChestESPSubControl(label + " Settings", id + "_settings");

    ConfigManager::RegisterControl(this);
}

ChestESPControl::~ChestESPControl() {
    delete masterEnabled;
    delete subControl;
    ConfigManager::UnregisterControl(this);
}

void ChestESPControl::Draw() {
    ImGui::PushID(id.c_str());

    if (ImGui::CollapsingHeader(label.c_str())) {
        masterEnabled->Draw();
        ImGui::Indent();
        subControl->Draw();
        ImGui::Unindent();
    }

    ImGui::PopID();
}

void ChestESPControl::Update() {
    masterEnabled->Update();
    subControl->Update();
}

ChestESPSubControl* ChestESPControl::GetSubControl() { return subControl; }
bool ChestESPControl::IsMasterEnabled() const { return masterEnabled->IsEnabled(); }

json ChestESPControl::Serialize() const {
    json data = InputControl::Serialize();
    data["masterEnabled"] = masterEnabled->Serialize();
    data["settings"] = subControl->Serialize();
    return data;
}

void ChestESPControl::Deserialize(const json& data) {
    InputControl::Deserialize(data);
    if (data.contains("masterEnabled")) masterEnabled->Deserialize(data["masterEnabled"]);
    if (data.contains("settings")) subControl->Deserialize(data["settings"]);
}