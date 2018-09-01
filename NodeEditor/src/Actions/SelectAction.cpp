# include "NodeEditor_Internal.h"
# include "Actions.h"
# include "Editor.h"

ax::NodeEditor::Action::Result ax::NodeEditor::SelectAction::Accept(const InputState& inputState)
{
    IM_ASSERT(!m_IsActive);

    if (inputState.Canvas.Active && ImGui::IsMouseDragging(c_ConfigSelectButtonIndex, m_Editor.View().InvScale))
    {
        m_IsActive          = true;
        m_StartRectPosition = inputState.MousePosition;
        m_EndRectPosition   = m_StartRectPosition;

        return Yes;
    }
    else if (inputState.Canvas.Clicked)
    {
        m_Editor.DeselectAll();

        return No;
    }
    else if (inputState.Node.Clicked || inputState.Pin.Clicked)
    {
        auto operation = SelectOperation::Replace;
        if (inputState.Modifiers == KeyModifers::Ctrl)
            operation = SelectOperation::Toggle;

        m_Editor.Select(inputState.Node.Clicked, operation);

        return No;
    }

    if (inputState.Node.Hovered)
        return Possible;

    return No;
}

bool ax::NodeEditor::SelectAction::Process(const InputState& inputState)
{
    if (!m_IsActive)
        return false;

    if (ImGui::IsMouseDragging(c_ConfigSelectButtonIndex, 0.0f))
    {
        m_EndRectPosition = ImGui::GetMousePos();
    }
    else
    {
        auto nodes = m_Editor.FindObjectsInside(SelectionRect(), ObjectTypes::Nodes);
        m_Editor.Select(nodes);

        Dismiss();

        return true;
    }

    //if (ImGui::IsMouseSelectging(c_ConfigScrollButtonIndex, 0.0f))
    //{
    //}
    //else
    //    Dismiss();

    return m_IsActive;
}

void ax::NodeEditor::SelectAction::Dismiss()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
}

void ax::NodeEditor::SelectAction::Cancel()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
}

ImRect ax::NodeEditor::SelectAction::SelectionRect() const
{
    ImRect result;
    result.Min = ImMin(m_StartRectPosition, m_EndRectPosition);
    result.Max = ImMax(m_StartRectPosition, m_EndRectPosition);
    return result;
}

void ax::NodeEditor::SelectAction::Draw()
{
    if (m_IsActive)
    {
        auto drawList = ImGui::GetWindowDrawList();

        const ImU32 c_ConfigSelectionRectFill   = IM_COL32(  5, 130, 255,  64);
        const ImU32 c_ConfigSelectionRectBorder = IM_COL32(  5, 130, 255, 128);

        auto selectionRect = m_Editor.ContentView().ToWorld(SelectionRect());

        m_Editor.SuspendView();
        if (selectionRect.GetWidth() <= 3.0f || selectionRect.GetHeight() <= 3.0f)
        {
            drawList->AddRectFilled(
                selectionRect.Min,
                selectionRect.Max,
                c_ConfigSelectionRectBorder);
        }
        else
        {
            drawList->AddRectFilled(
                selectionRect.Min,
                selectionRect.Max,
                c_ConfigSelectionRectFill);

            drawList->AddRect(
                selectionRect.Min,
                selectionRect.Max,
                c_ConfigSelectionRectBorder,
                0.0f,
                ImDrawCornerFlags_All,
                1.0f);
        }
        m_Editor.ResumeView();
    }
}

void ax::NodeEditor::SelectAction::Debug()
{
    //ImGui::Text("Mouse Position: %s", Debug::ToString(m_MousePosition).c_str());
    //ImGui::Text("Mouse Delta: %s", Debug::ToString(m_MouseSelectDelta).c_str());
    //ImGui::Text("Last Mouse Position: %s", Debug::ToString(m_LastMousePosition).c_str());
    //ImGui::Text("Initial View: %s", Debug::ToString(m_InitialView).c_str());
}
