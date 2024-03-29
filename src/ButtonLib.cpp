#include "main.hpp"
#include "shared/ButtonLib.hpp"
#include "GlobalNamespace/HMMainThreadDispatcher.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include <exception>
#include <vector>

std::vector<ButtonCallback> ButtonLib::buttonCombinationCallbacks;
std::unordered_map<ButtonList, bool> ButtonLib::buttonState;

// register, function void ptr, vector of required buttons by name, percent pressed to trigger action, and if you want to take sole ownership of button
bool ButtonLib::RegisterButtonCombination(const std::string& requestingMod, const std::function<void()>& callback, const std::vector<ButtonList>& requiredButtons, float requiredPercentage, bool allowOtherMods) {
    for (const auto& existingCallback : buttonCombinationCallbacks) {
        if (!existingCallback.allowOtherMods && existingCallback.owningMod != requestingMod) {
            // is da button owned
            for (const auto& requiredButton : requiredButtons) {
                if (IsButtonOwned(requiredButton, requestingMod)) {
                    // exit
                    return false;
                }
            }
        }
    }
    // we can register
    buttonCombinationCallbacks.push_back({requestingMod, callback, requiredButtons, requiredPercentage, allowOtherMods});
    return true;
}

bool ButtonLib::DeregisterButtonCombination(const std::string& requestingMod, const std::vector<ButtonList>& requiredButtons) {
    try {
        auto it = std::remove_if(buttonCombinationCallbacks.begin(), buttonCombinationCallbacks.end(), 
        [&](const ButtonCallback& callback) {
            return callback.owningMod == requestingMod && callback.requiredButtons == requiredButtons;
        });
        buttonCombinationCallbacks.erase(it, buttonCombinationCallbacks.end());
        return true;
    } catch (const std::exception& e) {
        getLogger().info("Something goofed lollllll");
        return false;
    }
}

bool ButtonLib::IsButtonOwned(const int& buttonName, const std::string& requestingMod) {
    for (const auto& callback : buttonCombinationCallbacks) {
        if (!callback.allowOtherMods && callback.owningMod != requestingMod) {
            if (std::find(callback.requiredButtons.begin(), callback.requiredButtons.end(), buttonName) != callback.requiredButtons.end()) {
                return true;
            }
        }
    }
    return false;
}

// clear the button states
void ButtonLib::ClearButtonState() {
    buttonState.clear();
}

// handle when a button is pressed
void ButtonLib::HandleButtonPress(const std::vector<ButtonList>& buttonNames) {

    for (const auto& button : buttonNames) {
        buttonState[button] = true;
    }

    for (const auto& callback : buttonCombinationCallbacks) {
        bool allButtonsPressed = true;
        for (const auto& requiredButton : callback.requiredButtons) {
            if (buttonState.find(requiredButton) == buttonState.end() || !buttonState[requiredButton]) {
                allButtonsPressed = false;
                break;
            }
        }

        float pressedPercentage = static_cast<float>(callback.requiredButtons.size()) / buttonState.size() * 100.0f;

        if (allButtonsPressed && pressedPercentage >= callback.requiredPercentage) {
            callback.callback();
        }
    }
}

MAKE_HOOK_MATCH(ButtonHook, &GlobalNamespace::HMMainThreadDispatcher::Update, void, GlobalNamespace::HMMainThreadDispatcher* self) {

    ButtonLib::ClearButtonState();
    std::vector<ButtonList> pressedButtons;

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::One, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(A);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::Two, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(B);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::Three, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(X);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::Four, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(Y);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryIndexTrigger, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(LeftTrigger);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::SecondaryIndexTrigger, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(RightTrigger);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryHandTrigger, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(LeftGrip);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::SecondaryHandTrigger, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(RightGrip);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::SecondaryThumbstick, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(LeftThumbstick);
    }

    if (GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryThumbstick, GlobalNamespace::OVRInput::Controller::Touch)) {
        pressedButtons.push_back(RightThumbstick);
    }

    if (!pressedButtons.empty()) {
        ButtonLib::HandleButtonPress(pressedButtons);
    }

    ButtonHook(self);
}

void ButtonLib::InstallHooks(){

    static bool isInitialised = false;

    if(isInitialised) return;
    INSTALL_HOOK(getLogger(), ButtonHook);
}
