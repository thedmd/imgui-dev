# define IMGUI_DEFINE_MATH_OPERATORS
# include "NodeEditor.h"
# include "imgui_internal.h"
# include "imguiex_internal.h"

ax::NodeEditor::NodeBuilder::NodeBuilder(Editor& editor, Node& node)
    : m_Editor(editor)
    , m_Node(node)
{
    m_Node.Touch();

    ImGui::SetCursorScreenPos(m_Node.m_Bounds.Min);

    ImGui::BeginGroup();
}


ax::NodeEditor::NodeBuilder::~NodeBuilder()
{
    ImGui::EndGroup();

    m_Node.m_Bounds.Min = ImGui::GetItemRectMin();
    m_Node.m_Bounds.Max = ImGui::GetItemRectMax();


}





ax::NodeEditor::Editor::Editor(const char* str_id)
    : m_Id(ImHash(str_id, 0))
    , m_Canvas(m_Id)
    , m_BackgroundCanvas(CanvasId(0))
    , m_NavigateAction(*this)
{
}

bool ax::NodeEditor::Editor::Begin(const ImVec2& size)
{
    if (!m_Canvas.Begin(nullptr, size))
        return false;

    m_Viewport = m_Canvas.ViewRect();

    m_BackgroundCanvas.Touch();

    // #debug
    ImGui::GetWindowDrawList()->AddRectFilled(
        m_Canvas.ViewRect().Min,
        m_Canvas.ViewRect().Max,
        IM_COL32(48, 48, 48, 255));

    return true;
}

void ax::NodeEditor::Editor::End()
{
    auto inputState = BuildInputState();

    ProcessActions(inputState);

    m_Canvas.End();

    m_Pins.Discard();
    m_Nodes.Discard();
    m_Links.Discard();
    m_BackgroundCanvas.Discard();

    // #debug
    ImGui::GetWindowDrawList()->AddRect(
        m_Canvas.ContentRect().Min - ImVec2(1, 1),
        m_Canvas.ContentRect().Max + ImVec2(1, 1),
        IM_COL32(255, 255, 255, 255));
}

void ax::NodeEditor::Editor::NavigateTo(const ImVec2& point, bool immediate)
{
    auto newViewport = m_Canvas.ViewRect();
    newViewport.Translate(point - newViewport.GetCenter());
    NavigateTo(newViewport);
}

void ax::NodeEditor::Editor::NavigateTo(const ImRect& rect, bool immediate)
{
    auto rectSize   = rect.GetSize();
    auto targetRect = ImRect(
        rectSize.x > 0 ? rect.Min.x : m_Viewport.Min.x,
        rectSize.y > 0 ? rect.Min.y : m_Viewport.Min.y,
        rectSize.x > 0 ? rect.Max.x : m_Viewport.Max.x,
        rectSize.y > 0 ? rect.Max.y : m_Viewport.Max.y
    );

    SetCanvasView(targetRect);
}

const ImRect& ax::NodeEditor::Editor::Viewport() const
{
    return m_Viewport;
}

ax::NodeEditor::NodeBuilder ax::NodeEditor::Editor::BuildNode(NodeId id)
{
    IM_ASSERT(id != NodeId::Invalid);

    return NodeBuilder(*this, *m_Nodes.Get(id));
}

void ax::NodeEditor::Editor::Debug()
{
    if (ImGui::Begin("Debug##NodeEditorDebug"))
    {

    }
    ImGui::End();
}

void ax::NodeEditor::Editor::SetCanvasView(const ImRect& rect)
{
    auto worldCanvasOrigin = m_Canvas.ToWorld(ImVec2(0, 0));
    auto worldTargetRect = ImRect(
        m_Canvas.ToWorld(rect.Min) - worldCanvasOrigin,
        m_Canvas.ToWorld(rect.Max) - worldCanvasOrigin
    );
    auto worldTargetSize = worldTargetRect.GetSize();
    auto      canvasSize = m_Canvas.ContentRect().GetSize();

    auto      canvasAspectRatio =      canvasSize.y > 0.0f ?      canvasSize.x /      canvasSize.y : 0.0f;
    auto worldTargetAspectRatio = worldTargetSize.y > 0.0f ? worldTargetSize.x / worldTargetSize.y : 0.0f;

    if (canvasAspectRatio <= 0.0f || worldTargetAspectRatio <= 0.0f)
        return;

    auto newOrigin = m_Canvas.ViewOrigin();
    auto newScale  = m_Canvas.ViewScale();
    if (worldTargetAspectRatio > canvasAspectRatio)
    {
        newOrigin    = ImVec2(0, 0) - worldTargetRect.Min;
        newOrigin.y += (canvasSize.y - worldTargetSize.y) * 0.5f;
        newScale     = canvasSize.x / rect.GetWidth();
    }
    else
    {
        newOrigin    = ImVec2(0, 0) - worldTargetRect.Min;
        newOrigin.x += (canvasSize.x - worldTargetSize.x) * 0.5f;
        newScale     = canvasSize.y / rect.GetHeight();
    }

    m_Canvas.SetView(newOrigin, newScale);
}

ax::NodeEditor::InputState ax::NodeEditor::Editor::BuildInputState()
{
    InputState result;

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

        bool pressed = ImGui::ButtonBehavior(bounds, id, &state.Hovered, &state.Active, ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick);

        bool isMouseDoubleClicked = ImGui::IsMouseDoubleClicked(0);
        state.Clicked       = pressed && !isMouseDoubleClicked;
        state.DoubleClicked = pressed && isMouseDoubleClicked;

        apply(state, inputItem, object);
        apply(state, result.Object, object);
    };

    auto window = ImGui::GetCurrentWindow();

    for (auto& node : m_Nodes.View())
    {
        if (!node->m_IsLive)
            continue;

        auto& bounds = node->m_Bounds;
        auto  id     = window->GetID(node->m_Id.AsPointer());

        widget(bounds, id, result.Node, node);
    }

    widget(m_Canvas.ViewRect(), window->GetID(m_BackgroundCanvas.m_Id.AsPointer()), result.Canvas, &m_BackgroundCanvas);

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
            else
                action->Dismiss();
        }
    }

    if (nextAction)
        m_CurrentAction = nextAction;

    if (possibleAction)
        possibleAction->Dismiss();
}
