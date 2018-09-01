# include "NodeEditor_Internal.h"
# include "Editor.h"
# include "Utilities/Drawing.h"

ax::NodeEditor::Editor::Editor(const char* str_id)
    : m_Id(ImHash(str_id, 0))
    , m_Canvas(m_Id)
    , m_BackgroundCanvas(CanvasId(0))
    , m_NavigateAction(*this)
    , m_SelectAction(*this)
    , m_DragNodeAction(*this)
{
}

bool ax::NodeEditor::Editor::Begin(const ImVec2& size)
{
    if (!m_Canvas.Begin(nullptr, size))
        return false;

    m_CanvasView        = m_Canvas.View();
    m_CanvasContentView = m_Canvas.ContentView();

    m_InputState = BuildInputState();

    // Navigate action do not collide with remaining ones.
    // If that will be the case in the future consider splitting action
    // into lines where they can be easily conditionally parallelized.
    m_NavigateAction.Update(m_InputState);

    ProcessActions(m_InputState);

    ResetLiveObjects();

    m_BackgroundCanvas.Touch();

    m_Canvas.Suspend();
    auto drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        m_Canvas.ContentRect().Min,
        m_Canvas.ContentRect().Max,
        c_ConfigBackgroundColor);
    Drawing::Grid(*drawList,
        m_Canvas.ContentRect(),
        m_Canvas.ViewOrigin(),
        c_ConfigGridSize * m_Canvas.ViewScale(),
        c_ConfigGridColor);
    m_Canvas.Resume();

    DrawActions();

    return true;
}

void ax::NodeEditor::Editor::End()
{
    m_Canvas.End();

    // #debug
    ImGui::GetWindowDrawList()->AddRect(
        m_Canvas.ContentRect().Min - ImVec2(1, 1),
        m_Canvas.ContentRect().Max + ImVec2(1, 1),
        IM_COL32(255, 255, 255, 255));
}

void ax::NodeEditor::Editor::NavigateTo(const ImVec2& point, bool immediate)
{
    m_Canvas.CenterView(point);
}

void ax::NodeEditor::Editor::NavigateTo(const ImRect& rect, bool immediate)
{
    auto rectSize = rect.GetSize();
    auto viewRect = m_Canvas.ViewRect();
    auto targetRect = ImRect(
        rectSize.x > 0 ? rect.Min.x : viewRect.Min.x,
        rectSize.y > 0 ? rect.Min.y : viewRect.Min.y,
        rectSize.x > 0 ? rect.Max.x : viewRect.Max.x,
        rectSize.y > 0 ? rect.Max.y : viewRect.Max.y
    );

    m_Canvas.CenterView(targetRect);

    m_CanvasView        = m_Canvas.View();
    m_CanvasContentView = m_Canvas.ContentView();
    //m_CanvasView.Origin        += m_Canvas.ContentRect().Min;
    //m_CanvasView.RoundedOrigin += m_Canvas.ContentRect().Min;
}

void ax::NodeEditor::Editor::SetView(const ImVec2& origin, float scale)
{
    //m_NextCanvasView.Set(origin, scale);

    m_Canvas.SetView(origin, scale);
    m_CanvasView        = m_Canvas.View();
    m_CanvasContentView = m_Canvas.ContentView();
}

void ax::NodeEditor::Editor::SetView(const ImGuiEx::CanvasView& view)
{
    //m_NextCanvasView.Set(view.Origin, view.Scale);

    m_Canvas.SetView(view);
    m_CanvasView        = m_Canvas.View();
    m_CanvasContentView = m_Canvas.ContentView();
}

const ImGuiEx::CanvasView& ax::NodeEditor::Editor::View() const
{
    return m_CanvasView;
}

const ImGuiEx::CanvasView& ax::NodeEditor::Editor::ContentView() const
{
    return m_CanvasContentView;
}

void ax::NodeEditor::Editor::SuspendView()
{
    m_Canvas.Suspend();
    ImGui::GetWindowDrawList()->PushClipRect(
        m_Canvas.ContentRect().Min, m_Canvas.ContentRect().Max, true);
}

void ax::NodeEditor::Editor::ResumeView()
{
    ImGui::GetWindowDrawList()->PopClipRect();
    m_Canvas.Resume();
}

void ax::NodeEditor::Editor::Select(Object* object, SelectOperation operation)
{
    m_Selection.Select(object, operation);
}

