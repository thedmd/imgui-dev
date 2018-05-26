# pragma once
# include "Action.h"
# include "imguiex_canvas.h"

namespace ax {
namespace NodeEditor {

struct Object;

struct DragAction final
    : Action
{
    using Action::Action;

    virtual const char* Name() const override { return "Drag"; }

    virtual Result Accept(const InputState& inputState) override;
    virtual bool Process(const InputState& inputState) override;
    virtual void Dismiss() override;
    virtual void Cancel() override;

    virtual void Debug() override;

private:
    bool                m_IsActive = false;
    ImVector<Object*>   m_Selection;
};


} // namespace NodeEditor
} // namespace ax