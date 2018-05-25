# include "Drawing.h"

void ax::Drawing::Grid(ImDrawList& drawList, const ImRect& rect, float gridSize, ImU32 gridColor, float scale)
{
    // Use transform API to handle fringe scaling properly.
    drawList.ApplyTransformation(ImMatrix::Scaling(scale, scale));

    auto origin = ImVec2(ImFmod(rect.Min.x, gridSize), ImFmod(rect.Min.y, gridSize));
    if (origin.x > 0.0f)
        origin.x -= gridSize;
    if (origin.y > 0.0f)
        origin.y -= gridSize;

    for (float x = rect.Min.x - origin.x; x < rect.Max.x; x += gridSize)
        drawList.AddLine(ImVec2(x, rect.Min.y), ImVec2(x, rect.Max.y), gridColor);

    for (float y = rect.Min.y - origin.y; y < rect.Max.y; y += gridSize)
        drawList.AddLine(ImVec2(rect.Min.x, y), ImVec2(rect.Max.x, y), gridColor);

    drawList.PopTransformation();
}