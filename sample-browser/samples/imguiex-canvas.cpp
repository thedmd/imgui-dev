# include "samples.h"
# include <imguiex.h>

struct CanvasSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "ImGuiEx - Infinite Canvas";
    }

    virtual void Initialize() override
    {
        ImGuiEx::CreateContext();
    }

    virtual void Finalize() override
    {
        ImGuiEx::DestroyContext();
    }

    void FillItemRect(ImColor color, float expand = 0.0f)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetItemRectMin() - ImVec2(expand, expand),
            ImGui::GetItemRectMax() + ImVec2(expand, expand),
            color);
    };

    void DrawScale(const ImVec2& from, const ImVec2& to, float majorUnit, float minorUnit, float labelAlignment, float sign = 1.0f)
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

        for (auto d = 0.0f; d <= distance + majorUnit; d += majorUnit)
        {
            p = from + direction * d;

            drawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));

            if (d == 0.0f)
                continue;

            char label[16];
            snprintf(label, 15, "%g", d * sign);
            auto labelSize = ImGui::CalcTextSize(label);

            auto labelPosition    = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
            auto labelAlignedSize = ImDot(labelSize, direction);
            labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
            labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

            drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
        }
    }

    virtual void Frame() override
    {
        static auto configOriginX     = MakeProperty("Origin X##canvas-sample", 0.0f, m_Settings);
        static auto configOriginY     = MakeProperty("Origin Y##canvas-sample", 0.0f, m_Settings);
        static auto configScale       = MakeProperty("Scale##canvas-sample", 1.0f, m_Settings);
        static auto configMargin      = MakeProperty("Margin##canvas-sample", 0.0f, m_Settings);
        static auto configInnerCanvas = MakeProperty("Inner Canvas##canvas-sample", true, m_Settings);
        static auto statsContentSize = ImRect(0, 0, 0, 0);
        static auto statsViewSize = ImRect(0, 0, 0, 0);
        static auto statsWidgetCount = 0;
        static auto statsInnerContentSize = ImRect(0, 0, 0, 0);
        static auto statsInnerViewSize = ImRect(0, 0, 0, 0);
        static auto statsInnerWidgetCount = 0;

        ImGui::Columns(2);
        //ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.75f);
        if (ImGui::DragFloat(configOriginX.Name.c_str(), &configOriginX, 0.001f, -2.0f, 2.0f)) configOriginX.Save(m_Settings);
        if (ImGui::DragFloat(configOriginY.Name.c_str(), &configOriginY, 0.001f, -2.0f, 2.0f)) configOriginY.Save(m_Settings);
        if (ImGui::DragFloat(configScale.Name.c_str(),   &configScale,   0.001f, 0.01f, 15.0f)) configScale.Save(m_Settings);
        if (ImGui::DragFloat(configMargin.Name.c_str(),  &configMargin,  1.0f, 0.0f, 200.0f)) configScale.Save(m_Settings);
        if (ImGui::Checkbox(configInnerCanvas.Name.c_str(), &configInnerCanvas)) configScale.Save(m_Settings);
        //ImGui::PopItemWidth();
        ImGui::NextColumn();
        ImGui::Text("Canvas:");
        ImGui::Text("    Content Rect: { l: %.2f, t: %.2f, r: %.2f, b: %.2f, w: %.2f, h: %.2f }",
            statsContentSize.Min.x, statsContentSize.Min.y,
            statsContentSize.Max.x, statsContentSize.Max.y,
            statsContentSize.GetWidth(), statsContentSize.GetHeight());
        ImGui::Text("    View Rect: { l: %.2f, t: %.2f, r: %.2f, b: %.2f, w: %.2f, h: %.2f }",
            statsViewSize.Min.x, statsViewSize.Min.y,
            statsViewSize.Max.x, statsViewSize.Max.y,
            statsViewSize.GetWidth(), statsViewSize.GetHeight());
        ImGui::Text("    Widgets: %d", statsWidgetCount);
        ImGui::Text("Inner Canvas:");
        ImGui::Text("    Content Rect: { l: %.2f, t: %.2f, r: %.2f, b: %.2f, w: %.2f, h: %.2f }",
            statsInnerContentSize.Min.x, statsInnerContentSize.Min.y,
            statsInnerContentSize.Max.x, statsInnerContentSize.Max.y,
            statsInnerContentSize.GetWidth(), statsInnerContentSize.GetHeight());
        ImGui::Text("    View Rect: { l: %.2f, t: %.2f, r: %.2f, b: %.2f, w: %.2f, h: %.2f }",
            statsInnerViewSize.Min.x, statsInnerViewSize.Min.y,
            statsInnerViewSize.Max.x, statsInnerViewSize.Max.y,
            statsInnerViewSize.GetWidth(), statsInnerViewSize.GetHeight());
        ImGui::Text("    Widgets: %d", statsInnerWidgetCount);
        ImGui::Columns();

        ImGui::Separator();

        auto drawList = ImGui::GetWindowDrawList();
        //drawList->AddRectFilled(ImVec2(0.0f, 0.0f),
        //    ImGui::GetIO().DisplaySize, IM_COL32(255, 0, 0, 255));

        auto margin = ImVec2(configMargin.Value, configMargin.Value);
        auto size   = ImGui::GetContentRegionAvail() - margin * 2;

        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + margin);

        statsWidgetCount = 0;
        statsInnerWidgetCount = 0;

        if (ImGuiEx::BeginCanvas("canvas", size))
        {
            auto canvasContentMin  = ImGuiEx::CanvasContentMin();
            auto canvasContentMax  = ImGuiEx::CanvasContentMax();
            auto canvasContentSize = ImGuiEx::CanvasContentSize();

            statsContentSize.Min = canvasContentMin;
            statsContentSize.Max = canvasContentMax;

            ImGuiEx::CanvasView(canvasContentSize * 0.5f + ImMul(canvasContentSize * 0.5f, ImVec2(configOriginX, configOriginY)), configScale);

            auto canvasViewMin  = ImGuiEx::CanvasViewMin();
            auto canvasViewMax  = ImGuiEx::CanvasViewMax();
            auto canvasViewSize = ImGuiEx::CanvasViewSize();

            statsViewSize.Min = canvasViewMin;
            statsViewSize.Max = canvasViewMax;

            drawList->AddRectFilled(canvasViewMin, canvasViewMax, IM_COL32(0, 128, 128, 255));

            if (canvasViewMax.x > 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(canvasViewMax.x, 0.0f), 100.0f, 10.0f, 0.6f);
            if (canvasViewMin.x < 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(canvasViewMin.x, 0.0f), 100.0f, 10.0f, 0.6f, -1.0f);
            if (canvasViewMax.y > 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, canvasViewMax.y), 100.0f, 10.0f, 0.6f);
            if (canvasViewMin.y < 0.0f)
                DrawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, canvasViewMin.y), 100.0f, 10.0f, 0.6f, -1.0f);

            ImGui::ArrowButton("Hello1", ImGuiDir_Right);
            if (ImGui::IsItemVisible())
                ++statsWidgetCount;

            ImGui::SetCursorScreenPos(ImVec2(100.0f, 100.0f));
            ImGui::ArrowButton("Hello2", ImGuiDir_Right);
            if (ImGui::IsItemVisible())
                ++statsWidgetCount;

            if (ImGui::IsItemActive())
                ImGui::SetCursorScreenPos(ImVec2(200.0f, 200.0f));

            ImGui::SetCursorScreenPos(ImVec2(200.0f, 200.0f));
            ImGui::ArrowButton("Hello3", ImGuiDir_Right);
            if (ImGui::IsItemVisible())
                ++statsWidgetCount;

            if (configInnerCanvas)
            {
                ImGui::SetCursorScreenPos(ImVec2(-200.0f, -200.0f));
                if (ImGuiEx::BeginCanvas("Inner", ImVec2(180, 180)))
                {
                    ImGuiEx::CanvasView(ImVec2(0.0f, 0.0f), 1.0f / configScale);

                    statsInnerContentSize.Min = ImGuiEx::CanvasContentMin();
                    statsInnerContentSize.Max = ImGuiEx::CanvasContentMax();
                    statsInnerViewSize.Min    = ImGuiEx::CanvasViewMin();
                    statsInnerViewSize.Max    = ImGuiEx::CanvasViewMax();

                    auto test = ImGuiEx::CanvasToParent(ImVec2(0.0f, 0.0f));
                    auto test2 = ImGuiEx::CanvasToWorld(ImVec2(0.0f, 0.0f));
                    auto test3 = ImGuiEx::CanvasFromParent(test);
                    auto test4 = ImGuiEx::CanvasFromWorld(test2);

                    auto innerCanvasViewMin  = ImGuiEx::CanvasViewMin() - ImVec2(100, 100);
                    auto innerCanvasViewMax  = ImGuiEx::CanvasViewMax() + ImVec2(100, 100);
                    //auto canvasViewSize = ImGuiEx::CanvasViewSize();

                    int dx = 0, dy = 0;
                    for (auto x = innerCanvasViewMin.x; x < innerCanvasViewMax.x; x += 40, ++dx)
                    {
                        dy = 0;
                        for (auto y = innerCanvasViewMin.y; y < innerCanvasViewMax.y; y += 40, ++dy)
                        {
                            char label[16];
                            snprintf(label, 15, "%dx%d", dx, dy);

                            ImGui::SetCursorScreenPos(ImVec2(x, y));
                            ImGui::Button(label, ImVec2(35, 35));
                            if (ImGui::IsItemVisible())
                                ++statsInnerWidgetCount;
                            //FillItemRect(ImColor(128, 128, 255));
                        }
                    }

                    ImGuiEx::EndCanvas();
                    ++statsWidgetCount;
                }
            }

            ImGuiEx::EndCanvas();
        }
    }
};

std::unique_ptr<Sample> CreateCanvasSample(ax::Settings& m_Settings)
{
    return std::make_unique<CanvasSample>(m_Settings);
}
