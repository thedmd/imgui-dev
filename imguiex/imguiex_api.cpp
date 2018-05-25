# define IMGUI_DEFINE_MATH_OPERATORS
# include "imguiex_internal.h"
# include "imguiex_canvas.h"

ImGuiEx::Context* ImGuiEx::CreateContext()
{
    return Context::Create();
}

void ImGuiEx::DestroyContext(Context* context)
{
    Context::Destroy(context);
}

ImGuiEx::Context* ImGuiEx::GetCurrentContext()
{
    return Context::GetCurrent();
}

void ImGuiEx::SetCurrentContext(Context* context)
{
    Context::SetCurrent(context);
}

bool ImGuiEx::BeginCanvas(const char* str_id, const ImVec2& size)
{
    auto id = ImHash(str_id, 0);

    CurrentContextPtr context;

    auto canvas = context->GetCanvas(id);

    auto parent_canvas = context->GetCurrentCanvas();

    context->PushCanvas(canvas);

    if (!canvas->Begin(parent_canvas, size))
    {
        context->PopCanvas();
        return false;
    }

    return true;
}

void ImGuiEx::EndCanvas()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->End();

    context->PopCanvas();
}

void ImGuiEx::CanvasSetView(const ImVec2& origin, float scale)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->SetView(origin, scale);
}

void ImGuiEx::CenterCanvasView(const ImVec2& virtualPoint)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->CenterView(virtualPoint);
}

void ImGuiEx::CenterCanvasView(const ImVec2& virtualMin, const ImVec2& virtualMax)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->CenterView(ImRect(virtualMin, virtualMax));
}

void ImGuiEx::SuspendCanvas()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->Suspend();
}

void ImGuiEx::ResumeCanvas()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    canvas->Resume();
}

ImVec2 ImGuiEx::CanvasToParent(const ImVec2& point)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ToParent(point);
}

ImVec2 ImGuiEx::CanvasFromParent(const ImVec2& point)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->FromParent(point);
}

ImVec2 ImGuiEx::CanvasToWorld(const ImVec2& point)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ToWorld(point);
}

ImVec2 ImGuiEx::CanvasFromWorld(const ImVec2& point)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->FromWorld(point);
}

ImVec2 ImGuiEx::CanvasContentMin()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ContentRect().Min;
}

ImVec2 ImGuiEx::CanvasContentMax()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ContentRect().Max;
}

ImVec2 ImGuiEx::CanvasContentSize()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ContentRect().GetSize();
}

ImVec2 ImGuiEx::CanvasViewOrigin()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ViewOrigin();
}

float ImGuiEx::CanvasViewScale()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ViewScale();
}

ImVec2 ImGuiEx::CanvasViewMin()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ViewRect().Min;
}

ImVec2 ImGuiEx::CanvasViewMax()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ViewRect().Max;
}

ImVec2 ImGuiEx::CanvasViewSize()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    IM_ASSERT(canvas != nullptr);

    return canvas->ViewRect().GetSize();
}