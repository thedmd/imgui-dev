# pragma once
# include "NodeEditor_Internal.h"

namespace ax {
namespace NodeEditor {

struct NodeBuilder
{
    NodeBuilder(Editor& editor, Node& node);
    ~NodeBuilder();

private:
    Editor& m_Editor;
    Node&   m_Node;
};

} // namespace NodeEditor
} // namespace ax
