# include "imguiex_internal.h"

ImGuiEx::Canvas::Canvas(ImGuiID id)
    : m_ID(id)
{
}

bool ImGuiEx::Canvas::Begin(Canvas* parent, const ImVec2& size)
{
    auto& io = ImGui::GetIO();

    m_Parent = parent;

    m_Size        = size;
    m_StartPos    = ImGui::GetCursorScreenPos();
    m_CurrentSize = ImSelectPositive(size, ImGui::GetContentRegionAvail());
    m_ContentRect = ImRect(m_StartPos, m_StartPos + m_CurrentSize);
    m_DrawList    = ImGui::GetWindowDrawList();

    auto bounding_rect = ImRect(m_StartPos, m_StartPos + m_CurrentSize);
    //if (parent)
    //    bounding_rect = m_View.LocalToWorld() * bounding_rect;
    if (ImGui::IsClippedEx(bounding_rect, m_ID, true))
        return false;

    // #debug: Canvas content.
    m_DrawList->AddRectFilled(m_StartPos, m_StartPos + m_CurrentSize, IM_COL32(0, 0, 0, 64));
    m_DrawList->AddRect(bounding_rect.Min, bounding_rect.Max, IM_COL32(255, 0, 255, 64));

    ImGui::SetCursorScreenPos(ImVec2(0.0f, 0.0f));

    SaveInputState();

    EnterLocalSpace();

    return true;
}

void ImGuiEx::Canvas::End()
{
    auto& io = ImGui::GetIO();

    LeaveLocalSpace();

    RestoreInputState();

    // Emit dummy widget matching bounds of the canvas.
    ImGui::SetCursorScreenPos(m_StartPos);
    ImGui::Dummy(m_CurrentSize);

    // #debug: Rect around canvas. Content should be inside these bounds.
    m_DrawList->AddRect(m_StartPos - ImVec2(1.0f, 1.0f), m_StartPos + m_CurrentSize + ImVec2(1.0f, 1.0f), IM_COL32(196, 0, 0, 255));
}

void ImGuiEx::Canvas::SetView(const ImVec2& origin, float scale)
{
    LeaveLocalSpace();

    m_View.Origin        = origin;
    m_View.RoundedOrigin = ImFloor(origin);
    m_View.Scale         = scale;
    m_View.InvScale      = scale ? 1.0f / scale : 0.0f;

    EnterLocalSpace();
}

void ImGuiEx::Canvas::SaveInputState()
{
    auto& io = ImGui::GetIO();
    m_MousePosBackup     = io.MousePos;
    m_MousePosPrevBackup = io.MousePosPrev;
    for (auto i = 0; i < IM_SIZE_OF_ARRAY(m_MouseClickedPosBackup); ++i)
        m_MouseClickedPosBackup[i] = io.MouseClickedPos[i];
}

void ImGuiEx::Canvas::RestoreInputState()
{
    auto& io = ImGui::GetIO();
    io.MousePos     = m_MousePosBackup;
    io.MousePosPrev = m_MousePosPrevBackup;
    for (auto i = 0; i < IM_SIZE_OF_ARRAY(m_MouseClickedPosBackup); ++i)
        io.MouseClickedPos[i] = m_MouseClickedPosBackup[i];
}

