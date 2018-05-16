# pragma once
# include <imgui.h>

namespace ax {
namespace NodeEditor {

struct Editor;

struct Action
{
    enum AcceptResult { Rejected, Accepted, Possible };

    Action(Editor& editor);

    ~Action();

    virtual AcceptResult Accept() = 0;
    virtual bool Process() = 0;
    virtual void Dismiss() {}

    virtual ImGuiMouseCursor Cursor() const { return ImGuiMouseCursor_Arrow; }


protected:
    Editor& m_Editor;
};

struct NavigateAction final
    : Action
{
    using Action::Action;

    virtual AcceptResult Accept() override;
    virtual bool Process() override;
};


} // namespace NodeEditor
} // namespace ax