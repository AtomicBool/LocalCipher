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

struct AddContactPayload
{
    std::string name;
    std::string publicKey;
};

struct SelectContactPayload
{
    int index = -1;
};

struct UIEvent
{
    enum class Type
    {
        None,
        AddContact,
        SelectContact
    } type = Type::None;

    // 只在对应 type 下有效
    AddContactPayload addContact{};
    SelectContactPayload selectContact{};
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

    // flags
    bool debug = false;
    bool display = false;
    bool firstFrame = false;

    // input
    char searchBuffer[256] = { 0 };

    // add contact
    bool showAddContact = false;
    char addName[128] = { 0 };
    char addKey[512] = { 0 };

    // selection
    int selectedContactIndex = -1;

    // events
    std::vector<UIEvent> events;

    // -------------------------
    // event helpers
    // -------------------------

    void PushAddContact(const std::string& name, const std::string& key)
    {
        UIEvent e;
        e.type = UIEvent::Type::AddContact;
        e.addContact = { name, key };
        events.push_back(e);
    }

    void PushSelectContact(int index)
    {
        UIEvent e;
        e.type = UIEvent::Type::SelectContact;
        e.selectContact = { index };
        events.push_back(e);
    }

    void ClearEvents()
    {
        events.clear();
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