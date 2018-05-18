# include "NodeEditor_Internal.h"
# include "NodeBuilder.h"

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
