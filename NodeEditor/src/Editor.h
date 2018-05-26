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
    const ImGuiEx::CanvasView& ContentView() const;
    void SuspendView();
    void ResumeView();

    void Select(Object* object, SelectOperation operation = SelectOperation::Replace);
    void Select(const ImVector<Object*>& objects, SelectOperation operation = SelectOperation::Replace);
    void DeselectAll();
    const Selection& SelectedObjects() const;

    NodeBuilder BuildNode(NodeId id);

    void Debug(bool inWindow = true);

private:
    InputState BuildInputState();

    void ProcessActions(const InputState& inputState);
    void DrawActions();

    void ResetLiveObjects();

    ImGuiID         m_Id;

    ImGuiEx::Canvas     m_Canvas;
    ImGuiEx::CanvasView m_CanvasView;
    ImGuiEx::CanvasView m_CanvasContentView;

    ObjectCollection<Pin>  m_Pins;
    ObjectCollection<Node> m_Nodes;
    ObjectCollection<Link> m_Links;
    ax::NodeEditor::Canvas m_BackgroundCanvas;

    InputState m_InputState;

    Action*             m_CurrentAction = nullptr;
    Action*             m_PossibleAction = nullptr;
    NavigateAction      m_NavigateAction;
    SelectAction        m_SelectAction;
    DragAction          m_DragNodeAction;
    Action*             m_Actions[3]
    {
        &m_NavigateAction,
        &m_SelectAction,
        &m_DragNodeAction
    };

    Selection           m_Selection;

    Object*             m_DebugLastClicked = nullptr;
    Object*             m_DebugLastDoubleClicked = nullptr;
};

} // namespace NodeEditor
} // namespace ax
