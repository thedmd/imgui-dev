# pragma once
# include "NodeEditor_Internal.h"
# include "Builders/NodeBuilder.h"

namespace ax {
namespace NodeEditor {

struct Editor
{
    Editor(const char* str_id);

    bool Begin(const ImVec2& size = ImVec2(0, 0));
    void End();

    void NavigateTo(const ImVec2& point, bool immediate = false);
    void NavigateTo(const ImRect& rect, bool immediate = false);

    const ImRect& Viewport() const;

    NodeBuilder BuildNode(NodeId id);

    void Debug();

private:
    InputState BuildInputState();

    void ProcessActions(const InputState& inputState);

    void ResetLiveObjects();

    ImGuiID         m_Id;

    ImGuiEx::Canvas m_Canvas;
    ImRect          m_Viewport;

    ObjectCollection<Pin>  m_Pins;
    ObjectCollection<Node> m_Nodes;
    ObjectCollection<Link> m_Links;
    Canvas                 m_BackgroundCanvas;

    InputState m_InputState;

    Action*             m_CurrentAction = nullptr;
    Action*             m_PossibleAction = nullptr;
    NavigateAction      m_NavigateAction;
    ImVector<Action*>   m_Actions;
};

} // namespace NodeEditor
} // namespace ax
