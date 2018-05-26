# include "NodeEditor_Internal.h"
# include "Actions.h"
# include "Editor.h"

ax::NodeEditor::Action::Result ax::NodeEditor::DragAction::Accept(const InputState& inputState)
{
    IM_ASSERT(!m_IsActive);

    auto& io = ImGui::GetIO();

    if (inputState.Node.Hovered && ImGui::IsMouseDragging(c_ConfigDragNodeButtonIndex, 0.0f))
    {
        m_IsActive          = true;
        //m_InitialView       = m_Editor.View();
        //m_LastMousePosition = m_InitialView.ToWorld(io.MouseClickedPos[c_ConfigScrollButtonIndex]);

        return Yes;
    }

    if (inputState.Node.Hovered)
        return Possible;

    return No;
}

bool ax::NodeEditor::DragAction::Process(const InputState& inputState)
{
    IM_ASSERT(m_IsActive);

    if (ImGui::IsMouseDragging(c_ConfigScrollButtonIndex, 0.0f))
    {
    }
    else
        Dismiss();

    return m_IsActive;
}

void ax::NodeEditor::DragAction::Dismiss()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
}

void ax::NodeEditor::DragAction::Cancel()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
}

void ax::NodeEditor::DragAction::Debug()
{
    //ImGui::Text("Mouse Position: %s", Debug::ToString(m_MousePosition).c_str());
    //ImGui::Text("Mouse Delta: %s", Debug::ToString(m_MouseDragDelta).c_str());
    //ImGui::Text("Last Mouse Position: %s", Debug::ToString(m_LastMousePosition).c_str());
    //ImGui::Text("Initial View: %s", Debug::ToString(m_InitialView).c_str());
}
