# include "samples.h"
# include <NodeEditor.h>
# include <NodeEditor_Internal.h>
# include <Editor.h>

struct NodeEditorSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "NodeEditor";
    }

    virtual void Initialize() override
    {
        ImGui::Columns(2, "MainSplit");
        ImGui::SetColumnOffset(1, ImGui::GetWindowContentRegionWidth() * 0.22f);
        ImGui::Columns();
    }

    virtual void Finalize() override
    {
    }

    virtual void Frame() override
    {
        ImGui::Columns(2, "MainSplit");

        m_Editor.Debug(false);

        ImGui::NextColumn();

        if (m_Editor.Begin())
        {

            //m_Editor.NavigateTo(ImRect(0, 0, 100, 100));
            //m_Editor.NavigateTo(ImVec2(0, 0));

            {
                auto builder = m_Editor.BuildNode(1);
                ImGui::Text("I'm a node!");
            }



            m_Editor.End();
        }

        ImGui::Columns();
    }

    ax::NodeEditor::Editor m_Editor = ax::NodeEditor::Editor("editor");
};

std::unique_ptr<Sample> CreateNodeEditorSample(ax::Settings& m_Settings)
{
    return std::make_unique<NodeEditorSample>(m_Settings);
}