void ImGuiEx::Canvas::EnterLocalSpace()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::BeginGroup();
    ImGui::PopStyleVar();

    // Prepare ImDrawList for drawing in local coordinate system:
    //   - start unique draw command
    //   - add clip rect matching canvas size
    //   - record current command index
    //   - record current vertex write index

    // Make sure we do not share draw command with anyone. We don't want to mess
    // with someones clip rectangle.
    if (!m_DrawList->CmdBuffer.empty() && m_DrawList->CmdBuffer.back().ElemCount > 0)
        m_DrawList->AddDrawCmd();
    m_DrawListCommadBufferSize = ImMax(m_DrawList->CmdBuffer.Size - 1, 0);

    // Clip rectangle in parent canvas space and move it to local space.
    ImGui::PushClipRect(m_StartPos, m_StartPos + m_CurrentSize, true);
    auto clipped_clip_rect = m_DrawList->_ClipRectStack.back();
    ImGui::PopClipRect();
    clipped_clip_rect.x = (clipped_clip_rect.x - m_StartPos.x) - m_View.RoundedOrigin.x;
    clipped_clip_rect.y = (clipped_clip_rect.y - m_StartPos.y) - m_View.RoundedOrigin.y;
    clipped_clip_rect.z = (clipped_clip_rect.z - m_StartPos.x) - m_View.RoundedOrigin.x;
    clipped_clip_rect.w = (clipped_clip_rect.w - m_StartPos.y) - m_View.RoundedOrigin.y;
    if (m_View.InvScale > 1.0f)
    {
        clipped_clip_rect.x *= m_View.InvScale;
        clipped_clip_rect.y *= m_View.InvScale;
        clipped_clip_rect.z *= m_View.InvScale;
        clipped_clip_rect.w *= m_View.InvScale;
    }
    ImGui::PushClipRect(ImVec2(clipped_clip_rect.x, clipped_clip_rect.y), ImVec2(clipped_clip_rect.z, clipped_clip_rect.w), false);
    //ImGui::PushClipRect(ImVec2(0.0f, 0.0f), ImGui::GetIO().DisplaySize, false);

    m_DrawListStartVertexIndex = m_DrawList->_VtxCurrentIdx;

    // Transform mouse position to local space.
    auto& io = ImGui::GetIO();
    io.MousePos     = (m_MousePosBackup     - m_StartPos - m_View.RoundedOrigin) * m_View.InvScale;
    io.MousePosPrev = (m_MousePosPrevBackup - m_StartPos - m_View.RoundedOrigin) * m_View.InvScale;
    for (auto i = 0; i < IM_SIZE_OF_ARRAY(m_MouseClickedPosBackup); ++i)
        io.MouseClickedPos[i] = (m_MouseClickedPosBackup[i] - m_StartPos - m_View.RoundedOrigin) * m_View.InvScale;

    m_ViewRect.Min = ImVec2(0.0f, 0.0f) - m_View.RoundedOrigin * m_View.InvScale;
    m_ViewRect.Max = (m_CurrentSize - m_View.RoundedOrigin) * m_View.InvScale;
}

void ImGuiEx::Canvas::LeaveLocalSpace()
{
    // Move vertices to screen space.
    auto vertex    = m_DrawList->VtxBuffer.Data + m_DrawListStartVertexIndex;
    auto vertexEnd = m_DrawList->VtxBuffer.Data + m_DrawList->_VtxCurrentIdx;
    while (vertex < vertexEnd)
    {
        vertex->pos.x = vertex->pos.x * m_View.Scale + m_StartPos.x + m_View.RoundedOrigin.x;
        vertex->pos.y = vertex->pos.y * m_View.Scale + m_StartPos.y + m_View.RoundedOrigin.y;
        ++vertex;
    }

    // Move clip rectangles to screen space.
    for (int i = m_DrawListCommadBufferSize; i < m_DrawList->CmdBuffer.size(); ++i)
    {
        auto& command = m_DrawList->CmdBuffer[i];
        command.ClipRect.x += m_StartPos.x + m_View.RoundedOrigin.x;
        command.ClipRect.y += m_StartPos.y + m_View.RoundedOrigin.y;
        command.ClipRect.z += m_StartPos.x + m_View.RoundedOrigin.x;
        command.ClipRect.w += m_StartPos.y + m_View.RoundedOrigin.y;
    }

    // And pop \o/
    ImGui::PopClipRect();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::GetCurrentWindow()->DC.GroupStack.back().AdvanceCursor = false;
    ImGui::EndGroup();
    ImGui::PopStyleVar();
}
