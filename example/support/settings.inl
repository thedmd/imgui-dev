//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
# pragma once
# include "settings.h"
# include <sstream>

template <typename T>
inline bool ax::Settings::Set(std::string_view key, T&& value)
{
    std::ostringstream v;
    v << std::forward<T>(value);
    return Assign(key, v.str());
}

inline bool ax::Settings::Set(std::string_view key, const char* value)
{
    return Assign(key, value);
}

inline bool ax::Settings::Set(std::string_view key, std::string_view value)
{
    return Assign(key, std::string(value));
}

inline bool ax::Settings::Set(std::string_view key, std::string value)
{
    return Assign(key, std::move(value));
}

template <typename T>
inline std::optional<T> ax::Settings::Get(std::string_view key) const
{
    T value;
    if (Get(key, value))
        return std::move(value);
    else
        return std::nullopt;
}

template <typename T>
inline bool ax::Settings::Get(std::string_view key, T& value) const
{
    auto it = Find(key);
    if (it == m_Values.end())
        return false;

    std::istringstream v(it->second);
    v >> value;
    if (!v)
        return false;

    return true;
}

inline bool ax::Settings::Get(std::string_view key, std::string_view& value) const
{
    auto it = Find(key);
    if (it != m_Values.end())
    {
        value = it->second;
        return true;
    }
    else
        return false;
}
