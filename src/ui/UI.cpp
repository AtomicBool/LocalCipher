#include "ui/UI.h"

#include <cstring>

namespace UI {
    void Render(UIState& state, ContactManager& contactManager) {
        if (!state.display) return;

        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        ImVec2 nextWindowSize = ImVec2(viewportSize.x * state.sizesPercentage[0], viewportSize.y * state.sizesPercentage[1]);
        ImGui::SetNextWindowSize(nextWindowSize);
        ImGui::SetNextWindowPos(ImVec2((viewportSize.x - nextWindowSize.x) / 2, (viewportSize.y - nextWindowSize.y) / 2));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
        ImGui::Begin("LocalCipherMain", nullptr, window_flags);

        if (state.firstFrame) {
            ImGui::SetKeyboardFocusHere(0);
            state.firstFrame = false;
        }

        ImGui::PushItemWidth(-1.0f);
        ImGui::InputTextWithHint("##search", "Search contacts...", state.searchBuffer, sizeof(state.searchBuffer));
        ImGui::PopItemWidth();
        ImGui::Separator();

        auto filteredContacts = contactManager.search(state.searchBuffer);
        ImGui::BeginChild("ContactList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);
        for (const auto& contact : filteredContacts) {
            bool is_selected = (state.selectedContact.name == contact.name);
            if (ImGui::Selectable(contact.name.c_str(), is_selected)) {
                state.selectedContact = contact;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Public Key: %s", contact.public_key.c_str());
        }
        ImGui::EndChild();

        if (ImGui::Button("Add New Contact")) {
            state.showAddContact = true;
            memset(state.addName, 0, sizeof(state.addName));
            memset(state.addKey, 0, sizeof(state.addKey));
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetFrameHeight() - ImGui::CalcTextSize("Debug").x - ImGui::GetStyle().ItemInnerSpacing.x - ImGui::GetStyle().WindowPadding.x);
        ImGui::Checkbox("Debug", &state.debug);

        if (state.showAddContact) ImGui::OpenPopup("Add Contact");
        if (ImGui::BeginPopupModal("Add Contact", &state.showAddContact, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Name", state.addName, 128);
            ImGui::InputText("Public Key", state.addKey, 512);
            if (ImGui::Button("Save", ImVec2(120, 0))) {
                if (state.addName[0] != 0 && state.addKey[0] != 0) {
                    contactManager.addContact({ state.addName, state.addKey });
                    state.showAddContact = false;
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { state.showAddContact = false; ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (state.debug) {
            ImGui::Separator();
            ImGui::DragFloat2("Window Sizes", state.sizesPercentage, 0.001f, 0.0f, 1.0f);
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        }

        ImGui::End();
    }
}
