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
    virtual void Cancel() override;

    virtual void Debug() override;

private:
    enum class ZoomDirection { In, Out, Match };
    float NextZoomLevel(float currentZoom, ZoomDirection direction) const;
    void ApplyZoom(ImGuiEx::CanvasView& view, const ImVec2& worldZoomPoint, ZoomDirection direction) const;
    ZoomDirection WheelToZoomDirection(float mouseWheel) const;

    bool                m_IsActive = false;
    ImGuiEx::CanvasView m_InitialView;
    ImVec2              m_MousePosition;
    ImVec2              m_MouseDragDelta;
    ImVec2              m_LastMousePosition;
};


} // namespace NodeEditor
} // namespace ax