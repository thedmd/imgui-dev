# pragma once
# include "Action.h"

namespace ax {
namespace NodeEditor {

struct NavigateAction final
    : Action
{
    using Action::Action;

    virtual Result Accept(const InputState& inputState) override;
    virtual bool Process(const InputState& inputState) override;

private:
};


} // namespace NodeEditor
} // namespace ax