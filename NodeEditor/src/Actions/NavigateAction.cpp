# include "NodeEditor_Internal.h"
# include "Actions.h"
# include "Editor.h"

static const float c_ConfigZoomMatchTolerance = ImSqrt(FLT_EPSILON);
static const float c_ConfigZoomLevels[] =
{
    0.10f, 0.15f, 0.20f, 0.25f, 0.33f, 0.50f,
    0.75f, 1.00f, 1.25f, 1.50f, 2.00f, 2.50f,
    3.00f, 4.00f, 5.00f, 6.00f, 7.00f, 8.00f
};
static const int   c_ConfigZoomLevelCount = sizeof(c_ConfigZoomLevels) / sizeof(*c_ConfigZoomLevels);

ax::NodeEditor::Action::Result ax::NodeEditor::NavigateAction::Accept(const InputState& inputState)
{
    IM_ASSERT(!m_IsActive);

    auto& io = ImGui::GetIO();

    if (ImGui::IsMouseDragging(c_ConfigScrollButtonIndex, 0.0f))
    {
        m_IsActive          = true;
        m_InitialView       = m_Editor.View();
        m_LastMousePosition = m_InitialView.ToWorld(io.MouseClickedPos[c_ConfigScrollButtonIndex]);

        return Yes;
    }

    if (io.MouseWheel != 0.0f)
    {
        auto view           = m_Editor.View();
        auto worldZoomPoint = view.ToWorld(io.MousePos);
        ApplyZoom(view, worldZoomPoint, WheelToZoomDirection(io.MouseWheel));
        m_Editor.SetView(view.Origin, view.Scale);
    }

    if (inputState.Canvas.Hovered)
        return Possible;

    return No;
}

bool ax::NodeEditor::NavigateAction::Process(const InputState& inputState)
{
    IM_ASSERT(m_IsActive);

    if (ImGui::IsMouseDragging(c_ConfigScrollButtonIndex, 0.0f))
    {
        auto& io = ImGui::GetIO();

        auto view       = m_Editor.View();
        m_MousePosition = view.ToWorld(io.MousePos);

        if (io.MouseWheel != 0.0f)
            ApplyZoom(view, m_MousePosition, WheelToZoomDirection(io.MouseWheel));

        m_MouseDragDelta = (m_MousePosition - m_LastMousePosition);
        m_LastMousePosition = m_MousePosition;

        view.SetOrigin(view.Origin + m_MouseDragDelta);

        m_Editor.SetView(view);
    }
    else
        Dismiss();

    return m_IsActive;
}

void ax::NodeEditor::NavigateAction::Dismiss()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
}

void ax::NodeEditor::NavigateAction::Cancel()
{
    IM_ASSERT(m_IsActive);

    m_IsActive = false;
    m_Editor.SetView(m_InitialView);
}

void ax::NodeEditor::NavigateAction::Debug()
{
    ImGui::Text("Mouse Position: %s", Debug::ToString(m_MousePosition).c_str());
    ImGui::Text("Mouse Delta: %s", Debug::ToString(m_MouseDragDelta).c_str());
    ImGui::Text("Last Mouse Position: %s", Debug::ToString(m_LastMousePosition).c_str());
    ImGui::Text("Initial View: %s", Debug::ToString(m_InitialView).c_str());
}

void ax::NodeEditor::NavigateAction::Update(const InputState& inputState)
{
    if (m_IsActive)
        Process(inputState);
    else
        Accept(inputState);
}

float ax::NodeEditor::NavigateAction::NextZoomLevel(float currentZoom, ZoomDirection direction) const
{
    int   closestZoomIndex    = -1;
    float closestZoomDistance = FLT_MAX;

    for (int i = 0; i < c_ConfigZoomLevelCount; ++i)
    {
        auto distance = ImFabs(c_ConfigZoomLevels[i] - currentZoom);
        if (distance < closestZoomDistance)
        {
            closestZoomDistance = distance;
            closestZoomIndex    = i;
        }
    }

    if (direction == ZoomDirection::Match || (ImFabs(c_ConfigZoomLevels[closestZoomIndex] - closestZoomDistance) <= c_ConfigZoomMatchTolerance))
        return c_ConfigZoomLevels[closestZoomIndex];

    if (direction == ZoomDirection::In)
        closestZoomIndex = ImMin(closestZoomIndex + 1, c_ConfigZoomLevelCount - 1);
    else
        closestZoomIndex = ImMax(closestZoomIndex - 1, 0);

    return c_ConfigZoomLevels[closestZoomIndex];
}

void ax::NodeEditor::NavigateAction::ApplyZoom(ImGuiEx::CanvasView& view, const ImVec2& worldZoomPoint, ZoomDirection direction) const
{
    auto newScale = NextZoomLevel(view.Scale, direction);

    auto localPosition = view.ToLocal(worldZoomPoint);
    view.SetScale(newScale);
    auto worldPosition = view.ToWorld(localPosition);
    auto originOffset  = worldZoomPoint - worldPosition;
    view.SetOrigin(view.Origin + originOffset);
}

ax::NodeEditor::NavigateAction::ZoomDirection ax::NodeEditor::NavigateAction::WheelToZoomDirection(float mouseWheel) const
{
    if (mouseWheel > 0.0f)
        return ZoomDirection::In;
    else if (mouseWheel < 0.0f)
        return ZoomDirection::Out;
    else
        return ZoomDirection::Match;
}