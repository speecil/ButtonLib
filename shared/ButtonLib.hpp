#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

struct ButtonCallback {
    std::string owningMod;
    std::function<void()> callback;
    std::vector<std::string> requiredButtons;
    float requiredPercentage;
    bool allowOtherMods;
};

class ButtonLib {
public:
    static void RegisterButtonCombination(const std::string& requestingMod, const std::function<void()>& callback, const std::vector<std::string>& requiredButtons, float requiredPercentage, bool allowOtherMods);

    static void DeregisterButtonCombination(const std::string& requestingMod, const std::vector<std::string>& requiredButtons);
    
    static void HandleButtonPress(const std::vector<std::string>& buttonNames);

    static bool IsButtonOwned(const std::string& buttonName, const std::string& requestingMod);

    static void ClearButtonState();

    static void InstallHooks();

private:
    static std::vector<ButtonCallback> buttonCombinationCallbacks;

    static std::unordered_map<std::string, bool> buttonState;
};
