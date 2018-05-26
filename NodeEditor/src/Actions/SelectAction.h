# pragma once
# include "Action.h"
# include "imguiex_canvas.h"

namespace ax {
namespace NodeEditor {

struct Object;

struct SelectAction final
    : Action
{
    using Action::Action;

    virtual const char* Name() const override { return "Select"; }

    virtual Result Accept(const InputState& inputState) override;
    virtual bool Process(const InputState& inputState) override;
    virtual void Dismiss() override;
    virtual void Cancel() override;

    virtual void Draw() override;

    virtual void Debug() override;

private:
    ImRect SelectionRect() const;

    bool m_IsActive = false;
    ImVec2 m_StartRectPosition;
    ImVec2 m_EndRectPosition;
};


} // namespace NodeEditor
} // namespace ax