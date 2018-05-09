# pragma once
# include <imgui.h>
# include "../support/Settings.h"
# include <iostream>
# include <string_view>

inline std::ostream& operator <<(std::ostream& stream, const ImVec2& value)
{
    stream << value.x << ' ' << value.y;
    return stream;
}

inline std::istream& operator >>(std::istream& stream, ImVec2& value)
{
    stream >> value.x >> value.y;
    return stream;
}

template <typename T>
struct Property
{
    std::string Name;
    T           Value;

    Property(std::string_view name, T value)
        : Name(name)
        , Value(value)
    {
    }

    Property(std::string_view name, T value, const ax::Settings& m_Settings)
        : Name(name)
        , Value(m_Settings.Get<T>(name).value_or(value))
    {
    }

    void Save(ax::Settings& m_Settings)
    {
        m_Settings.Set(Name, Value);
    }

    operator const T&() const { return Value; }
    operator       T&()       { return Value; }

    const T* operator &() const { return &Value; }
          T* operator &()       { return &Value; }

    const T* operator->() const { return &Value; }
          T* operator->()       { return &Value; }
};

template <typename T, typename... Args>
auto MakeProperty(std::string_view name, T&& value, Args&&... args)
{
    return Property<std::decay_t<T>>(name, std::forward<T>(value), std::forward<Args>(args)...);
}