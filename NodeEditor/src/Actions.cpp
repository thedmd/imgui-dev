# include "Actions.h"

ax::NodeEditor::Action::Action(Editor& editor)
    : m_Editor(editor)
{
}

ax::NodeEditor::Action::~Action()
{
}

ax::NodeEditor::Action::AcceptResult ax::NodeEditor::NavigateAction::Accept()
{
    return False;
}

bool ax::NodeEditor::NavigateAction::Process()
{
    return false;
}