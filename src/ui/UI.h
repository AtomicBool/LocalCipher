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

struct UIEvent
{
    enum class Type
    {
        None,
        AddContact,
        SelectContact
    } type = Type::None;

    struct AddContactPayload
    {
        std::string name;
        std::string publicKey;
    } addContact{};

    struct SelectContactPayload
    {
        Contact selected;
    } selectContact{};
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
// UI STATE
// =========================
//
struct UIState
{
    // flags
    bool debug = false;
    bool display = false;
    bool firstFrame = false;

    // events
    std::vector<UIEvent> events;

    // input
    char searchBuffer[256] = { 0 };

    // layout
    float sizesPercentage[2] = { 0.5f, 0.3f };

    // add contact
    bool showAddContact = false;
    char addName[128] = { 0 };
    char addKey[567] = { 0 };

    // selection
    std::string selectedContactName = "";

    // -------------------------
    // event helpers
    // ------------------------- 
    void ClearEvents()
    {
        events.clear();
    }
    
    void PushAddContact(const std::string& name, const std::string& key)
    {
        UIEvent e;
        e.type = UIEvent::Type::AddContact;
        e.addContact = { name, key };
        events.push_back(e);
    }

    void PushSelectContact(Contact m_contact)
    {
        UIEvent e;
        e.type = UIEvent::Type::SelectContact;
        e.selectContact = { m_contact };
        events.push_back(e);
    }
};

struct PopupState {
    bool visible = false;
    std::string text;
    ImVec2 pos = ImVec2(0, 0);
    ImVec2 curMousePos = ImVec2(0, 0);
    ImVec2 lastMousePos = ImVec2(0, 0);
};

//
// =========================
// UI RENDER API
// =========================
//

namespace UI
{
    void Render(UIState& state, const ContactViewModel& vm);
    void RenderPopUP(PopupState& state);
}