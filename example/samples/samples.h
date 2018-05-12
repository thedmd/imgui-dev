# pragma once
# include <imgui.h>
# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui_internal.h>
# include <memory>
# include <vector>
# include "support/settings.h"
# include "support/property.h"

struct Sample
{
    Sample(ax::Settings& settings): m_SettingsPtr(&settings), m_Settings(*m_SettingsPtr) {}
    Sample(Sample&& other): m_SettingsPtr(other.m_SettingsPtr), m_Settings(*m_SettingsPtr) {}
    Sample& operator=(Sample&& other) { this->m_SettingsPtr = other.m_SettingsPtr; return *this; }
    virtual ~Sample() {}

    virtual const char* Name() const = 0;

    virtual void Initialize() { }
    virtual void Finalize() {}

    virtual void Frame() {}

protected:
    ax::Settings* m_SettingsPtr;
    ax::Settings& m_Settings;
};

std::unique_ptr<Sample> CreateLayoutsSample(ax::Settings& settings);
std::unique_ptr<Sample> CreateCanvasSample(ax::Settings& settings);
std::unique_ptr<Sample> CreateTransformationSample(ax::Settings& m_Settings);
std::unique_ptr<Sample> CreateBlueprintNodeSample(ax::Settings& m_Settings);

inline std::vector<std::unique_ptr<Sample>> CreateSamples(ax::Settings& settings)
{
    using Factory = std::unique_ptr<Sample>(*)(ax::Settings& settings);

    static Factory factories[] =
    {
        &CreateLayoutsSample,
        &CreateCanvasSample,
        &CreateTransformationSample,
        &CreateBlueprintNodeSample
    };

    std::vector<std::unique_ptr<Sample>> result;
    result.reserve(sizeof(factories) / sizeof(*factories));
    for (auto& factory : factories)
        result.emplace_back(factory(settings));

    return std::move(result);
}
