# pragma once
# include <string_view>
# include <string>
# include <vector>
# include <optional>
# include <utility>

namespace ax {

class Settings
{
public:
    Settings() = default;
    Settings(const Settings&) = default;
    Settings(Settings&&) = default;

    Settings& operator=(const Settings&) = default;
    Settings& operator=(Settings&&) = default;

    bool Parse(std::string_view configuration);
    std::string Serialize() const;

    bool Load(std::string_view path);
    bool Save(std::string_view path) const;

    void Merge(const Settings& other);

    template <typename T>
    bool Set(std::string_view key, T&& value);
    bool Set(std::string_view key, const char* value);
    bool Set(std::string_view key, std::string_view value);
    bool Set(std::string_view key, std::string value);

    template <typename T>
    std::optional<T> Get(std::string_view key) const;
    template <typename T>
    bool Get(std::string_view key, T& value) const;
    bool Get(std::string_view key, std::string_view& value) const;

    bool Remove(std::string_view key);

    bool Has(std::string_view key) const;

private:
    using Values = std::vector<std::pair<std::string, std::string>>;

    bool Assign(std::string_view key, std::string value);
    Values::iterator Find(std::string_view key);
    Values::const_iterator Find(std::string_view key) const;

    Values m_Values;
};

} // namespace ax

# include "Settings.inl"