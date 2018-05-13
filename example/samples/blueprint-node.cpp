# include "samples.h"
# include <iostream>
# include "imgui_internal.h"

static void DrawItemRect(ImColor color, float expand = 0.0f)
{
    ImGui::GetWindowDrawList()->AddRect(
        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        color);
};

enum class IconType { Flow, Circle, Square, Grid, RoundSquare, Diamond };

static void DrawIcon(ImDrawList& drawList, IconType type, const ImVec2& a, const ImVec2& b, bool filled, ImU32 color, ImU32 innerColor)
{
          auto rect           = ImRect(a, b);
    const auto outline_scale  = rect.GetWidth() / 24.0f;
    const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

    if (type == IconType::Flow)
    {
        const auto origin_scale = rect.GetWidth() / 24.0f;

        const auto offset_x   = 1.0f * origin_scale;
        const auto offset_y   = 0.0f * origin_scale;
        const auto margin     = (filled ? 2.0f : 2.0f) * origin_scale;
        const auto rounding   = 0.81f * origin_scale;
        const auto tip_round  = 0.7f; // percentage of triangle edge (for tip)
                                      //const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
        const auto canvas = ImRect(
            rect.Min.x + margin + offset_x,
            rect.Min.y + margin + offset_y,
            rect.Max.x - margin,
            rect.Max.y - margin);

        const auto left     = canvas.Min.x + canvas.GetWidth()                       * 0.5f * 0.3f;
        const auto right    = canvas.Min.x + canvas.GetWidth() - canvas.GetWidth()   * 0.5f * 0.3f;
        const auto top      = canvas.Min.y + canvas.GetHeight()                      * 0.5f * 0.2f;
        const auto bottom   = canvas.Min.y + canvas.GetHeight() - canvas.GetHeight() * 0.5f * 0.2f;
        const auto center_y = (top + bottom) * 0.5f;
        //const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

        const auto tip_top    = ImVec2(canvas.Min.x + canvas.GetWidth() * 0.5f, top);
        const auto tip_right  = ImVec2(right, center_y);
        const auto tip_bottom = ImVec2(canvas.Min.x + canvas.GetWidth() * 0.5f, bottom);

        drawList.PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
        drawList.PathBezierCurveTo(
            ImVec2(left, top),
            ImVec2(left, top),
            ImVec2(left, top) + ImVec2(rounding, 0));
        drawList.PathLineTo(tip_top);
        drawList.PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
        drawList.PathBezierCurveTo(
            tip_right,
            tip_right,
            tip_bottom + (tip_right - tip_bottom) * tip_round);
        drawList.PathLineTo(tip_bottom);
        drawList.PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
        drawList.PathBezierCurveTo(
            ImVec2(left, bottom),
            ImVec2(left, bottom),
            ImVec2(left, bottom) - ImVec2(0, rounding));

        if (!filled)
        {
            if (innerColor & 0xFF000000)
                drawList.AddConvexPolyFilled(drawList._Path.Data, drawList._Path.Size, innerColor);

            drawList.PathStroke(color, true, 2.0f * outline_scale);
        }
        else
            drawList.PathFillConvex(color);
    }
    else
    {
        auto triangleStart = rect.GetCenter().x + 0.32f * rect.GetWidth();

        rect.Min.x -= floorf(rect.GetWidth() * 0.25f * 0.525f);

        if (type == IconType::Circle)
        {
            const auto c = rect.GetCenter();

            if (!filled)
            {
                const auto r = 0.5f * rect.GetWidth() / 2.0f - 0.5f;

                if (innerColor & 0xFF000000)
                    drawList.AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                drawList.AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
            }
            else
                drawList.AddCircleFilled(c, 0.5f * rect.GetWidth() / 2.0f, color, 12 + extra_segments);
        }

        if (type == IconType::Square)
        {
            if (filled)
            {
                const auto r  = 0.5f * rect.GetWidth() / 2.0f;
                const auto p0 = rect.GetCenter() - ImVec2(r, r);
                const auto p1 = rect.GetCenter() + ImVec2(r, r);

                drawList.AddRectFilled(p0, p1, color, 0, 15 + extra_segments);
            }
            else
            {
                const auto r  = 0.5f * rect.GetWidth() / 2.0f - 0.5f;
                const auto p0 = rect.GetCenter() - ImVec2(r, r);
                const auto p1 = rect.GetCenter() + ImVec2(r, r);

                if (innerColor & 0xFF000000)
                    drawList.AddRectFilled(p0, p1, innerColor, 0, 15 + extra_segments);

                drawList.AddRect(p0, p1, color, 0, 15 + extra_segments, 2.0f * outline_scale);
            }
        }

        if (type == IconType::Grid)
        {
            const auto r = 0.5f * rect.GetWidth() / 2.0f;
            const auto w = ceilf(r / 3.0f);

            const auto baseTl = ImVec2(floorf(rect.GetCenter().x - w * 2.5f), floorf(rect.GetCenter().y - w * 2.5f));
            const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

            auto tl = baseTl;
            auto br = baseBr;
            for (int i = 0; i < 3; ++i)
            {
                tl.x = baseTl.x;
                br.x = baseBr.x;
                drawList.AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                if (i != 1 || filled)
                    drawList.AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                drawList.AddRectFilled(tl, br, color);

                tl.y += w * 2;
                br.y += w * 2;
            }

            triangleStart = br.x + w + 1.0f / 24.0f * rect.GetWidth();
        }

        if (type == IconType::RoundSquare)
        {
            if (filled)
            {
                const auto r  = 0.5f * rect.GetWidth() / 2.0f;
                const auto cr = r * 0.5f;
                const auto p0 = rect.GetCenter() - ImVec2(r, r);
                const auto p1 = rect.GetCenter() + ImVec2(r, r);

                drawList.AddRectFilled(p0, p1, color, cr, 15);
            }
            else
            {
                const auto r = 0.5f * rect.GetWidth() / 2.0f - 0.5f;
                const auto cr = r * 0.5f;
                const auto p0 = rect.GetCenter() - ImVec2(r, r);
                const auto p1 = rect.GetCenter() + ImVec2(r, r);

                if (innerColor & 0xFF000000)
                    drawList.AddRectFilled(p0, p1, innerColor, cr, 15);

                drawList.AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
            }
        }
        else if (type == IconType::Diamond)
        {
            if (filled)
            {
                const auto r = 0.607f * rect.GetWidth() / 2.0f;
                const auto c = rect.GetCenter();

                drawList.PathLineTo(c + ImVec2( 0, -r));
                drawList.PathLineTo(c + ImVec2( r,  0));
                drawList.PathLineTo(c + ImVec2( 0,  r));
                drawList.PathLineTo(c + ImVec2(-r,  0));
                drawList.PathFillConvex(color);
            }
            else
            {
                const auto r = 0.607f * rect.GetWidth() / 2.0f - 0.5f;
                const auto c = rect.GetCenter();

                drawList.PathLineTo(c + ImVec2( 0, -r));
                drawList.PathLineTo(c + ImVec2( r,  0));
                drawList.PathLineTo(c + ImVec2( 0,  r));
                drawList.PathLineTo(c + ImVec2(-r,  0));

                if (innerColor & 0xFF000000)
                    drawList.AddConvexPolyFilled(drawList._Path.Data, drawList._Path.Size, innerColor);

                drawList.PathStroke(color, true, 2.0f * outline_scale);
            }
        }
        else
        {
            const auto triangleTip = triangleStart + rect.GetWidth() * (0.45f - 0.32f);
            const auto c = rect.GetCenter();

            drawList.AddTriangleFilled(
                ImVec2(ceilf(triangleTip), rect.Min.y + rect.GetHeight() * 0.5f),
                ImVec2(triangleStart, c.y + 0.15f * rect.GetHeight()),
                ImVec2(triangleStart, c.y - 0.15f * rect.GetHeight()),
                color);
        }
    }
}

