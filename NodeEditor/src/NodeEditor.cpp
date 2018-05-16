# define IMGUI_DEFINE_MATH_OPERATORS
# include "NodeEditor.h"
# include "imgui_internal.h"
# include "imguiex_internal.h"

ax::NodeEditor::NodeBuilder::NodeBuilder(Editor& editor, Node& node)
    : m_Editor(editor)
    , m_Node(node)
{
    m_Node.Touch();

    ImGui::SetCursorScreenPos(m_Node.m_Bounds.Min);

    ImGui::BeginGroup();
}


ax::NodeEditor::NodeBuilder::~NodeBuilder()
{
    ImGui::EndGroup();

    m_Node.m_Bounds.Min = ImGui::GetItemRectMin();
    m_Node.m_Bounds.Max = ImGui::GetItemRectMax();


}





ax::NodeEditor::Editor::Editor(const char* str_id)
    : m_Id(ImHash(str_id, 0))
    , m_Canvas(m_Id)
    , m_NavigateAction(*this)
{
}

bool ax::NodeEditor::Editor::Begin(const ImVec2& size)
{
    if (!m_Canvas.Begin(nullptr, size))
        return false;

    // #debug
    ImGui::GetWindowDrawList()->AddRectFilled(
        m_Canvas.ViewRect().Min,
        m_Canvas.ViewRect().Max,
        IM_COL32(48, 48, 48, 255));

    return true;
}

void ax::NodeEditor::Editor::End()
{
    ProcessActions();

    m_Canvas.End();

    m_Pins.Discard();
    m_Nodes.Discard();
    m_Links.Discard();

    // #debug
    ImGui::GetWindowDrawList()->AddRect(
        m_Canvas.ContentRect().Min - ImVec2(1, 1),
        m_Canvas.ContentRect().Max + ImVec2(1, 1),
        IM_COL32(255, 255, 255, 255));
}

void ax::NodeEditor::Editor::ProcessActions()
{
    if (m_CurrentAction && !m_CurrentAction->Process())
        m_CurrentAction = nullptr;

    if (nullptr != m_CurrentAction)
        return;

    Action* possibleAction = nullptr;
    Action* nextAction = nullptr;

    for (auto& action : m_Actions)
    {
        auto response = action->Accept();
        if (response == Action::Accepted)
        {;
            nextAction = action;
            break;
        }
        else if (response == Action::Possible)
        {
            if (!possibleAction)
                possibleAction = action;
            else
                action->Dismiss();
        }
    }

    if (nextAction)
    {
        m_CurrentAction = nextAction;

        if (possibleAction)
            possibleAction->Dismiss();
    }
}

ax::NodeEditor::NodeBuilder ax::NodeEditor::Editor::BuildNode(NodeId id)
{
    IM_ASSERT(id != NodeId::Invalid);

    return NodeBuilder(*this, *m_Nodes.Get(id));
}