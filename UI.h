#pragma once
#include "imgui/imgui.h"
#include "search.h"

struct UIState {
    float sizesPercentage[2] = { 0.9f, 0.7f };
    char searchBuffer[256] = "";
    bool debug = false;
    bool display = false;
    bool firstFrame = false;
    
    // Add contact temporary state
    bool showAddContact = false;
    char addName[128] = "";
    char addKey[512] = "";
    
    Contact selectedContact = { "", "" };
};

namespace UI {
    void Render(UIState& state, ContactManager& contactManager);
}