void ax::NodeEditor::Editor::Select(const ImVector<Object*>& objects, SelectOperation operation)
{
    m_Selection.Select(objects, operation);
}

void ax::NodeEditor::Editor::DeselectAll()
{
    m_Selection.DeselectAll();
}

const ax::NodeEditor::Selection& ax::NodeEditor::Editor::SelectedObjects() const
{
    return m_Selection;
}

ImVector<ax::NodeEditor::Object*> ax::NodeEditor::Editor::FindObjectsInside(const ImRect& rect, ObjectTypes mask)
{
    ImVector<ax::NodeEditor::Object*> result;

    if ((mask & ObjectTypes::Pins) == ObjectTypes::Pins)
    {
    }

    if ((mask & ObjectTypes::Nodes) == ObjectTypes::Nodes)
    {
        for (auto node : m_Nodes.View())
            if (rect.Overlaps(node->m_Bounds))
                result.push_back(node);
    }

    if ((mask & ObjectTypes::Links) == ObjectTypes::Links)
    {
    }

    return result;
}

ax::NodeEditor::NodeBuilder ax::NodeEditor::Editor::BuildNode(NodeId id)
{
    IM_ASSERT(id != NodeId::Invalid);

    return NodeBuilder(*this, *m_Nodes.Get(id));
}

