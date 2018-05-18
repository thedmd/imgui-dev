# pragma once
# include <imgui.h>

namespace ax {
namespace NodeEditor {

struct Editor;
struct InputState;

struct Action
{
    enum Result { No, Yes, Possible };

    Action(Editor& editor);

    ~Action();

    virtual Result Accept(const InputState& inputState) = 0;
    virtual bool Process(const InputState& inputState) = 0;
    virtual void Dismiss() {}

    virtual ImGuiMouseCursor Cursor() const { return ImGuiMouseCursor_Arrow; }


protected:
    Editor& m_Editor;
};

} // namespace NodeEditor
} // namespace ax