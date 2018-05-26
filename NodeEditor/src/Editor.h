# pragma once
# include "NodeEditor_Internal.h"
# include "Builders/NodeBuilder.h"
# include "Actions/NavigateAction.h"

namespace ax {
namespace NodeEditor {

struct Editor
{
    Editor(const char* str_id);

    bool Begin(const ImVec2& size = ImVec2(0, 0));
    void End();

    void NavigateTo(const ImVec2& point, bool immediate = false);
    void NavigateTo(const ImRect& rect, bool immediate = false);

    void SetView(const ImVec2& origin, float scale);
    void SetView(const ImGuiEx::CanvasView& view);
    const ImGuiEx::CanvasView& View() const;

    NodeBuilder BuildNode(NodeId id);

    void Debug(bool inWindow = true);

private:
    InputState BuildInputState();

    void ProcessActions(const InputState& inputState);

    void ResetLiveObjects();

    ImGuiID         m_Id;

    ImGuiEx::Canvas     m_Canvas;
    ImGuiEx::CanvasView m_CanvasView;

    ObjectCollection<Pin>  m_Pins;
    ObjectCollection<Node> m_Nodes;
    ObjectCollection<Link> m_Links;
    ax::NodeEditor::Canvas m_BackgroundCanvas;

    InputState m_InputState;

    Action*             m_CurrentAction = nullptr;
    Action*             m_PossibleAction = nullptr;
    NavigateAction      m_NavigateAction;
    Action*             m_Actions[1]
    {
        &m_NavigateAction
    };
};

} // namespace NodeEditor
} // namespace ax
