#pragma once

#include "ButtonLib.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

enum ButtonList
{
    A,
    B,
    X,
    Y,
    LeftTrigger,
    RightTrigger,
    LeftGrip,
    RightGrip,
    LeftThumbstick,
    RightThumbstick,
};

struct ButtonCallback {
    std::string owningMod;
    std::function<void()> callback;
    std::vector<ButtonList> requiredButtons;
    float requiredPercentage;
    bool allowOtherMods;
};



class ButtonLib {
public:
    static bool RegisterButtonCombination(const std::string& requestingMod, const std::function<void()>& callback, const std::vector<ButtonList>& requiredButtons, float requiredPercentage, bool allowOtherMods);

    static bool DeregisterButtonCombination(const std::string& requestingMod, const std::vector<ButtonList>& requiredButtons);
    
    static void HandleButtonPress(const std::vector<ButtonList>& buttonNames);

    static bool IsButtonOwned(const int& buttonName, const std::string& requestingMod);

    static void ClearButtonState();

    static void InstallHooks();

private:
    static std::vector<ButtonCallback> buttonCombinationCallbacks;

    static std::unordered_map<ButtonList, bool> buttonState;
};
