# include "samples.h"
# include <iostream>

struct CanvasSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Canvas";
    }

    virtual void Frame() override
    {
    }
};

std::unique_ptr<Sample> CreateCanvasSample(ax::Settings& m_Settings)
{
    return std::make_unique<CanvasSample>(m_Settings);
}
