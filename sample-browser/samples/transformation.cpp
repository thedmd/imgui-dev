# include "samples.h"
# include <iostream>

struct TransformationSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Transformation";
    }

    virtual void Frame() override
    {
        static float scale = 1.0f;
        static ImVec2 scale2 = ImVec2(1.0f, 1.0f);
        static float angle = 0.0f;
        static ImVec2 offset = ImVec2(0.0f, 0.0f);
        ImGui::DragFloat("Uniform Scale", &scale, 0.01f, 0.001f, 40.0f);
        ImGui::DragFloat2("Scale", &scale2.x, 0.01f, 0.001f, 40.0f);
        ImGui::DragFloat("Angle", &angle, 0.01f * 0.5f * 3.1415f);
        ImGui::DragFloat2("Translation", &offset.x, 1.0f, -1000.0f, 1000.0f);

        auto size     = ImGui::GetWindowSize() - ImGui::GetCursorPos() - ImGui::GetStyle().WindowPadding;
        auto center   = ImGui::GetCursorScreenPos() + size * 0.5f;
        auto drawList = ImGui::GetWindowDrawList();

        drawList->AddRect(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + size,
            IM_COL32(255, 255, 255, 255));

        ImMatrix transf = ImMatrix()
            * ImMatrix::Scaling(scale2.x, scale2.y)
            * ImMatrix::Translation(offset)
            * ImMatrix::Scaling(scale, -scale)
            * ImMatrix::Rotation(angle)
            * ImMatrix::Translation(center)
            ;

        drawList->PushClipRect(
            ImGui::GetCursorScreenPos() + ImVec2(1, 1),
            ImGui::GetCursorScreenPos() + size - ImVec2(1, 1));

        drawList->ApplyTransformation(transf);

        drawList->ApplyTransformation(ImMatrix::Scaling(10, 10));
        const float gridSize = 10;
        ImVec2 correction = ImVec2(drawList->_HalfPixel.x, 0.0f);
        for (float i = -gridSize; i <= gridSize; ++i)
        {
            drawList->AddLine(ImVec2(i, -gridSize) - correction, ImVec2(i, gridSize) - correction, IM_COL32(255, 200, 200, 100), 1.0f);
            drawList->AddLine(ImVec2(-gridSize, i) - correction, ImVec2(gridSize, i) - correction, IM_COL32(200, 255, 200, 100), 1.0f);
        }
        drawList->AddLine(ImVec2(0, 0), ImVec2(0, gridSize), IM_COL32(80, 255, 80, 200), 3.0f);
        drawList->AddLine(ImVec2(0, 0), ImVec2(gridSize, 0), IM_COL32(255, 80, 80, 200), 3.0f);
        drawList->PopTransformation();

        drawList->PathClear();
        drawList->PathLineTo(
            ImVec2(0.0f, 30.0f));
        drawList->PathBezierCurveTo(
            ImVec2(-40.0f, 80.0f),
            ImVec2(-80.0f, 0.0f),
            ImVec2(0.0f, -50.0f));
        drawList->PathBezierCurveTo(
            ImVec2(80.0f, 0.0f),
            ImVec2(40.0f, 80.0f),
            ImVec2(0.0f, 30.0f));

        drawList->AddPolyline(
            drawList->_Path.Data,
            drawList->_Path.Size,
            IM_COL32(255, 255, 255, 200),
            true,
            4.0f / drawList->_InvTransformationScale);

        drawList->PathFillConvex(IM_COL32(255, 0, 0, 200));

        drawList->PopTransformation();

        drawList->PopClipRect();
    }
};

std::unique_ptr<Sample> CreateTransformationSample(ax::Settings& m_Settings)
{
    return std::make_unique<TransformationSample>(m_Settings);
}
