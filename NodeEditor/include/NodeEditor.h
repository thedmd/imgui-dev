# pragma once
# include "ax/SafePointerType.h"
# include "imgui.h"
# include "imguiex_canvas.h"

namespace ax {
namespace NodeEditor {

struct PinId  final: SafePointerType<PinId>  { using SafePointerType::SafePointerType; };
struct NodeId final: SafePointerType<NodeId> { using SafePointerType::SafePointerType; };
struct LinkId final: SafePointerType<LinkId> { using SafePointerType::SafePointerType; };

struct Editor;

template <typename IdTypeT>
struct Object
{
    using IdType = IdTypeT;

    Object(IdType id);

    IdType Id() const;
    bool IsLive() const;

    void Touch();
    void Discard();

private:
    IdType  m_Id;
    bool    m_IsLive = false;
};

// #todo: Make collection sorted to speed up Find()
template <typename T>
struct ObjectCollection
{
    using IdType = typename T::IdType;

    ~ObjectCollection();

          T* Create(IdType id);
          T* Find(IdType id);
    const T* Find(IdType id) const;
          T* Get(IdType id);

    void Discard();

private:
    ImVector<T*> m_Objects;
};

struct Pin final
    : Object<PinId>
{
    using Object::Object;
};

struct Node final
    : Object<NodeId>
{
    using Object::Object;

    ImRect m_Bounds = ImRect(0, 0, 0, 0);
};

struct Link final
    : Object<LinkId>
{
    using Object::Object;
};

struct NodeBuilder
{
    NodeBuilder(Editor& editor, Node& node);
    ~NodeBuilder();

private:
    Editor& m_Editor;
    Node&   m_Node;
};

struct Editor
{
    Editor(const char* str_id);

    bool Begin(const ImVec2& size = ImVec2(0, 0));
    void End();

    NodeBuilder BuildNode(NodeId id);

private:
    ImGuiID         m_Id;
    ImGuiEx::Canvas m_Canvas;

    ObjectCollection<Pin>  m_Pins;
    ObjectCollection<Node> m_Nodes;
    ObjectCollection<Link> m_Links;
};


} // namespace NodeEditor
} // namespace ax

# include "NodeEditor.inl"