static void NodeIcon(const ImVec4& color)
{
    auto fontSize = ImGui::GetFrameHeight();//ImGui::GetFontSize();

    auto a = ImGui::GetCursorScreenPos();
    auto b = a + ImVec2(fontSize, fontSize);

    auto bb = ImRect(a, b);
    ImGui::ItemSize(bb, ImGui::GetStyle().FramePadding.y);
    ImGui::ItemAdd(bb, 0);

    auto drawList = ImGui::GetWindowDrawList();
    //drawList->AddRect(bb.GetTL(), bb.GetBR(), ImColor(color));
    ////bb.Min -= ImVec2(0.5f, 0.5f);
    //bb.Max -= ImVec2(0.5f, 0.5f);
    //drawList->AddLine(bb.GetTL(), bb.GetBR(), ImColor(color));
    //drawList->AddLine(bb.GetTR(), bb.GetBL(), ImColor(color));


    DrawIcon(*drawList, IconType::Diamond, a, b, false, ImColor(color), IM_COL32(0, 0, 0, 0));
}

static void Icon(IconType type, const ImVec4& color)
{
    auto fontSize = ImGui::GetFrameHeight();//ImGui::GetFontSize();

    auto a = ImGui::GetCursorScreenPos();
    auto b = a + ImVec2(fontSize, fontSize);

    auto bb = ImRect(a, b);
    ImGui::ItemSize(bb, ImGui::GetStyle().FramePadding.y);
    ImGui::ItemAdd(bb, 0);

    bool filled = ImGui::IsMouseHoveringRect(a, b);
    DrawIcon(*ImGui::GetWindowDrawList(), type, a, b, filled, ImColor(color), IM_COL32(0, 0, 0, 0));
}