void ax::NodeEditor::Editor::Debug(bool inWindow)
{
    auto highlightNode = [this](Node* node, ImU32 color)
    {
        if (!node)
            return;

        auto worldBounds     = ImRect(m_Canvas.ToWorld(node->m_Bounds.Min), m_Canvas.ToWorld(node->m_Bounds.Max));
        auto overlayDrawList = ImGui::GetOverlayDrawList();
        overlayDrawList->AddRect(worldBounds.Min, worldBounds.Max, color);
    };

    if (m_InputState.Object.Clicked)
        m_DebugLastClicked = m_InputState.Object.Clicked;
    else if (m_DebugLastClicked && !m_DebugLastClicked->m_IsLive)
        m_DebugLastClicked = nullptr;

    if (m_InputState.Object.DoubleClicked)
        m_DebugLastDoubleClicked = m_InputState.Object.DoubleClicked;
    else if (m_DebugLastDoubleClicked && !m_DebugLastDoubleClicked->m_IsLive)
        m_DebugLastDoubleClicked = nullptr;

    if (!inWindow || ImGui::Begin("Debug##NodeEditorDebug", nullptr))
    {
        ImGui::TextUnformatted("Input State");
        ImGui::Indent();
        ImGui::Text("Mouse: %s", Debug::ToString(m_InputState.MousePosition).c_str());
        ImGui::Text("Hovered: %s", Debug::ToString(m_InputState.Object.Hovered).c_str());
        ImGui::Text("Active: %s", Debug::ToString(m_InputState.Object.Active).c_str());
        ImGui::Text("Clicked: %s", Debug::ToString(m_InputState.Object.Clicked).c_str());
        ImGui::Text("Double Clicked: %s", Debug::ToString(m_InputState.Object.DoubleClicked).c_str());
        ImGui::Text("Last Clicked: %s", Debug::ToString(m_DebugLastClicked).c_str());
        ImGui::Text("Last Double Clicked: %s", Debug::ToString(m_DebugLastDoubleClicked).c_str());
        ImGui::Text("Modifiers: %s", Debug::ToString(m_InputState.Modifiers).c_str());
        ImGui::Unindent();
        ImGui::TextUnformatted("Stats");
        ImGui::Indent();
        ImGui::Text("View: %s", Debug::ToString(m_Canvas.ViewRect()).c_str());
        ImGui::Text("Current Action: %s", m_CurrentAction ? m_CurrentAction->Name() : "---");
        ImGui::Text("Possible Action: %s", m_PossibleAction ? m_PossibleAction->Name() : "---");
        ImGui::Text("Pins: %d", m_Pins.Count());
        ImGui::Text("Nodes: %d", m_Nodes.Count());
        ImGui::Text("Links: %d", m_Links.Count());
        ImGui::Unindent();

        char selectionHeader[64];
        ImFormatString(selectionHeader, 64, "Selection (%d)##Selection", m_Selection.Objects.Size);
        if (ImGui::CollapsingHeader(selectionHeader, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();
            for (auto& selection : m_Selection.Objects)
            {
                ImGui::Text("%s", Debug::ToString(selection).c_str());
                //auto highlight = ImGui::IsItemHovered();
                //if (highlight)
                //    highlightNode(node, IM_COL32(255, 0, 0, 255));
            }
            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("Nodes", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto& node : m_Nodes.View())
            {
                auto expand = ImGui::TreeNode(node->m_Id.AsPointer(), "[%p] %s", node->m_Id.AsPointer(), node->m_IsLive ? "Live" : "Dead");
                auto highlight = ImGui::IsItemHovered();
                if (expand)
                {
                    ImGui::Text("Bounds: %s", Debug::ToString(node->m_Bounds).c_str());
                    ImGui::TreePop();
                }

                if (highlight)
                    highlightNode(node, IM_COL32(255, 0, 0, 255));
            }
        }
        if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto action : m_Actions)
            {
                if (ImGui::CollapsingHeader(action->Name(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Indent();
                    action->Debug();
                    ImGui::Unindent();
                }
            }
        }
    }

    if (inWindow)
        ImGui::End();
}

ax::NodeEditor::InputState ax::NodeEditor::Editor::BuildInputState()
{
    InputState result;

    result.MousePosition = ImGui::GetIO().MousePos;

    //if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    //    return result;

    struct WidgetState
    {
        bool Hovered       = false;
        bool Active        = false;
        bool Clicked       = false;
        bool DoubleClicked = false;
    };

    auto apply = [](const WidgetState& state, auto& item, auto* object)
    {
        if (!item.Hovered       && state.Hovered)       item.Hovered       = object;
        if (!item.Active        && state.Active)        item.Active        = object;
        if (!item.Clicked       && state.Clicked)       item.Clicked       = object;
        if (!item.DoubleClicked && state.DoubleClicked) item.DoubleClicked = object;
    };

    auto widget = [&apply, &result](const ImRect& bounds, ImGuiID id, auto& inputItem, auto* object)
    {
        if (ImGui::IsClippedEx(bounds, id, false))
            return;

        WidgetState state;

        bool pressed = ImGui::ButtonBehavior(bounds, id, &state.Hovered, &state.Active,
            ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick);

        bool isMouseDoubleClicked = ImGui::IsMouseDoubleClicked(0);
        state.Clicked       = pressed && !isMouseDoubleClicked;
        state.DoubleClicked = pressed && isMouseDoubleClicked;

        apply(state, inputItem, object);
        apply(state, result.Object, object);
    };

    auto window = ImGui::GetCurrentWindow();

    ImGui::PushID(m_Id);

    for (auto& node : m_Nodes.View())
    {
        if (!node->m_IsLive)
            continue;

        auto& bounds = node->m_Bounds;
        auto  id     = window->GetID(node->m_Id.AsPointer());

        widget(bounds, id, result.Node, node);
    }

    widget(m_Canvas.ViewRect(), window->GetID(m_BackgroundCanvas.m_Id.AsPointer()), result.Canvas, &m_BackgroundCanvas);

    ImGui::PopID();

    auto& io = ImGui::GetIO();
    if (io.KeyCtrl)  result.Modifiers |= KeyModifers::Ctrl;
    if (io.KeyShift) result.Modifiers |= KeyModifers::Shift;
    if (io.KeyAlt)   result.Modifiers |= KeyModifers::Alt;
    if (io.KeySuper) result.Modifiers |= KeyModifers::Super;

    return result;
}

void ax::NodeEditor::Editor::ProcessActions(const InputState& inputState)
{
    if (m_CurrentAction && !m_CurrentAction->Process(inputState))
        m_CurrentAction = nullptr;

    if (nullptr != m_CurrentAction)
        return;

    Action* possibleAction = nullptr;
    Action* nextAction     = nullptr;

    for (auto& action : m_Actions)
    {
        auto response = action->Accept(inputState);
        if (response == Action::Yes)
        {;
            nextAction = action;
            break;
        }
        else if (response == Action::Possible)
        {
            if (!possibleAction)
                possibleAction = action;
        }
    }

    if (nextAction)
        m_CurrentAction = nextAction;

    m_PossibleAction = possibleAction;
}

void ax::NodeEditor::Editor::DrawActions()
{
    for (auto action : m_Actions)
        action->Draw();
}

void ax::NodeEditor::Editor::ResetLiveObjects()
{
    m_Pins.Reset();
    m_Nodes.Reset();
    m_Links.Reset();
    m_BackgroundCanvas.Reset();
}
