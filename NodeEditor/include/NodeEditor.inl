# pragma once
# include "NodeEditor.h"

template <typename IdTypeT>
ax::NodeEditor::Object<IdTypeT>::Object(IdType id)
    : m_Id(id)
{
}

template <typename IdTypeT>
IdTypeT ax::NodeEditor::Object<IdTypeT>::Id() const
{
    return m_Id;
}

template <typename IdTypeT>
bool ax::NodeEditor::Object<IdTypeT>::IsLive() const
{
    return m_IsLive;
}

template <typename IdTypeT>
void ax::NodeEditor::Object<IdTypeT>::Touch()
{
    m_IsLive = true;
}

template <typename IdTypeT>
void ax::NodeEditor::Object<IdTypeT>::Discard()
{
    m_IsLive = false;
}

template <typename T>
ax::NodeEditor::ObjectCollection<T>::~ObjectCollection()
{
    for (auto& object : m_Objects)
        delete object;
}

template <typename T>
T* ax::NodeEditor::ObjectCollection<T>::Create(IdType id)
{
    if (auto object = Find(id))
        return nullptr;

    m_Objects.reserve(m_Objects.size() + 1);
    m_Objects.push_back(new T(id));
    return m_Objects.back();
}

template <typename T>
const T* ax::NodeEditor::ObjectCollection<T>::Find(IdType id) const
{
    for (auto& object : m_Objects)
        if (object->Id() == id)
            return object;

    return nullptr;
}

template <typename T>
T* ax::NodeEditor::ObjectCollection<T>::Find(IdType id)
{
    return const_cast<T*>(const_cast<const ObjectCollection*>(this)->Find(id));
}

template <typename T>
T* ax::NodeEditor::ObjectCollection<T>::Get(IdType id)
{
    auto object = Find(id);
    if (!object)
        object = Create(id);
    return object;
}


template <typename T>
void ax::NodeEditor::ObjectCollection<T>::Discard()
{
    for (auto& object : m_Objects)
        object->Discard();
}
