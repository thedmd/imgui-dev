# pragma once
# include "imgui.h"

namespace ImGuiEx {

struct Context;

Context* CreateContext();
void     DestroyContext(Context* context = nullptr); // NULL = destroy current context
Context* GetCurrentContext();
void     SetCurrentContext(Context* context);

// Canvas widget - view over infinite virtual space.
//
// Canvas allows you to draw your widgets anywhere over infinite space and provide
// view over it with support for panning and scaling.
//
// When you enter a canvas ImGui is moved to virtual space which mean:
//   - ImGui::GetCursorScreenPos() return (0, 0) and which correspond to top left corner
//     of the canvas on the screen (this can be changed usign CanvasView()).
//   - Mouse input is brought to canvas space, so widgets works as usual.
//   - Everything you draw with ImDrawList will be in virtual space.
//
// By default origin point is on top left corner of canvas widget. It can be
// changed with call to CanvasView() where you can specify what part of space
// should be viewed by setting viewport origin point and scale. Current state
// can be queried with CanvasViewOrigin() and CanvasViewScale().
//
// Viewport size is controlled by 'size' parameter in BeginCanvas(). You can query
// it using CanvasContentMin/Max/Size functions. They are useful if you to not specify
// canvas size in which case all free space is used.
//
// Bounds of visible region of infinite space can be queried using CanvasViewMin/Max/Size
// functions. Everything that is drawn outside of this region will be clipped
// as usual in ImGui.
//
// While drawing inside canvas you can translate position from world (usual ImGui space)
// to virtual space and back usign CanvasFromWorld()/CanvasToWorld().
//
// Canvas can be nested in each other (they are regular widgets after all). There
// is a way to transform position between current and parent canvas with
// CanvasFromParent()/CanvasToParent().
//
// Sometimes in more elaborate scenarios you want to move out canvas virtual space,
// do something and came back. You can do that with SuspendCanvas() and ResumeCanvas().
//
// Note:
//     It is not valid to call canvas API outside of BeginCanvas() / EndCanvas() scope.
bool BeginCanvas(const char* str_id, const ImVec2& size = ImVec2(0.0f, 0.0f));
void EndCanvas(); // Call only when BeginCanvas() returned true
void CanvasSetView(const ImVec2& origin, float scale = 1.0f);
void CenterCanvasView(const ImVec2& virtualPoint);
void CenterCanvasView(const ImVec2& virtualMin, const ImVec2& virtualMax);
void SuspendCanvas();
void ResumeCanvas();
ImVec2 CanvasToParent(const ImVec2& point);
ImVec2 CanvasFromParent(const ImVec2& point);
ImVec2 CanvasToWorld(const ImVec2& point);
ImVec2 CanvasFromWorld(const ImVec2& point);
ImVec2 CanvasContentMin();
ImVec2 CanvasContentMax();
ImVec2 CanvasContentSize();
ImVec2 CanvasViewOrigin();
float CanvasViewScale();
ImVec2 CanvasViewMin();
ImVec2 CanvasViewMax();
ImVec2 CanvasViewSize();

} // namespace ImGuiEx
