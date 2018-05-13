# pragma once
# include "imguiex.h"
# pragma push_macro("IMGUI_DEFINE_MATH_OPERATORS")
# if !defined(IMGUI_DEFINE_MATH_OPERATORS)
#     define IMGUI_DEFINE_MATH_OPERATORS
# endif
# include "imgui_internal.h"
# pragma pop_macro("IMGUI_DEFINE_MATH_OPERATORS")

namespace ImGuiEx {

# define IM_SIZE_OF_ARRAY(xxx)   (sizeof(xxx) / sizeof(*(xxx)))

static inline ImVec2 ImSelectPositive(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x > 0.0f ? lhs.x : rhs.x, lhs.y > 0.0f ? lhs.y : rhs.y); }

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

struct Canvas
{
    struct View
    {
        ImVec2 Origin        = ImVec2(0.0f, 0.0f);
        ImVec2 RoundedOrigin = ImVec2(0.0f, 0.0f);
        float  Scale         = 1.0f;
        float  InvScale      = 1.0f;

        ImMatrix LocalToWorld() const { return ImMatrix(Scale,    0.0f, 0.0f,    Scale,  RoundedOrigin.x,  RoundedOrigin.y); }
        ImMatrix WorldToLocal() const { return ImMatrix(InvScale, 0.0f, 0.0f, InvScale, -RoundedOrigin.x, -RoundedOrigin.y); }
    };

    Canvas(ImGuiID id);

    ImGuiID GetID() const { return m_ID; }

    // Returns false if canvas is not visible.
    bool Begin(Canvas* parent, const ImVec2& size = ImVec2(0, 0));
    void End();

    void SetView(const ImVec2& origin, float scale = 1.0f);

    const ImRect& ContentRect() const { return m_ContentRect; }
    const ImRect& ViewRect()    const { return m_ViewRect; }

private:
    void SaveInputState();
    void RestoreInputState();

    void EnterLocalSpace();
    void LeaveLocalSpace();

    Canvas* m_Parent = nullptr;

    ImGuiID m_ID;
    ImVec2 m_Size; // Size as user passed in
    ImVec2 m_StartPos;
    ImVec2 m_CurrentSize;
    ImRect m_ContentRect;

    ImDrawList* m_DrawList = nullptr;
    int m_DrawListCommadBufferSize;
    int m_DrawListStartVertexIndex;

    View       m_View;
    ImRect     m_ViewRect;

    ImVec2 m_MousePosBackup;
    ImVec2 m_MousePosPrevBackup;
    ImVec2 m_MouseClickedPosBackup[IM_SIZE_OF_ARRAY(ImGuiIO::MouseClickedPos)];
};

} // namespace ImGuiExtras
