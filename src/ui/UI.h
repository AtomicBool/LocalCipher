#pragma once

#include <vector>
#include <string>
#include "imgui.h"
#include "utils/Contact.h"

//
// =========================
// UI EVENTS (UI -> App)
// =========================
//

enum class UIEventType
{
    None,
    AddContact,
    DeleteContact,
    SelectContact
};

struct UIEvent
{
    UIEventType type = UIEventType::None;

    std::string payload;
};

//
// =========================
// VIEW MODEL (App -> UI)
// =========================
//

struct ContactViewModel
{
    std::vector<Contact> contacts;
};

//
// =========================
// UI STATE (pure UI state)
// =========================
//

struct UIState
{
    // layout
    float sizesPercentage[2] = { 0.45f, 0.6f };

    // global ui flags
    bool debug = false;
    bool display = false;
    bool firstFrame = false;

    // search
    char searchBuffer[256] = { 0 };

    // add contact dialog input cache
    bool showAddContact = false;
    char addName[128] = { 0 };
    char addKey[512] = { 0 };

    // selection (UI-only index, NOT domain object)
    int selectedContactIndex = -1;

    // UI OUTPUT (events to Application layer)
    std::vector<UIEvent> events;

    // helper: clear events each frame
    void ClearEvents()
    {
        events.clear();
    }

    void PushEvent(UIEventType type, const std::string& payload = "")
    {
        events.push_back({ type, payload });
    }
};

//
// =========================
// UI RENDER API
// =========================
//

namespace UI
{
    void Render(UIState& state, const ContactViewModel& vm);
}