#include "ui/UI.h"
#include <cstring>

namespace UI {

    void Render(UIState& state, const ContactViewModel& vm)
    {
        if (!state.display)
            return;

        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

        ImVec2 nextWindowSize(
            viewportSize.x * state.sizesPercentage[0],
            viewportSize.y * state.sizesPercentage[1]
        );

        ImGui::SetNextWindowSize(nextWindowSize);
        ImGui::SetNextWindowPos(
            ImVec2(
                (viewportSize.x - nextWindowSize.x) * 0.5f,
                (viewportSize.y - nextWindowSize.y) * 0.5f
            )
        );

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove;

        ImGui::Begin("ChatRSAMain", nullptr, flags);

        // =====================================================
        // First frame focus
        // =====================================================
        if (state.firstFrame)
        {
            ImGui::SetKeyboardFocusHere(0);
            state.firstFrame = false;
        }

        // =====================================================
        // Search (pure UI input)
        // =====================================================
        ImGui::PushItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##search",
            "Search contacts...",
            state.searchBuffer,
            sizeof(state.searchBuffer)
        );
        ImGui::PopItemWidth();

        ImGui::Separator();

        // =====================================================
        // Contact list (read-only view)
        // =====================================================
        ImGui::BeginChild(
            "ContactList",
            ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2),
            true
        );

        for (int i = 0; i < (int)vm.contacts.size(); i++)
        {
            const auto& contact = vm.contacts[i];

            bool selected = (state.selectedContactIndex == i);

            if (ImGui::Selectable(contact.name.c_str(), selected))
            {
                state.selectedContactIndex = i;

                state.PushEvent(
                    UIEventType::SelectContact,
                    contact.name
                );
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "Public Key: %s",
                    contact.public_key.c_str()
                );
            }
        }

        ImGui::EndChild();

        // =====================================================
        // Add contact button (UI intent only)
        // =====================================================
        if (ImGui::Button("Add New Contact"))
        {
            state.showAddContact = true;
            memset(state.addName, 0, sizeof(state.addName));
            memset(state.addKey, 0, sizeof(state.addKey));
        }

        // Debug toggle
        ImGui::SameLine(
            ImGui::GetWindowWidth()
            - ImGui::GetFrameHeight()
            - ImGui::CalcTextSize("Debug").x
            - ImGui::GetStyle().ItemInnerSpacing.x
            - ImGui::GetStyle().WindowPadding.x
        );

        ImGui::Checkbox("Debug", &state.debug);

        // =====================================================
        // Popup (still UI-only, but emits event on confirm)
        // =====================================================
        if (state.showAddContact)
            ImGui::OpenPopup("Add Contact");

        if (ImGui::BeginPopupModal(
            "Add Contact",
            &state.showAddContact,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Name", state.addName, 128);
            ImGui::InputText("Public Key", state.addKey, 512);

            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                // emit event instead of flag
                std::string payload =
                    std::string(state.addName) + "|" + std::string(state.addKey);

                state.PushEvent(UIEventType::AddContact, payload);

                state.showAddContact = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                state.showAddContact = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // =====================================================
        // Debug panel
        // =====================================================
        if (state.debug)
        {
            ImGui::Separator();
            ImGui::DragFloat2(
                "Window Sizes",
                state.sizesPercentage,
                0.001f,
                0.0f,
                1.0f
            );

            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        }

        ImGui::End();
    }

}