static void BlendVertsLinearColorGradient(ImDrawVert* vert_start, ImDrawVert* vert_end, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = ImLerp((int)(col0 >> IM_COL32_R_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF, t);
        int g = ImLerp((int)(col0 >> IM_COL32_G_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF, t);
        int b = ImLerp((int)(col0 >> IM_COL32_B_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF, t);
        int a = ImLerp((int)(col0 >> IM_COL32_A_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_A_SHIFT) & 0xFF, t);

        float at = a / 255.0f;

        r = ImLerp((int)(vert->col >> IM_COL32_R_SHIFT) & 0xFF, r, at);
        g = ImLerp((int)(vert->col >> IM_COL32_G_SHIFT) & 0xFF, g, at);
        b = ImLerp((int)(vert->col >> IM_COL32_B_SHIFT) & 0xFF, b, at);

        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

static void DrawNodeFrame(const ImVec2& a, const ImVec2& b, float headerHeight, float topCut, float bottomCut, ImColor color, ImColor fillColor, float thickness = 1.0f)
{
    auto size         = b - a;
    auto shortestEdge = ImMin(size.x, size.y);
    topCut            = ImMin(shortestEdge, topCut);
    bottomCut         = ImMin(shortestEdge, bottomCut);

    auto drawList = ImGui::GetWindowDrawList();
    drawList->PathLineToMergeDuplicate(a + ImVec2(topCut, 0.0f));
    drawList->PathLineToMergeDuplicate(ImVec2(b.x, a.y));
    drawList->PathLineToMergeDuplicate(b - ImVec2(0.0f, bottomCut));
    drawList->PathLineToMergeDuplicate(b - ImVec2(bottomCut, 0.0f));
    drawList->PathLineToMergeDuplicate(ImVec2(a.x, b.y));
    drawList->PathLineToMergeDuplicate(a + ImVec2(0.0f, topCut));

    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, fillColor);

    auto gradientColor = ImColor(255, 255, 255, 128);

    drawList->PushClipRect(a, a + ImVec2(size.x, headerHeight), true);
    auto headerVertextIndexStart = drawList->_VtxCurrentIdx;
    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, fillColor);
    auto headerVertextIndexEnd = drawList->_VtxCurrentIdx;
    BlendVertsLinearColorGradient(
        drawList->VtxBuffer.Data + headerVertextIndexStart,
        drawList->VtxBuffer.Data + headerVertextIndexEnd,
        a + ImVec2(0.0f, 0.0f),
        a + ImVec2(size.x, size.x) * 0.5f,
        gradientColor,
        ImColor(0, 0, 0, 0));
    drawList->PopClipRect();

    auto lineColor = ImLerp(gradientColor, fillColor, gradientColor.Value.w);
    auto top = drawList->_Path.Size;
    drawList->PathLineTo(ImVec2(a.x, a.y + headerHeight) + drawList->_HalfPixel);
    drawList->PathLineTo(ImVec2(b.x - thickness * 0.5f, a.y + headerHeight) + drawList->_HalfPixel);
    drawList->AddPolyline(drawList->_Path.Data + top, 2, ImColor(lineColor), false, 1.0f / drawList->_InvTransformationScale);
    drawList->_Path.pop_back();
    drawList->_Path.pop_back();

    drawList->PathStroke(color, true, thickness);
}

struct BlueprintNodeSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Blueprint Node";
    }

    virtual void Frame() override
    {
        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail(),
            ImColor(64, 64, 64, 255));

        auto borderColor = ImColor(255, 255, 255, 64);
        auto flowColor   = ImColor(255, 255, 255, 255);
        auto intPinColor = ImColor( 68, 201, 156, 255);
        auto pinIconPadding = 5.0f;

        //DrawIcon(
        //    *drawList,
        //    IconType::Flow,
        //    ImVec2(100, 100),
        //    ImVec2(1000, 1000),
        //    false,
        //    IM_COL32(128, 128, 255, 255),
        //    IM_COL32(255, 255, 128, 255)
        //);

        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(10.0f, 10.0f));

        //ImGui::GetWindowDrawList()->ApplyTransformation(ImMatrix::Scaling(4.0f, 4.0f));

        drawList->ChannelsSplit(2);
        drawList->ChannelsSetCurrent(1);

        ImGui::BeginVertical("node");
            ImGui::Spring(0.0f, 1.0f);
            ImGui::BeginHorizontal("header");
                ImGui::Spring(0.0, 20.0f);
                NodeIcon(ImColor(255, 255, 255, 255));
                ImGui::SameLine(0.0f, pinIconPadding);
                ImGui::TextUnformatted("Do N");
