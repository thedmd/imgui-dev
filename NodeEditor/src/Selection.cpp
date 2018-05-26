# include "NodeEditor_Internal.h"

bool ax::NodeEditor::Selection::Select(Object* object, SelectOperation operation)
{
    switch (operation)
    {
        case SelectOperation::Replace:
            if (Objects.size() == 1 && Objects.contains(object))
                return false;
            DeselectAll();
            Objects.push_back(object);
            return true;

        case SelectOperation::Add:
            if (Objects.contains(object))
                return false;
            Objects.push_back(object);
            return true;

        case SelectOperation::Remove:
            for (auto it = Objects.begin(), itEnd = Objects.end(); it != itEnd; ++it)
            {
                if (*it == object)
                {
                    Objects.erase(it);
                    return true;
                }
            }
            return false;

        case SelectOperation::Toggle:
            if (Objects.contains(object))
                return Select(object, SelectOperation::Remove);
            else
                return Select(object, SelectOperation::Add);

        default:
            return false;
    }
}

bool ax::NodeEditor::Selection::Select(const ImVector<Object*>& objects, SelectOperation operation)
{
    IM_ASSERT(&Objects != &objects);

    bool any = false;
    for (auto o : objects)
        any |= Select(o, operation);

    return any;
}

void ax::NodeEditor::Selection::DeselectAll()
{
    Objects.clear();
}