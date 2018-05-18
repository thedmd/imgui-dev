# pragma once
# include "NodeEditor.h"

inline ax::NodeEditor::Object::Object(ObjectId id)
    : m_Id(id)
{
}

inline ax::NodeEditor::ObjectType ax::NodeEditor::Object::Type() const
{
    return m_Id.Type();
}

inline void ax::NodeEditor::Object::Touch()
{
    m_IsLive = true;
}

inline void ax::NodeEditor::Object::Reset()
{
    m_IsLive = false;
}

inline       ax::NodeEditor::Pin*  ax::NodeEditor::Object::AsPin()        { return m_Id.IsPinId()  ? static_cast<       Pin*>(this) : nullptr; }
inline const ax::NodeEditor::Pin*  ax::NodeEditor::Object::AsPin()  const { return m_Id.IsPinId()  ? static_cast<const  Pin*>(this) : nullptr; }
inline       ax::NodeEditor::Node* ax::NodeEditor::Object::AsNode()       { return m_Id.IsNodeId() ? static_cast<      Node*>(this) : nullptr; }
inline const ax::NodeEditor::Node* ax::NodeEditor::Object::AsNode() const { return m_Id.IsNodeId() ? static_cast<const Node*>(this) : nullptr; }
inline       ax::NodeEditor::Link* ax::NodeEditor::Object::AsLink()       { return m_Id.IsLinkId() ? static_cast<      Link*>(this) : nullptr; }
inline const ax::NodeEditor::Link* ax::NodeEditor::Object::AsLink() const { return m_Id.IsLinkId() ? static_cast<const Link*>(this) : nullptr; }

template <typename T>
inline ax::NodeEditor::ObjectCollection<T>::~ObjectCollection()
{
    for (auto& object : m_Objects)
        delete object;
}

template <typename T>
inline T* ax::NodeEditor::ObjectCollection<T>::Create(ObjectId id)
{
    if (auto object = Find(id))
        return nullptr;

    m_Objects.reserve(m_Objects.size() + 1);
    m_Objects.push_back(new T(id));
    return m_Objects.back();
}

template <typename T>
inline const T* ax::NodeEditor::ObjectCollection<T>::Find(ObjectId id) const
{
    for (auto& object : m_Objects)
        if (object->m_Id == id)
            return object;

    return nullptr;
}

template <typename T>
inline T* ax::NodeEditor::ObjectCollection<T>::Find(ObjectId id)
{
    return const_cast<T*>(const_cast<const ObjectCollection*>(this)->Find(id));
}

template <typename T>
inline T* ax::NodeEditor::ObjectCollection<T>::Get(ObjectId id)
{
    auto object = Find(id);
    if (!object)
        object = Create(id);
    return object;
}

template <typename T>
int ax::NodeEditor::ObjectCollection<T>::Count() const
{
    return m_Objects.size();
}

template <typename T>
inline void ax::NodeEditor::ObjectCollection<T>::Reset()
{
    for (auto& object : m_Objects)
        object->Reset();
}
