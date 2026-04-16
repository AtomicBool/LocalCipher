#pragma once

#include <cstring>
#include <vector>
#include "imgui.h"
#include "utils/Contact.h"

struct UIState {
    float sizesPercentage[2] = { 0.45f, 0.6f };
    char searchBuffer[256] = "";
    bool debug = false;
    bool display = false;
    bool firstFrame = false;

    bool showAddContact = false;
    char addName[128] = "";
    char addKey[512] = "";

    Contact selectedContact = { "", "" };

    // events
    bool pendingAdd = false;
};

struct ContactViewModel {
    std::vector<Contact> contacts;
};

namespace UI {
    void Render(UIState& state, const ContactViewModel& vm);
}