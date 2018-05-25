# pragma once
# include "imguiex.h"
# include "imgui_internal.h"

static inline ImVec2 ImSelectPositive(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x > 0.0f ? lhs.x : rhs.x, lhs.y > 0.0f ? lhs.y : rhs.y); }

# if defined(IMGUI_DEFINE_MATH_OPERATORS)
static inline ImVec2   operator*(const float lhs, const ImVec2& rhs)              { return ImVec2(lhs*rhs.x, lhs*rhs.y); }
static inline ImVec2   operator-(const ImVec2& lhs)                               { return ImVec2(-lhs.x, -lhs.y); }
static inline ImVec2   operator*(const ImMatrix& lhs, const ImVec2& rhs)          { return lhs.Transformed(rhs); }
static inline ImRect   operator*(const ImMatrix& lhs, const ImRect& rhs)
{
    ImVec2 points[4] =
    {
        rhs.Min,
        ImVec2(rhs.Max.x, rhs.Min.y),
        rhs.Max,
        ImVec2(rhs.Min.x, rhs.Max.y)
    };

    lhs.Transform(points, 4);

    ImRect out(points[0], points[0]);
    for (int i = 1; i < 4; ++i)
    {
        out.Min = ImMin(out.Min, points[i]);
        out.Max = ImMax(out.Max, points[i]);
    }

    return out;
}
# endif

namespace ImGuiEx {

struct Canvas;

struct Context
{
    static Context* Create();
    static void     Destroy(Context* context = nullptr);
    static Context* GetCurrent();
    static void     SetCurrent(Context* context);

    ~Context();

    Canvas* GetCanvas(ImGuiID id);
    void    SetCurrentCanvas(Canvas* canvas) { m_CurrentCanvas = canvas; }
    Canvas* GetCurrentCanvas()               { return m_CurrentCanvas; }
    void    PushCanvas(Canvas* canvas);
    void    PopCanvas();
    bool    IsFirstCanvas(ImGuiID id);


private:
    Canvas*             m_CurrentCanvas = nullptr;
    ImVector<Canvas*>   m_CanvasStack;
    ImGuiStorage        m_CanvaInstances;
};

struct CurrentContextPtr
{
    CurrentContextPtr()
        : m_Context(Context::GetCurrent())
    {
        IM_ASSERT(m_Context);
    }

    CurrentContextPtr(const CurrentContextPtr&) = delete;
    CurrentContextPtr(CurrentContextPtr&&) = delete;
    CurrentContextPtr& operator=(const CurrentContextPtr&) = delete;
    CurrentContextPtr& operator=(CurrentContextPtr&&) = delete;

          Context* operator->()       { return m_Context; }
    const Context* operator->() const { return m_Context; }

          Context* Get()       { return m_Context; }
    const Context* Get() const { return m_Context; }

private:
    Context* m_Context;
};

} // namespace ImGuiExtras
