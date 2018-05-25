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
    IM_ASSERT(m_IsActive == false);

    if (inputState.Canvas.Active && ImGui::IsMouseDragging(c_ConfigScrollButtonIndex, 0.0f))
    {
        auto& io = ImGui::GetIO();

        m_IsActive              = true;
        m_InitialView           = m_Editor.View();
        m_CurrentView           = m_InitialView;
        m_InitialClickPosition  = m_Editor.View().ToWorld(io.MouseClickedPos[c_ConfigScrollButtonIndex]);
        m_CurrentClickPosition  = m_InitialClickPosition;

        return Yes;
    }
    else if (inputState.Canvas.Hovered && !inputState.Object.Active)
        return Possible;

    return No;
}

bool ax::NodeEditor::NavigateAction::Process(const InputState& inputState)
{
    IM_ASSERT(m_IsActive == true);

    if (ImGui::IsMouseDragging(c_ConfigScrollButtonIndex, 0.0f))
    {
        auto& io = ImGui::GetIO();

        m_MousePosition = m_Editor.View().ToWorld(io.MousePos);
        m_CurrentView   = m_Editor.View();

        if (io.MouseWheel != 0.0f)
        {
            auto newScale = NextZoomLevel(m_CurrentView.Scale, io.MouseWheel > 0.0f ? ZoomDirection::In : ZoomDirection::Out);

            ////auto relativeScale = m_CurrentScale / newScale;
            //auto origin = m_Editor.Canvas().ToWorld(m_CurrentViewOrigin);

            //m_CurrentViewOrigin = (origin - m_InitialViewOrigin) * (newScale / m_InitialViewScale) + origin;

            ////viewport.Translate(-io.MousePos);
            ////viewport.Min = viewport.Min * relativeScale;
            ////viewport.Max = viewport.Max * relativeScale;
            ////viewport.Translate(io.MousePos);
            //m_CurrentViewScale = newScale;

            

            //auto localClickPos = m_InitialView.ToLocal(m_MousePosition);
            //m_CurrentView.Set(m_CurrentView.Origin, newScale);
            //auto worldClickPos = m_CurrentView.ToWorld(localClickPos);

            //m_CurrentClickPosition = m_InitialClickPosition + (worldClickPos - m_MousePosition);
        }

        m_LastDelta = (m_MousePosition - m_CurrentClickPosition);
        //auto delta2 = ImGui::GetMouseDragDelta(c_ConfigScrollButtonIndex);

        //auto viewport = m_Editor.Canvas().ViewRect();

        //auto& io = ImGui::GetIO();
        //if (io.MouseWheel != 0.0f)
        //{
        //    m_CurrentScale = NextZoomLevel(m_CurrentScale, io.MouseWheel > 0.0f ? ZoomDirection::In : ZoomDirection::Out);

        //    auto relativeScale = m_CurrentScale / m_InitialScale;

        //    viewport.Translate(-io.MousePos);
        //    viewport.Min = viewport.Min * relativeScale;
        //    viewport.Max = viewport.Max * relativeScale;
        //    viewport.Translate(io.MousePos);
        //}

        //m_CurrentView.Set(m_CurrentView.Origin + delta, m_CurrentView.Scale);

        //viewport.Translate(-delta);

        //m_Editor.NavigateTo(viewport);

        m_Editor.SetView(m_CurrentView.Origin + m_LastDelta, m_CurrentView.Scale);
    }
    else
    {
        //m_Editor.NavigateTo(m_StartViewport);

        m_IsActive = false;
    }

    return m_IsActive;
}

void ax::NodeEditor::NavigateAction::Dismiss()
{
    if (m_IsActive)
    {
        m_IsActive = false;
        //m_Editor.NavigateTo(m_V)
    }
}

void ax::NodeEditor::NavigateAction::Debug()
{
    ImGui::Text("Mouse Position: %s", Debug::ToString(m_MousePosition).c_str());
    ImGui::Text("Initial View: %s", Debug::ToString(m_InitialView).c_str());
    ImGui::Text("Initial Click Position: %s", Debug::ToString(m_InitialClickPosition).c_str());
    ImGui::Text("Current View: %s", Debug::ToString(m_CurrentView).c_str());
    ImGui::Text("Current Click Position: %s", Debug::ToString(m_CurrentClickPosition).c_str());
    ImGui::Text("Delta: %s", Debug::ToString(m_LastDelta).c_str());
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
