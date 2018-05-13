# include "samples.h"
# include <imguiex.h>

struct CanvasSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Canvas";
    }

    virtual void Initialize() override
    {
        ImGuiEx::CreateContext();
    }

    virtual void Finalize() override
    {
        ImGuiEx::DestroyContext();
    }

    void DrawScale(const ImVec2& from, const ImVec2& to, float majorUnit, float minorUnit, float labelAlignment)
    {
        auto drawList  = ImGui::GetWindowDrawList();
        auto direction = (to - from) * ImInvLength(to - from, 0.0f);
        auto normal    = ImVec2(-direction.y, direction.x);
        auto distance  = sqrtf(ImLengthSqr(to - from));

        if (ImDot(direction, direction) < FLT_EPSILON)
            return;

        auto minorSize = 5.0f;
        auto majorSize = 10.0f;
        auto labelDistance = 8.0f;

        drawList->AddLine(from, to, IM_COL32(255, 255, 255, 255));

        auto p = from;
        for (auto d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit)
            drawList->AddLine(p - normal * minorSize, p + normal * minorSize, IM_COL32(255, 255, 255, 255));

        p = from;
        for (auto d = 0.0f; d <= distance; d += majorUnit, p += direction * majorUnit)
        {
            drawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));

            char label[16];
            snprintf(label, 15, "%g", d);
            auto labelSize = ImGui::CalcTextSize(label);

            auto labelPosition    = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
            auto labelAlignedSize = ImDot(labelSize, direction);
            labelPosition += direction * floorf(-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);

            drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
        }
    }

    virtual void Frame() override
    {
        auto drawList = ImGui::GetWindowDrawList();
        //drawList->AddRectFilled(ImVec2(0.0f, 0.0f),
        //    ImGui::GetIO().DisplaySize, IM_COL32(255, 0, 0, 255));

        if (ImGuiEx::BeginCanvas("canvas"))
        {
            auto canvasContentMin  = ImGuiEx::CanvasContentMin();
            auto canvasContentMax  = ImGuiEx::CanvasContentMax();
            auto canvasContentSize = ImGuiEx::CanvasContentSize();

            ImGuiEx::CanvasView(canvasContentSize * 0.5f, 0.41f);

            auto canvasViewMin  = ImGuiEx::CanvasViewMin();
            auto canvasViewMax  = ImGuiEx::CanvasViewMax();
            auto canvasViewSize = ImGuiEx::CanvasViewSize();

            drawList->AddRectFilled(canvasViewMin, canvasViewMax, IM_COL32(0, 128, 128, 255));

            DrawScale(ImVec2(0.0f, 0.0f), ImVec2(canvasViewMax.x, 0.0f), 100.0f, 10.0f, 0.6f);
            DrawScale(ImVec2(0.0f, 0.0f), ImVec2(canvasViewMin.x, 0.0f), 100.0f, 10.0f, 0.6f);
            DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, canvasViewMax.y), 100.0f, 10.0f, 0.6f);
            DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, canvasViewMin.y), 100.0f, 10.0f, 0.6f);

            ImGui::ArrowButton("Hello", ImGuiDir_Right);

            ImGui::SetCursorScreenPos(ImVec2(100.0f, 100.0f));
            ImGui::ArrowButton("Hello", ImGuiDir_Right);

            ImGui::SetCursorScreenPos(ImVec2(200.0f, 200.0f));
            ImGui::ArrowButton("Hello", ImGuiDir_Right);


            ImGuiEx::EndCanvas();
        }
    }
};

std::unique_ptr<Sample> CreateCanvasSample(ax::Settings& m_Settings)
{
    return std::make_unique<CanvasSample>(m_Settings);
}
