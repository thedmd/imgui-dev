# include "imguiex_internal.h"

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

    canvas->End();

    context->PopCanvas();
}

void ImGuiEx::CanvasView(const ImVec2& origin, float scale)
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    canvas->SetView(origin, scale);
}

ImVec2 ImGuiEx::CanvasContentMin()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ContentRect().Min;
}

ImVec2 ImGuiEx::CanvasContentMax()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ContentRect().Max;
}

ImVec2 ImGuiEx::CanvasContentSize()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ContentRect().GetSize();
}

ImVec2 ImGuiEx::CanvasViewMin()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ViewRect().Min;
}

ImVec2 ImGuiEx::CanvasViewMax()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ViewRect().Max;
}

ImVec2 ImGuiEx::CanvasViewSize()
{
    CurrentContextPtr context;

    auto canvas = context->GetCurrentCanvas();

    return canvas->ViewRect().GetSize();
}