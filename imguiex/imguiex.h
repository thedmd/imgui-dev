# pragma once
# include "imgui.h"

namespace ImGuiEx {

struct Context;

Context* CreateContext();
void     DestroyContext(Context* context = nullptr); // NULL = destroy current context
Context* GetCurrentContext();
void     SetCurrentContext(Context* context);

bool BeginCanvas(const char* str_id, const ImVec2& size = ImVec2(0.0f, 0.0f));
void EndCanvas(); // Call only when BeginCanvas() returned true
void CanvasView(const ImVec2& origin, float scale = 1.0f);
ImVec2 CanvasContentMin();
ImVec2 CanvasContentMax();
ImVec2 CanvasContentSize();
ImVec2 CanvasViewMin();
ImVec2 CanvasViewMax();
ImVec2 CanvasViewSize();

} // namespace ImGuiEx
