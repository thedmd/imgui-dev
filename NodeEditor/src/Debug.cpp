# include "NodeEditor_Internal.h"

ImGuiTextBuffer ax::NodeEditor::Debug::ToString(const ImVec2& p)
{
    ImGuiTextBuffer buffer;
    buffer.reserve(64);

    buffer.appendf("{ x: %g, y: %g }", p.x, p.y);

    return buffer;
}

ImGuiTextBuffer ax::NodeEditor::Debug::ToString(const ImRect& rect)
{
    ImGuiTextBuffer buffer;
    buffer.reserve(128);

    buffer.appendf("{ l: %g, t: %g, r: %g, b: %g, w: %g, h: %g }",
        rect.Min.x, rect.Min.y,
        rect.Max.x, rect.Max.y,
        rect.GetWidth(), rect.GetHeight());

    return buffer;
}

ImGuiTextBuffer ax::NodeEditor::Debug::ToString(const ImGuiEx::CanvasView& view)
{
    ImGuiTextBuffer buffer;
    buffer.reserve(128);

    buffer.appendf("{ Origin: %s, Scale: %g }", ToString(view.Origin).c_str(), view.Scale);

    return buffer;
}

ImGuiTextBuffer ax::NodeEditor::Debug::ToString(KeyModifers modifiers)
{
    ImGuiTextBuffer buffer;
    buffer.reserve(128);

    if (modifiers == KeyModifers::None)
        buffer.appendf("None");
    else
    {
        if (!!(modifiers & KeyModifers::Ctrl))  { buffer.appendf("Ctrl | ");  }
        if (!!(modifiers & KeyModifers::Shift)) { buffer.appendf("Shift | "); }
        if (!!(modifiers & KeyModifers::Alt))   { buffer.appendf("Alt | ");   }
        if (!!(modifiers & KeyModifers::Super)) { buffer.appendf("Super | "); }

        if (!buffer.empty())
        {
            buffer.Buf.resize(buffer.Buf.size() - 3);
            buffer.Buf.push_back(0);
        }
    }

    return buffer;
}
