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
    bool showHotkey;

public:
    ToggleControl(const std::string& label, const std::string& id, bool enabled = false, bool autoRegister = true, bool showHotkey = true);
    virtual ~ToggleControl();

    void Draw() override;
    void Update() override;
    void SetOnChange(std::function<void(bool)> callback) { onChange = callback; }
    void SetShowHotkey(bool show) { showHotkey = show; }

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

// Integer input control
class IntControl : public InputControl {
private:
    int value;
    int frozenValue; // Value to maintain when protection is enabled
    int minValue;
    int maxValue;
    int step;
    ImGuiKey incHotkey;
    ImGuiKey decHotkey;
    bool isCapturingIncHotkey;
    bool isCapturingDecHotkey;
    bool disableValueOnToggle;
    bool showCheckbox;
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
    int GetFrozenValue() const { return frozenValue; }
    void SetValue(int newValue);
    void UpdateFrozenValue() { frozenValue = value; }
    void SetOnChange(std::function<void(int)> callback) { onChange = callback; }
    void SetOnToggle(std::function<void(bool)> callback) { onToggle = callback; }
    void SetDisableValueOnToggle(bool disable) { disableValueOnToggle = disable; }
    bool GetDisableValueOnToggle() const { return disableValueOnToggle; }
    void SetShowCheckbox(bool show) { showCheckbox = show; }

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

class SliderControl : public InputControl {
private:
    float value;
    float minValue;
    float maxValue;
    std::function<void(float)> onChange;

public:
    SliderControl(const std::string& label, const std::string& id, float value,
                 float minValue = 0.0f, float maxValue = 100.0f, bool autoRegister = true);
    virtual ~SliderControl();

    void Draw() override;
    void Update() override;
    float GetValue() const { return value; }
    void SetValue(float newValue);
    void SetOnChange(std::function<void(float)> callback) { onChange = callback; }

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

class EntityESPSubControl {
private:
    std::string label;
    std::string id;

    std::unique_ptr<ToggleControl> enabled;
    std::unique_ptr<ToggleControl> showName;
    std::unique_ptr<ToggleControl> showDistance;
    std::unique_ptr<ToggleControl> showHealth;
    std::unique_ptr<ToggleControl> showMaxHealth;
    std::unique_ptr<ToggleControl> showHealthbar;
    std::unique_ptr<ToggleControl> showBox;
    std::unique_ptr<ToggleControl> showTraceline;
    std::unique_ptr<SliderControl> maxDistance;
    ImVec4 nameColor;
    ImVec4 distanceColor;
    ImVec4 healthColor;
    ImVec4 maxHealthColor;
    ImVec4 boxColor;
    ImVec4 tracelineColor;

public:
    EntityESPSubControl(const std::string& label, const std::string& id);
    ~EntityESPSubControl();

    void Draw();
    void Update();

    // Getters
    bool IsEnabled() const;
    bool ShouldShowName() const;
    bool ShouldShowDistance() const;
    bool ShouldShowHealth() const;
    bool ShouldShowMaxHealth() const;
    bool ShouldShowHealthbar() const;
    bool ShouldShowBox() const;
    bool ShouldShowTraceline() const;
    float GetMaxDistance() const;
    ImVec4 GetNameColor() const;
    ImVec4 GetDistanceColor() const;
    ImVec4 GetHealthColor() const;
    ImVec4 GetMaxHealthColor() const;
    ImVec4 GetBoxColor() const;
    ImVec4 GetTracelineColor() const;
    ImU32 GetNameColorU32() const;
    ImU32 GetDistanceColorU32() const;
    ImU32 GetHealthColorU32() const;
    ImU32 GetMaxHealthColorU32() const;
    ImU32 GetBoxColorU32() const;
    ImU32 GetTracelineColorU32() const;

    json Serialize() const;
    void Deserialize(const json& data);
};

class EntityESPControl : public InputControl {
private:
    std::unique_ptr<ToggleControl> masterEnabled;
    std::unique_ptr<EntityESPSubControl> visibleControl;
    std::unique_ptr<EntityESPSubControl> nonVisibleControl;

public:
    EntityESPControl(const std::string& label, const std::string& id);
    ~EntityESPControl();

