#pragma once
#include <string>
#include <imgui.h>
#include <functional>
#include "utils/json.hpp"

using json = nlohmann::json;

// Key state helper functions
namespace InputHelper {
    bool IsKeyPressed(ImGuiKey key);
    bool IsKeyDown(ImGuiKey key);
    const char* KeyToString(ImGuiKey key);
    bool DrawHotkeyButton(const char* id, ImGuiKey* key);
}

// Base interface for all input controls
class InputControl {
protected:
    std::string label;
    std::string id;
    bool enabled;
    ImGuiKey hotkey;
    bool isCapturingHotkey;
    bool saveEnabledState;

public:
    InputControl(const std::string& label, const std::string& id, bool enabled = false);
    virtual ~InputControl() = default;

    virtual void Draw() = 0;
    virtual void Update() = 0;
    bool DrawHotkeyButton();

    void SetEnabled(bool value) { enabled = value; }
    bool IsEnabled() const { return enabled; }
    void SetHotkey(ImGuiKey key) { hotkey = key; }
    ImGuiKey GetHotkey() const { return hotkey; }

    void SetSaveEnabledState(bool save) { saveEnabledState = save; }
    bool GetSaveEnabledState() const { return saveEnabledState; }

    virtual json Serialize() const;
    virtual void Deserialize(const json& data);
    const std::string& GetId() const { return id; }
};

// Toggle (checkbox) control
class ToggleControl : public InputControl {
private:
    std::function<void(bool)> onChange;

public:
    ToggleControl(const std::string& label, const std::string& id, bool enabled = false);
    virtual ~ToggleControl();

    void Draw() override;
    void Update() override;
    void SetOnChange(std::function<void(bool)> callback) { onChange = callback; }

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

// Integer input control
class IntControl : public InputControl {
private:
    int value;
    int minValue;
    int maxValue;
    int step;
    ImGuiKey incHotkey;
    ImGuiKey decHotkey;
    bool isCapturingIncHotkey;
    bool isCapturingDecHotkey;
    bool disableValueOnToggle;
    bool valueProtected;
    std::function<void(int)> onChange;
    std::function<void(bool)> onToggle;

public:
    IntControl(const std::string& label, const std::string& id, int value,
              int minValue, int maxValue, int step = 1, bool enabled = false,
              bool disableValueOnToggle = true);
    virtual ~IntControl();

    void Draw() override;
    void Update() override;
    void Increment();
    void Decrement();
    int GetValue() const { return value; }
    void SetValue(int newValue);
    void SetValue(int newValue, bool bypassProtection);
    void SetOnChange(std::function<void(int)> callback) { onChange = callback; }
    void SetOnToggle(std::function<void(bool)> callback) { onToggle = callback; }
    void SetDisableValueOnToggle(bool disable) { disableValueOnToggle = disable; }
    bool GetDisableValueOnToggle() const { return disableValueOnToggle; }
    void SetValueProtected(bool isProtected) { valueProtected = isProtected; }
    bool IsValueProtected() const { return valueProtected; }

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

// Float input control
class FloatControl : public InputControl {
private:
    float value;
    float minValue;
    float maxValue;
    float step;
    ImGuiKey incHotkey;
    ImGuiKey decHotkey;
    bool isCapturingIncHotkey;
    bool isCapturingDecHotkey;
    bool disableValueOnToggle;
    std::function<void(float)> onChange;
    std::function<void(bool)> onToggle;

public:
    FloatControl(const std::string& label, const std::string& id, float value,
                float minValue = 0.0f, float maxValue = FLT_MAX, float step = 1.0f,
                bool enabled = false, bool disableValueOnToggle = true);
    virtual ~FloatControl();

    void Draw() override;
    void Update() override;
    void Increment();
    void Decrement();
    float GetValue() const { return value; }
    void SetValue(float newValue);
    void SetOnChange(std::function<void(float)> callback) { onChange = callback; }
    void SetOnToggle(std::function<void(bool)> callback) { onToggle = callback; }
    void SetDisableValueOnToggle(bool disable) { disableValueOnToggle = disable; }
    bool GetDisableValueOnToggle() const { return disableValueOnToggle; }

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

class ButtonControl : public InputControl {
private:
    std::function<void()> onClick;
    std::string buttonText;
    bool highlighted;
    float highlightTimer;

public:
    ButtonControl(const std::string& label, const std::string& id, const std::string& buttonText = "",
                  std::function<void()> callback = nullptr);
    virtual ~ButtonControl();

    void Draw() override;
    void Update() override;
    void SetOnClick(std::function<void()> callback) { onClick = callback; }
    void SetButtonText(const std::string& text) { buttonText = text; }

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

class ESPControl : public InputControl {
private:
    float distance;
    float maxDistance;
    ImVec4 color;
    ImVec4 outlineColor;
    bool enableOutline;

public:
    ESPControl(const std::string& label, const std::string& id,
              bool enabled = false, float defaultDistance = 100.0f,
              float maxDistance = 500.0f, ImVec4 defaultColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
              ImVec4 outlineColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f), bool enableOutline = true);
    virtual ~ESPControl();

    void Draw() override;
    void Update() override;

    float GetDistance() const { return distance; }
    ImVec4 GetColor() const { return color; }
    ImU32 GetColorU32() const { return ImGui::ColorConvertFloat4ToU32(color); }
    ImU32 GetOutlineColorU32() const { return ImGui::ColorConvertFloat4ToU32(outlineColor); }
    bool IsOutlineEnabled() const { return enableOutline; }

    void SetDistance(float newDistance);
    void SetColor(const ImVec4& newColor);
    void SetOutlineColor(const ImVec4& newOutlineColor);
    void SetEnableOutline(bool enable);

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

class ToggleButtonControl : public InputControl {
private:
    std::function<void()> onAction;
    std::string buttonText;
    ImGuiKey actionHotkey;
    bool isCapturingActionHotkey;
    bool actionHighlighted;
    float actionHighlightTimer;

public:
    ToggleButtonControl(const std::string& label, const std::string& id,
                      const std::string& buttonText = "Execute",
                      bool enabled = false);
    virtual ~ToggleButtonControl();

    void Draw() override;
    void Update() override;

    void SetOnAction(std::function<void()> callback) { onAction = callback; }
    void SetActionHotkey(ImGuiKey key) { actionHotkey = key; }
    ImGuiKey GetActionHotkey() const { return actionHotkey; }
    void ExecuteAction();

    json Serialize() const override;
    void Deserialize(const json& data) override;
};