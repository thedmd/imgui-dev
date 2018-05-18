# include "Actions.h"

ax::NodeEditor::Action::Result ax::NodeEditor::NavigateAction::Accept(const InputState& inputState)
{
    return No;
}

bool ax::NodeEditor::NavigateAction::Process(const InputState& inputState)
{
    return false;
}