    void Draw() override;
    void Update() override;

    EntityESPSubControl* GetVisibleControl();
    EntityESPSubControl* GetNonVisibleControl();
    bool IsMasterEnabled() const;

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

class ChestESPSubControl : public InputControl {
private:
    std::unique_ptr<ToggleControl> enabled;
    std::unique_ptr<ToggleControl> showName;
    std::unique_ptr<ToggleControl> showDistance;
    std::unique_ptr<ToggleControl> showCost;
    std::unique_ptr<ToggleControl> showUnavailable;
    std::unique_ptr<ToggleControl> showTraceline;
    std::unique_ptr<ToggleControl> enableNameShadow;
    std::unique_ptr<ToggleControl> enableDistanceShadow;
    std::unique_ptr<ToggleControl> enableCostShadow;
    std::unique_ptr<SliderControl> maxDistance;
    ImVec4 nameColor;
    ImVec4 distanceColor;
    ImVec4 costColor;
    ImVec4 tracelineColor;
    ImVec4 nameShadowColor;
    ImVec4 distanceShadowColor;
    ImVec4 costShadowColor;

public:
    ChestESPSubControl(const std::string& label, const std::string& id);
    ~ChestESPSubControl();

    void Draw() override;
    void Update() override;

    // Getters
    bool IsEnabled() const;
    bool ShouldShowName() const;
    bool ShouldShowDistance() const;
    bool ShouldShowCost() const;
    bool ShouldShowUnavailable() const;
    bool ShouldShowTraceline() const;
    bool IsNameShadowEnabled() const;
    bool IsDistanceShadowEnabled() const;
    bool IsCostShadowEnabled() const;
    float GetMaxDistance() const;
    ImVec4 GetNameColor() const;
    ImVec4 GetDistanceColor() const;
    ImVec4 GetCostColor() const;
    ImVec4 GetTracelineColor() const;
    ImVec4 GetNameShadowColor() const;
    ImVec4 GetDistanceShadowColor() const;
    ImVec4 GetCostShadowColor() const;
    ImU32 GetNameColorU32() const;
    ImU32 GetDistanceColorU32() const;
    ImU32 GetCostColorU32() const;
    ImU32 GetTracelineColorU32() const;
    ImU32 GetNameShadowColorU32() const;
    ImU32 GetDistanceShadowColorU32() const;
    ImU32 GetCostShadowColorU32() const;

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

class ChestESPControl : public InputControl {
private:
    std::unique_ptr<ToggleControl> masterEnabled;
    std::unique_ptr<ChestESPSubControl> subControl;

public:
    ChestESPControl(const std::string& label, const std::string& id);
    ~ChestESPControl();

    void Draw() override;
    void Update() override;

    ChestESPSubControl* GetSubControl();
    bool IsMasterEnabled() const;

    json Serialize() const override;
    void Deserialize(const json& data) override;
};

// Dropdown control with hotkey navigation
class ComboControl : public InputControl {
private:
    int selectedIndex;
    std::vector<std::string> items;
    std::vector<int> itemValues;  // Optional associated values
    ImGuiKey prevHotkey;
    ImGuiKey nextHotkey;
    bool isCapturingPrevHotkey;
    bool isCapturingNextHotkey;
    std::function<void(int)> onChange;
    bool showHotkeys;

public:
    ComboControl(const std::string& label, const std::string& id,
                const std::vector<std::string>& items,
                int defaultIndex = 0, bool showHotkeys = true);
    ComboControl(const std::string& label, const std::string& id,
                const std::vector<std::string>& items,
                const std::vector<int>& values,
                int defaultIndex = 0, bool showHotkeys = true);
    virtual ~ComboControl();

    void Draw() override;
    void Update() override;

    int GetSelectedIndex() const { return selectedIndex; }
    int GetSelectedValue() const;
    const std::string& GetSelectedItem() const;
    void SetSelectedIndex(int index);
    void SelectNext();
    void SelectPrevious();

    void SetOnChange(std::function<void(int)> callback) { onChange = callback; }
    void SetItems(const std::vector<std::string>& newItems, const std::vector<int>& newValues = {});

    json Serialize() const override;
    void Deserialize(const json& data) override;
};