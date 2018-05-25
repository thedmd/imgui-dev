# pragma once
# include <imgui.h>
# define IMGUI_DEFINE_MATH_OPERATORS
# include "imgui_internal.h"

namespace ax {
namespace NodeEditor {

struct Editor;
struct InputState;

struct Action
{
    enum Result { No, Yes, Possible };

    Action(Editor& editor);

    ~Action();

    virtual const char* Name() const = 0;

    virtual Result Accept(const InputState& inputState) = 0;
    virtual bool Process(const InputState& inputState) = 0;
    virtual void Dismiss() {}

    virtual ImGuiMouseCursor Cursor() const { return ImGuiMouseCursor_Arrow; }

    virtual void Debug() {}

protected:
    Editor& m_Editor;
};

} // namespace NodeEditor
} // namespace ax