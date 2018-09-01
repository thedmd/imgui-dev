# include "samples.h"
# include <iostream>

struct HitTestSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Hit Test";
    }

    virtual void Frame() override
    {
        //*/
        auto circleHitTest = [](const ImVec2& point, const ImVec2& min, const ImVec2& max, void*)
        {
            const auto min_size = ImMin(max.x - min.x, max.y - min.y);
            const auto radius = min_size * 0.5f;
            const auto radius_squared = radius * radius;
            const auto center = (max + min) * 0.5f;
            const auto offset = point - center;
            const auto distance_squared = offset.x * offset.x + offset.y * offset.y;

            return distance_squared < radius_squared;
        };

        auto circleButton = [&circleHitTest](const char* str_id, const ImVec2& size) -> bool
        {
            ImGui::PushHitTest(circleHitTest);

            auto result   = ImGui::InvisibleButton(str_id, size);
            auto min      = ImGui::GetItemRectMin();
            auto max      = ImGui::GetItemRectMax();
            auto center   = (max + min) * 0.5f;
            auto min_size = ImMin(max.x - min.x, max.y - min.y);
            auto radius   = min_size * 0.5f;

            auto& style = ImGui::GetStyle();

            ImU32 color = IM_COL32_WHITE;
            if (ImGui::IsItemActive())
                color = ImColor(style.Colors[ImGuiCol_ButtonActive]);
            else if (ImGui::IsItemHovered())
                color = ImColor(style.Colors[ImGuiCol_ButtonHovered]);
            else
                color = ImColor(style.Colors[ImGuiCol_Button]);
            ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, color, 24);
            auto textSize = ImGui::CalcTextSize(str_id);
            ImGui::GetWindowDrawList()->AddText(center - textSize * 0.5f,
                ImColor(style.Colors[ImGuiCol_Text]), str_id);
            ImGui::PopHitTest();

            return result;
        };

        ImGui::BeginGroup();
        {
            bool down = ImGui::Button("I'm ImGui::Button()!", ImVec2(150, 150));
            static int counter = 0;
            if (down)
                ++counter;
            ImGui::Text("Clicks: %d", counter);
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 75.0f);
            ImGui::PushHitTest(circleHitTest);
            bool down = ImGui::Button("I'm ImGui::Button()\n   with rounding\n   and hit test!", ImVec2(150, 150));
            ImGui::PopHitTest();
            ImGui::PopStyleVar();
            static int counter = 0;
            if (down)
                ++counter;
            ImGui::Text("Clicks: %d", counter);
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            bool down = circleButton(" I'm a custom\ncircle button!", ImVec2(150, 150));
            static int counter = 0;
            if (down)
                ++counter;
            ImGui::Text("Clicks: %d", counter);
        }
        ImGui::EndGroup();


        static ImVec2 emptyRect;
        ImGui::SetNextWindowHitTest([](const ImVec2& point, const ImVec2& min, const ImVec2& max, void*)
        {
            if (point.x >= emptyRect.x && point.y >= emptyRect.y && point.x <= (emptyRect.x + 150.0f) && point.y <= (emptyRect.y + 150.0f))
                return false;

            return true;
        });
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Window with a Hole"))
        {
            emptyRect = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRect(
                ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2(150, 150), IM_COL32(255, 0, 0, 255));

            ImGui::GetWindowDrawList()->AddText(
                ImGui::GetCursorScreenPos() + ImVec2(0, 150 + ImGui::GetStyle().ItemSpacing.y),
                IM_COL32_WHITE, "This is a hole in window.");
        }
        ImGui::End();
    }
};

std::unique_ptr<Sample> CreateHitTestSample(ax::Settings& m_Settings)
{
    return std::make_unique<HitTestSample>(m_Settings);
}