//                     ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
//                     ImGui::TextUnformatted("Comment maybe?");
//                     ImGui::PopStyleVar();
                ImGui::Spring();
                //ImGui::Button("XxX");
                //ImGui::Spring(0.0);
            ImGui::EndHorizontal();
            ImGui::Spring(0, 0.0f);

            auto nodeHeaderBottom = ImGui::GetCursorScreenPos().y;

            ImGui::Spring(0);

            ImGui::BeginHorizontal("content", ImVec2(0.0f, 0.0f), 0.0f);
                ImGui::Spring();
                ImGui::BeginVertical("inputs", ImVec2(0.0f, 0.0f), 0.0f);
                    Icon(IconType::Flow, flowColor); ImGui::SameLine(0.0f, pinIconPadding); ImGui::TextUnformatted("Enter");
                    ImGui::Spring(0);
                    Icon(IconType::Circle, intPinColor); ImGui::SameLine(0.0f, pinIconPadding); ImGui::TextUnformatted("N");
                    ImGui::SameLine(0.0f, pinIconPadding);
                    static char value[32] = "1";
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                    ImGui::PushItemWidth(16.0f);
                    ImGui::InputText("##N", value, 32);
                    ImGui::PopItemWidth();
                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();
                    ImGui::Spring(0);
                    Icon(IconType::Flow, flowColor); ImGui::SameLine(0.0f, pinIconPadding); ImGui::TextUnformatted("Reset");
                    ImGui::Spring(1);
                ImGui::EndVertical();

                ImGui::Spring(1.0f, ImGui::GetStyle().ItemSpacing.x * 2.0f);

                ImGui::BeginVertical("outputs", ImVec2(0.0f, 0.0f), 1.0f);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Exit");    ImGui::SameLine(0.0f, pinIconPadding); Icon(IconType::Flow, flowColor);
                    ImGui::Spring(0);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted("Counter"); ImGui::SameLine(0.0f, pinIconPadding); Icon(IconType::Circle, intPinColor);
                    //ImGui::Spring(1);
                    //ImGui::Button("+ Add Output");
                ImGui::EndVertical();
                ImGui::Spring();

            ImGui::EndHorizontal();
        ImGui::EndVertical();

        drawList->ChannelsSetCurrent(0);

        auto normalColor   = ImColor(  0, 150, 245, 128);
        auto selectedColor = ImColor(241, 207,  99, 255);
        auto overColor     = ImColor(  0, 150, 245, 255);

        ImGui::SetCursorScreenPos(ImGui::GetItemRectMin());
        ImGui::InvisibleButton("dummyButton", ImGui::GetItemRectSize());

        auto color = ImGui::IsItemHovered() ? overColor : normalColor;
        if (ImGui::IsItemActive())
            color = selectedColor;

        DrawNodeFrame(
            ImGui::GetItemRectMin(),
            ImGui::GetItemRectMax(),
            nodeHeaderBottom - ImGui::GetItemRectMin().y,
            20.0f, 10.0f,
            color,
            ImColor(8, 44, 66, 255),
            2.0f);

        drawList->ChannelsMerge();

        //ImGui::GetWindowDrawList()->PopTransformation();
    }
};

std::unique_ptr<Sample> CreateBlueprintNodeSample(ax::Settings& m_Settings)
{
    return std::make_unique<BlueprintNodeSample>(m_Settings);
}
