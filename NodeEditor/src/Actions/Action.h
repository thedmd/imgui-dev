# pragma once
# include <imgui.h>

namespace ax {
namespace NodeEditor {

struct Editor;
struct InputState;

struct Action
{
    Action(Editor& editor);

    ~Action();

    virtual const char* Name() const = 0;

    virtual bool CanAccept(const InputState& inputState) = 0;

    virtual bool Accept(const InputState& inputState) = 0;
    virtual bool Process(const InputState& inputState) = 0;
    virtual void Dismiss() {}

    virtual ImGuiMouseCursor Cursor() const { return ImGuiMouseCursor_Arrow; }


protected:
    Editor& m_Editor;
};

} // namespace NodeEditor
} // namespace ax