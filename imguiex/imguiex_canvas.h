# pragma once
# include "imguiex_internal.h"

namespace ImGuiEx {

struct Canvas
{
    struct View
    {
        ImVec2 Origin        = ImVec2(0.0f, 0.0f);
        ImVec2 RoundedOrigin = ImVec2(0.0f, 0.0f);
        float  Scale         = 1.0f;
        float  InvScale      = 1.0f;
    };

    Canvas(ImGuiID id);

    ImGuiID GetID() const { return m_ID; }

    // Returns false if canvas is not visible.
    bool Begin(Canvas* parent, const ImVec2& size = ImVec2(0, 0));
    void End();

    void SetView(const ImVec2& origin, float scale = 1.0f);

    void Suspend();
    void Resume();

    ImVec2 ToParent(const ImVec2& point) const;
    ImVec2 FromParent(const ImVec2& point) const;

    ImVec2 ToWorld(const ImVec2& point) const;
    ImVec2 FromWorld(const ImVec2& point) const;

    const ImRect& ContentRect() const { return m_ContentRect; }
    const ImRect& ViewRect()    const { return m_ViewRect; }
    const ImVec2& ViewOrigin()  const { return m_View.Origin; }
    float         ViewScale()   const { return m_View.Scale; }
    bool          IsSuspended() const { return m_SuspendCounter > 0; }

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

    int m_SuspendCounter = 0;

    ImVec2 m_MousePosBackup;
    ImVec2 m_MousePosPrevBackup;
    ImVec2 m_MouseClickedPosBackup[IM_SIZE_OF_ARRAY(ImGuiIO::MouseClickedPos)];
    ImVec2 m_WindowCursorMaxBackup;
};

} // namespace ImGuiExtras
