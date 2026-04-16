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

            bool selected = (contact.name == state.selectedContactName);

            if (ImGui::Selectable(contact.name.c_str(), selected))
            {
                state.selectedContactName = contact.name;

                state.PushSelectContact(contact);
            }

            //if (ImGui::IsItemHovered())
            //{
            //    ImGui::SetTooltip(
            //        "Public Key: %s",
            //        contact.public_key.c_str()
            //    );
            //}
        }

        ImGui::EndChild();

        // =====================================================
        // Add contact button
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
        // Add contact Popup
        // =====================================================
        if (state.showAddContact)
            ImGui::OpenPopup("Add Contact");

        if (ImGui::BeginPopupModal(
            "Add Contact",
            &state.showAddContact,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("Name", state.addName, 128);
			ImGui::InputText("Public Key", state.addKey, sizeof(state.addKey)); // 566 char + 1 terminator

            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                state.PushAddContact(state.addName, state.addKey);

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

    void RenderPopUP(PopupState& state) {
        if (!state.visible)
            return;

        float dx = state.curMousePos.x - state.lastMousePos.x;
        float dy = state.curMousePos.y - state.lastMousePos.y;

        if (dx * dx + dy * dy > 5.0f)
        {
            state.visible = false;
            return;
        }

        state.pos = ImVec2(
            state.curMousePos.x + 12,
            state.curMousePos.y + 12
        );

        ImGui::SetNextWindowPos(state.pos);

        ImGui::Begin(
            "##popup",
            nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav
        );

        ImGui::PushTextWrapPos(500.0f);
        ImGui::TextUnformatted(state.text.c_str());
        ImGui::PopTextWrapPos();

        ImGui::End();
    }
}