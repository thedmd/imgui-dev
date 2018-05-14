# define IMGUI_DEFINE_MATH_OPERATORS
# include "imguiex_internal.h"

static ImGuiEx::Context* g_Context = nullptr;

ImGuiEx::Context* ImGuiEx::Context::Create()
{
    auto context = new Context;
    if (GetCurrent() == nullptr)
        SetCurrent(context);
    return context;
}

void ImGuiEx::Context::Destroy(Context* context)
{
    auto current_context = GetCurrent();
    if (context == nullptr)
        context = current_context;
    if (context == current_context)
        SetCurrent(nullptr);
    delete context;
}

ImGuiEx::Context* ImGuiEx::Context::GetCurrent()
{
    return g_Context;
}

void ImGuiEx::Context::SetCurrent(Context* context)
{
    g_Context = context;
}

ImGuiEx::Context::~Context()
{
    for (auto& entry : m_CanvaInstances.Data)
        delete reinterpret_cast<Canvas*>(entry.val_p);
}

ImGuiEx::Canvas* ImGuiEx::Context::GetCanvas(ImGuiID id)
{
    auto ptr = reinterpret_cast<Canvas*>(m_CanvaInstances.GetVoidPtr(id));

    if (ptr == nullptr)
    {
        ptr = new Canvas(id);
        m_CanvaInstances.SetVoidPtr(id, ptr);
    }

    return ptr;
}

void ImGuiEx::Context::PushCanvas(Canvas* canvas)
{
    m_CanvasStack.push_back(m_CurrentCanvas);
    SetCurrentCanvas(canvas);
}

void ImGuiEx::Context::PopCanvas()
{
    SetCurrentCanvas(m_CanvasStack.back());
    m_CanvasStack.pop_back();
}

bool ImGuiEx::Context::IsFirstCanvas(ImGuiID id)
{
    for (auto& canvas : m_CanvasStack)
    {
        if (!canvas)
            continue;

        return canvas->GetID() == id;
    }

    return m_CurrentCanvas && m_CurrentCanvas->GetID() == id;
}
