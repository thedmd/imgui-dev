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
    }

    virtual void Finalize() override
    {
    }

    virtual void Frame() override
    {
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

        m_Editor.Debug();
    }

    ax::NodeEditor::Editor m_Editor = ax::NodeEditor::Editor("editor");
};

std::unique_ptr<Sample> CreateNodeEditorSample(ax::Settings& m_Settings)
{
    return std::make_unique<NodeEditorSample>(m_Settings);
}
