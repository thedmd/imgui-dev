# include "samples.h"
# include <NodeEditor.h>

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

            {
                auto builder = m_Editor.BuildNode(1);
                ImGui::Text("I'm a node!");
            }



            m_Editor.End();
        }
    }

    ax::NodeEditor::Editor m_Editor = ax::NodeEditor::Editor("editor");
};

std::unique_ptr<Sample> CreateNodeEditorSample(ax::Settings& m_Settings)
{
    return std::make_unique<NodeEditorSample>(m_Settings);
}
