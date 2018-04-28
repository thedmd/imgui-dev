# include "Settings.h"
# include "picojson.h"
# include <fstream>
# include <algorithm>

bool ax::Settings::Parse(std::string_view configuration)
{
    namespace json = picojson;

    std::string parseError;
    json::value parseValue;
    json::parse(parseValue, begin(configuration), end(configuration), &parseError);
    if (!parseError.empty())
        return false;

    if (!parseValue.is<json::object>())
        return false;

    for (auto& entry : parseValue.get<json::object>())
        Assign(entry.first, entry.second.to_str());

    return true;
}

bool ax::Settings::Load(std::string_view path)
{
    std::ifstream file(std::string(path), std::ios::binary);
    if (!file)
        return false;

    file.seekg(0, std::ios::end);
    auto size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<char> data;
    data.resize(size);

    file.read(data.data(), data.size());
    if (!file)
        return false;

    return Parse(std::string_view(data.data(), data.size()));
}

bool ax::Settings::Save(std::string_view path) const
{
    std::ofstream file(std::string(path), std::ios::binary);
    if (!file)
        return false;

    auto data = Serialize();

    file.write(data.data(), data.size());

    return !!file;
}

std::string ax::Settings::Serialize() const
{
    namespace json = picojson;

    json::object object;
    for (auto& entry : m_Values)
        object[entry.first] = json::value(entry.second);

    json::value value(std::move(object));
    return value.serialize(true);
}

void ax::Settings::Merge(const Settings& other)
{
    if (this == &other)
        return;

    for (auto& entry : other.m_Values)
        Assign(entry.first, entry.second);
}

bool ax::Settings::Assign(std::string_view key, std::string value)
{
    auto where = std::lower_bound(m_Values.begin(), m_Values.end(), key, [](const std::pair<std::string,std::string>& e, const std::string_view& a)
    {
        return a > std::string_view(e.first);
    });

    if (where != m_Values.end() && std::string_view(where->first) == key)
    {
        where->second = std::move(value);
        return false;
    }
    else
    {
        m_Values.emplace(where, std::string(key), std::move(value));
        return true;
    }
}

ax::Settings::Values::iterator ax::Settings::Find(std::string_view key)
{
    auto where = std::lower_bound(m_Values.begin(), m_Values.end(), key, [](const std::pair<std::string,std::string>& e, const std::string_view& a)
    {
        return a > std::string_view(e.first);
    });

    if (where != m_Values.end() && std::string_view(where->first) == key)
        return where;
    else
        return m_Values.end();
}

ax::Settings::Values::const_iterator ax::Settings::Find(std::string_view key) const
{
    auto where = std::lower_bound(m_Values.begin(), m_Values.end(), key, [](const std::pair<std::string,std::string>& e, const std::string_view& a)
    {
        return a > std::string_view(e.first);
    });

    if (where != m_Values.end() && std::string_view(where->first) == key)
        return where;
    else
        return m_Values.end();
}

bool ax::Settings::Remove(std::string_view key)
{
    auto where = Find(key);
    if (where != m_Values.end())
    {
        m_Values.erase(where);
        return true;
    }
    else
        return false;
}

bool ax::Settings::Has(std::string_view key) const
{
    return Find(key) != m_Values.end();
}
