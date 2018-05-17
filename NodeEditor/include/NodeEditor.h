# pragma once
# include "ax/SafePointerType.h"
# include "imgui.h"
# include "imguiex_canvas.h"
# include "Actions.h"

namespace ax {
namespace NodeEditor {

struct PinId    final: SafePointerType<PinId>    { using SafePointerType::SafePointerType; };
struct NodeId   final: SafePointerType<NodeId>   { using SafePointerType::SafePointerType; };
struct LinkId   final: SafePointerType<LinkId>   { using SafePointerType::SafePointerType; };
struct CanvasId final: SafePointerType<CanvasId> { using SafePointerType::SafePointerType; };

struct Pin;
struct Node;
struct Link;
struct Canvas;

enum class ObjectType
{
    Pin,
    Node,
    Link,
    Canvas
};
struct ObjectId final: SafePointerType<ObjectId>
{
    using SafePointerType::SafePointerType;
    ObjectId(PinId  pinId):      SafePointerType(pinId.AsPointer()),    m_Type(ObjectType::Pin)    {}
    ObjectId(NodeId nodeId):     SafePointerType(nodeId.AsPointer()),   m_Type(ObjectType::Node)   {}
    ObjectId(LinkId linkId):     SafePointerType(linkId.AsPointer()),   m_Type(ObjectType::Link)   {}
    ObjectId(CanvasId canvasId): SafePointerType(canvasId.AsPointer()), m_Type(ObjectType::Canvas) {}

    explicit operator PinId()    const { return AsPinId();    }
    explicit operator NodeId()   const { return AsNodeId();   }
    explicit operator LinkId()   const { return AsLinkId();   }
    explicit operator CanvasId() const { return AsCanvasId(); }

    PinId    AsPinId()    const { IM_ASSERT(IsPinId());    return PinId(AsPointer());    }
    NodeId   AsNodeId()   const { IM_ASSERT(IsNodeId());   return NodeId(AsPointer());   }
    LinkId   AsLinkId()   const { IM_ASSERT(IsLinkId());   return LinkId(AsPointer());   }
    CanvasId AsCanvasId() const { IM_ASSERT(IsCanvasId()); return CanvasId(AsPointer()); }

    bool IsPinId()    const { return m_Type == ObjectType::Pin;    }
    bool IsNodeId()   const { return m_Type == ObjectType::Node;   }
    bool IsLinkId()   const { return m_Type == ObjectType::Link;   }
    bool IsCanvasId() const { return m_Type == ObjectType::Canvas; }

    ObjectType Type() const { return m_Type; }

private:
    ObjectType m_Type;
};

struct Editor;
struct NodeBuilder;
struct Object;

template <typename T>
struct InputItem
{
    T Hovered       = T{};
    T Active        = T{};
    T Clicked       = T{};
    T DoubleClicked = T{};
};

struct InputState
{
    InputItem<Object*> Object;
    InputItem<Pin*>    Pin;
    InputItem<Node*>   Node;
    InputItem<Link*>   Link;
    InputItem<Canvas*> Canvas;
};

struct Object
{
    Object(ObjectId id);

    ObjectId m_Id;
    bool     m_IsLive = false;

    void Touch();
    void Discard();

          Pin*  AsPin();
    const Pin*  AsPin()  const;
          Node* AsNode();
    const Node* AsNode() const;
          Link* AsLink();
    const Link* AsLink() const;
};

// #todo: Make collection sorted to speed up Find()
template <typename T>
struct ObjectCollection
{
    ~ObjectCollection();

          T* Create(ObjectId id);
          T* Find(ObjectId id);
    const T* Find(ObjectId id) const;
          T* Get(ObjectId id);

    void Discard();

    const ImVector<T*>&       View()       { return m_Objects; }
    const ImVector<const T*>& View() const { return m_Objects; }

private:
    ImVector<T*> m_Objects;
};

struct Pin final : Object
{
    using Object::Object;
};

struct Node final : Object
{
    using Object::Object;

    ImRect m_Bounds = ImRect(0, 0, 0, 0);
};

struct Link final : Object
{
    using Object::Object;
};

struct Canvas final : Object
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

    void NavigateTo(const ImVec2& point, bool immediate = false);
    void NavigateTo(const ImRect& rect, bool immediate = false);

    const ImRect& Viewport() const;

    NodeBuilder BuildNode(NodeId id);

    void Debug();

private:
    void SetCanvasView(const ImRect& rect);

    InputState BuildInputState();

    void ProcessActions(const InputState& inputState);

    ImGuiID         m_Id;

    ImGuiEx::Canvas m_Canvas;
    ImRect          m_Viewport;

    ObjectCollection<Pin>  m_Pins;
    ObjectCollection<Node> m_Nodes;
    ObjectCollection<Link> m_Links;
    Canvas                 m_BackgroundCanvas;

    Action*         m_CurrentAction = nullptr;
    NavigateAction  m_NavigateAction;
    Action*         m_Actions[1]
    {
        &m_NavigateAction
    };
};


} // namespace NodeEditor
} // namespace ax

# include "NodeEditor.inl"