# include "Drawing.h"

void ax::Drawing::Grid(ImDrawList& drawList, const ImRect& rect, const ImVec2& gridOrigin, float gridSize, ImU32 gridColor)
{
    auto origin = ImVec2(ImFmod(gridOrigin.x, gridSize), ImFmod(gridOrigin.y, gridSize));
    if (origin.x < 0.0f)
        origin.x += gridSize;
    if (origin.y < 0.0f)
        origin.y += gridSize;

    for (float x = rect.Min.x + origin.x; x < rect.Max.x; x += gridSize)
        drawList.AddLine(ImVec2(x, rect.Min.y), ImVec2(x, rect.Max.y), gridColor);

    for (float y = rect.Min.y + origin.y; y < rect.Max.y; y += gridSize)
        drawList.AddLine(ImVec2(rect.Min.x, y), ImVec2(rect.Max.x, y), gridColor);
}