# pragma once
# include "Action.h"
# include "imguiex_canvas.h"

namespace ax {
namespace NodeEditor {

struct NavigateAction final
    : Action
{
    using Action::Action;

    virtual const char* Name() const override { return "Navigate"; }

    virtual Result Accept(const InputState& inputState) override;
    virtual bool Process(const InputState& inputState) override;
    virtual void Dismiss() override;

    virtual void Debug() override;

private:
    enum class ZoomDirection { In, Out, Match };
    float NextZoomLevel(float currentZoom, ZoomDirection direction) const;

    bool                m_IsActive = false;
    ImGuiEx::CanvasView m_InitialView;
    ImGuiEx::CanvasView m_CurrentView;
    ImVec2              m_InitialClickPosition;
    ImVec2              m_CurrentClickPosition;

    ImVec2              m_MousePosition;
    ImVec2              m_LastDelta;
};


} // namespace NodeEditor
} // namespace ax