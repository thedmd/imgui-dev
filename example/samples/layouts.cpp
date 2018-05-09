# include "samples.h"
# include <iostream>

extern "C" __declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);

static void DrawRegion(const char* name, ImColor color, float expand = 0.0f)
{
    auto drawList = ImGui::GetWindowDrawList();
    auto a = ImGui::GetItemRectMin();
    auto b = ImGui::GetItemRectMax();
    drawList->AddRectFilled(
        a - ImVec2(expand, expand),
        b + ImVec2(expand, expand),
        color);
    if (name)
    {
        char buffer[256];
        snprintf(buffer, 255, "%s\n%.0f x %0.f", name, (b.x - a.x), (b.y - a.y));
        auto textSize = ImGui::CalcTextSize(buffer, nullptr);
        drawList->AddText((b + a) * 0.5f - textSize * 0.5f, IM_COL32(0, 0, 0, 255), buffer);
    }
}

static void Region(const char* name, ImVec2 size, ImColor color)
{
    ImGui::InvisibleButton(name, size);
    DrawRegion(name, ImGui::IsItemHovered() ? ImColor(255, 255, 255, 240) : color);
}

static void DrawItemRect(ImColor color, float expand = 0.0f)
{
    ImGui::GetWindowDrawList()->AddRect(
        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        color);
};

template <typename F>
static void BoundedWidget(ImVec2 size, F&& f)
{
    const ImColor color(255, 255, 255);
    const float frame = 5;
    const float padding = 3;

    auto lastItemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::Dummy(ImVec2(0.0f, padding + frame));
    ImGui::Dummy(ImVec2(padding + frame, 0.0f));
    ImGui::SameLine();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, lastItemSpacing);
    f(size);
    ImGui::PopStyleVar();

    DrawItemRect(ImColor(255, 255, 255), 1);
    auto itemMin = ImGui::GetItemRectMin();
    auto itemMax = ImGui::GetItemRectMax();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(padding + frame, 0.0f));
    ImGui::Dummy(ImVec2(0.0f, padding + frame));
    ImGui::PopStyleVar();
    ImGui::EndGroup();

    auto drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        ImVec2(itemMin.x, itemMin.y - padding - frame),
        ImVec2(itemMax.x, itemMin.y - padding),
        color);

    drawList->AddRectFilled(
        ImVec2(itemMin.x, itemMax.y + padding),
        ImVec2(itemMax.x, itemMax.y + padding + frame),
        color);

    drawList->AddRectFilled(
        ImVec2(itemMin.x - padding - frame, itemMin.y),
        ImVec2(itemMin.x - padding,         itemMax.y),
        color);

    drawList->AddRectFilled(
        ImVec2(itemMax.x + padding,         itemMin.y),
        ImVec2(itemMax.x + padding + frame, itemMax.y),
        color);
}

static void DrawSpring(ImGuiLayout& layout, ImGuiLayoutItem& item)
{
    ImVec2 itemMin  = item.MeasuredBounds.Min;
    ImVec2 itemMax  = item.MeasuredBounds.Max;
    ImVec2 itemSize = item.MeasuredBounds.GetSize();
    if (itemSize.x <= 0.0f || itemSize.y <= 0.0f)
        return;

    auto drawList = ImGui::GetWindowDrawList();

    drawList->PushClipRect(itemMin, itemMax);

    auto width = 0.0f;
    ImVec2 direction, origin;

    auto spacing = floorf(item.SpringSpacing);

    if (layout.Type == ImGuiLayoutType_Horizontal)
    {
        width     = item.MeasuredBounds.GetWidth();
        origin    = ImVec2(item.MeasuredBounds.Min.x + spacing, item.MeasuredBounds.GetCenter().y);
        direction = ImVec2(1.0f, 0.0f);
    }
    else
    {
        width     = item.MeasuredBounds.GetHeight();
        origin    = ImVec2(item.MeasuredBounds.GetCenter().x, item.MeasuredBounds.Min.y + spacing);
        direction = ImVec2(0.0f, 1.0f);
    }

    drawList->AddRectFilled(itemMin + direction * spacing, itemMax, ImColor(255, 128, 255, 40));

    const float size = 3;
    ImVec2 normal = ImVec2(-direction.y, direction.x);

    drawList->PathClear();
    origin -= normal * (size * 0.5f);
    drawList->PathLineTo(origin);
    for (float x = size * 0.5f; x <= width; x += size)
    {
        ImVec2 p = origin + direction * x + normal * size;
        drawList->PathLineTo(p);
        normal = ImVec2(-normal.x, -normal.y);
    }
    drawList->PathStroke(ImColor(255, 255, 255, 190), false, 1.0f);

    drawList->PopClipRect();

    drawList->AddRect(itemMin + direction * spacing, itemMax, ImColor(255, 128, 255, 60));
}

static void DrawSprings(ImGuiLayout& layout)
{
    for (auto& item : layout.Items)
        if (item.Type == ImGuiLayoutItemType_Spring)
            DrawSpring(layout, item);
}

static void DrawAllSprings()
{
    auto windows = ImGui::GetCurrentWindow();
    for (auto& pair : windows->DC.Layouts.Data)
    {
        ImGuiLayout* layout = (ImGuiLayout*)pair.val_p;
        if (layout->Live)
            DrawSprings(*layout);
    }
}

struct LayoutSample: Sample
{
    using Sample::Sample;

    virtual const char* Name() const override
    {
        return "Layouts";
    }

    virtual void Frame() override
    {
        if (sleep > 0)
        {
            Sleep(1000);
            --sleep;
        }

        auto origin = ImGui::GetCursorScreenPos();

        auto drawRect2 = [](ImVec2 position, ImVec2 size, ImColor color, float padding = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRect(
                position - ImVec2(padding, padding), position + size + ImVec2(padding, padding), color);
        };

        //auto fillRect = [](ImVec2 position, ImVec2 size, ImColor color)
        //{
        //    ImGui::GetWindowDrawList()->AddRectFilled(
        //        position, position + size, color);
        //};

        auto drawItemRect = [](ImColor color, float expand = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRect(
                ImGui::GetItemRectMin() - ImVec2(expand, expand),
                ImGui::GetItemRectMax() + ImVec2(expand, expand),
                color);
        };

        auto fillItemRect = [](ImColor color, float expand = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetItemRectMin() - ImVec2(expand, expand),
                ImGui::GetItemRectMax() + ImVec2(expand, expand),
                color);
        };

        auto fillRect = [](float w, float h, ImColor color, float expand = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos() - ImVec2(expand, expand),
                ImGui::GetCursorScreenPos() + ImVec2(w + expand, h + expand),
                color);
        };

        auto drawRect = [](float w, float h, ImColor color, float expand = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRect(
                ImGui::GetCursorScreenPos() - ImVec2(expand, expand),
                ImGui::GetCursorScreenPos() + ImVec2(w + expand, h + expand),
                color);
        };

        auto borderColor = ImColor(0, 0, 0, 255);

        ImVec2 bounds  = ImVec2(     600,      400);
        ImVec2 boundsH = ImVec2(bounds.x,       40);
        ImVec2 boundsV = ImVec2(      80, bounds.y);

        auto& style = ImGui::GetStyle();

        static auto tabIndex      = MakeProperty("Tab Index", 0, m_Settings);
        static auto enableSleep   = MakeProperty("Enable Sleep", false, m_Settings);
        static auto showSprings   = MakeProperty("Show Springs", true, m_Settings);

        if (ImGui::RadioButton("Basic", tabIndex == 0)) { tabIndex.Value = 0; tabIndex.Save(m_Settings); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Composite", tabIndex == 1)) { tabIndex.Value = 1; tabIndex.Save(m_Settings); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Dynamic", tabIndex == 2)) { tabIndex.Value = 2; tabIndex.Save(m_Settings); }
        ImGui::SameLine();
        ImGui::VerticalSeparator();
        ImGui::SameLine();
        if (ImGui::Checkbox(showSprings.Name.c_str(), &showSprings)) { showSprings.Save(m_Settings); }

        ImGui::Separator();

        static float t = 0.0f;
        t += ImGui::GetIO().DeltaTime;

        if (tabIndex == 0)
        {
            static auto itemSpacing   = MakeProperty("Layout Item Spacing", style.ItemSpacing, m_Settings);
            static auto springWeight  = MakeProperty("Spring Weight", 0.5f, m_Settings);
            static auto alignment     = MakeProperty("Alignment", 0.5f, m_Settings);
            static auto showA         = MakeProperty("A", true, m_Settings);
            static auto showB         = MakeProperty("B", true, m_Settings);
            static auto showC         = MakeProperty("C", true, m_Settings);
            static auto showSA        = MakeProperty("Spring A", true, m_Settings);
            static auto showSB        = MakeProperty("Spring B", true, m_Settings);
            static auto autoWidth     = MakeProperty("Auto Width", false, m_Settings);
            static auto autoHeight    = MakeProperty("Auto Height", false, m_Settings);

            if (ImGui::DragFloat2(itemSpacing.Name.c_str(), &itemSpacing->x, 0.1f, 0.0f, 50.0f))
                itemSpacing.Save(m_Settings);
            if (ImGui::DragFloat(springWeight.Name.c_str(), &springWeight, 0.002f, 0.0f, 1.0f))
                springWeight.Save(m_Settings);
            if (ImGui::DragFloat(alignment.Name.c_str(), &alignment, 0.002f, 0.0f, 1.0f))
                alignment.Save(m_Settings);
            if (ImGui::Checkbox(showA.Name.c_str(),  &showA))  { sleep = 1; showA.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showB.Name.c_str(),  &showB))  { sleep = 1; showB.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showC.Name.c_str(),  &showC))  { sleep = 1; showC.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showSA.Name.c_str(), &showSA)) { sleep = 1; showSA.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(showSB.Name.c_str(), &showSB)) { sleep = 1; showSB.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoWidth.Name.c_str(), &autoWidth)) { sleep = 1; autoWidth.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoHeight.Name.c_str(), &autoHeight)) { sleep = 1; autoHeight.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(enableSleep.Name.c_str(), &enableSleep))
                enableSleep.Save(m_Settings);
            if (!enableSleep)
                sleep = 0;
            if (sleep)
            {
                ImGui::SameLine();
                auto drawList = ImGui::GetWindowDrawList();
                drawList->ChannelsSplit(2);
                drawList->ChannelsSetCurrent(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                ImGui::Text("Sleeping...");
                ImGui::PopStyleColor();
                drawList->ChannelsSetCurrent(0);
                fillItemRect(ImColor(255, 255, 128), 4);
                drawList->ChannelsMerge();
            }

            auto items                = (showA ? 1 : 0) + (showB ? 1 : 0) + (showC ? 1 : 0);
            auto itemSpacings         = items > 1 ? items - 1 : 0;
            auto springs              = (showSA ? 1 : 0) + (showSB ? 1 : 0);
            auto refWidth             = 200.0f * items;
            auto refHeight            =  80.0f * items;
            auto refWidthWithSprings  = refWidth  + (springs * floorf(itemSpacing->x));
            auto refHeightWithSprings = refHeight + (springs * floorf(itemSpacing->y));

            auto applySizeRules = [&](ImVec2 size)
            {
                if (autoWidth)  size.x = 0;
                if (autoHeight) size.y = 0;
                return size;
            };
# if 1
            ImGui::Text("Basic Horizontal");

            BoundedWidget(ImVec2(refWidth + itemSpacings * floorf(itemSpacing->x), boundsH.y), [&](ImVec2 size)
            {
                ImGui::BeginHorizontal("h1", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(itemSpacing->x, style.ItemSpacing.y));
                if (showA)
                    Region("A", ImVec2(200, size.y), ImColor(255, 128, 128));
                if (showB)
                    Region("B", ImVec2(200, size.y), ImColor(128, 255, 128));
                if (showC)
                    Region("C", ImVec2(200, size.y), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndHorizontal();
            });
# endif

# if 1
            ImGui::Text("Basic Horizontal With Zero Springs");

            BoundedWidget(ImVec2(refWidthWithSprings, boundsH.y), [&](ImVec2 size)
            {
                ImGui::BeginHorizontal("h2", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, style.ItemSpacing.y));
                if (showA)
                    Region("A", ImVec2(200, size.y), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(0.0f, itemSpacing->x);
                if (showB)
                    Region("B", ImVec2(200, size.y), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(0.0f, itemSpacing->x);
                if (showC)
                    Region("C", ImVec2(200, size.y), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndHorizontal();
            });
# endif

# if 1
            ImGui::Text("Basic Horizontal With Springs");

            BoundedWidget(ImVec2(refWidthWithSprings, boundsH.y), [&](ImVec2 size)
            {
                ImGui::BeginHorizontal("h3", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, style.ItemSpacing.y));
                if (showA)
                    Region("A", ImVec2(200, size.y), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(springWeight, itemSpacing->x);
                if (showB)
                    Region("B", ImVec2(100, size.y), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(1.0f - springWeight, itemSpacing->x);
                if (showC)
                    Region("C", ImVec2(200, size.y), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndHorizontal();
            });
# endif

# if 1
            ImGui::Text("Vertical Alignment");

            BoundedWidget(ImVec2(refWidthWithSprings, boundsH.y), [&](ImVec2 size)
            {
                ImGui::BeginHorizontal("ch1", applySizeRules(size), alignment);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, style.ItemSpacing.y));
                if (showA)
                    Region("A", ImVec2(200, size.y), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(0.0f, itemSpacing->x);
                if (showB)
                    Region("B", ImVec2(200, size.y / 4), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(0.0f, itemSpacing->x);
                if (showC)
                    Region("C", ImVec2(200, size.y), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndHorizontal();
            });
# endif

            ImGui::Separator();

            ImGui::Columns(4, 0, false);

# if 1
            ImGui::Text("\nBasic Vertical");

            BoundedWidget(ImVec2(boundsV.x, refHeight + itemSpacings * floorf(itemSpacing->y)), [&](ImVec2 size)
            {
                ImGui::BeginVertical("v1", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, itemSpacing->y));
                if (showA)
                    Region("A", ImVec2(size.x, 80), ImColor(255, 128, 128));
                if (showB)
                    Region("B", ImVec2(size.x, 80), ImColor(128, 255, 128));
                if (showC)
                    Region("C", ImVec2(size.x, 80), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });

            ImGui::NextColumn();
# endif

# if 1
            ImGui::Text(" Basic Vertical\n"
                "With Zero Springs");

            BoundedWidget(ImVec2(boundsV.x, refHeightWithSprings), [&](ImVec2 size)
            {
                ImGui::BeginVertical("v2", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, 0.0f));
                if (showA)
                    Region("A", ImVec2(size.x, 80), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(0.0f, itemSpacing->y);
                if (showB)
                    Region("B", ImVec2(size.x, 80), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(0.0f, itemSpacing->y);
                if (showC)
                    Region("C", ImVec2(size.x, 80), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });

            ImGui::NextColumn();
# endif

# if 1
            ImGui::Text("Basic Vertical\n"
                " With Springs");

            BoundedWidget(ImVec2(boundsV.x, refHeightWithSprings), [&](ImVec2 size)
            {
                ImGui::BeginVertical("v3", applySizeRules(size));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, 0.0f));
                if (showA)
                    Region("A", ImVec2(size.x, 80), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(springWeight, itemSpacing->y);
                if (showB)
                    Region("B", ImVec2(size.x, 40), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(1.0f - springWeight, itemSpacing->y);
                if (showC)
                    Region("C", ImVec2(size.x, 80), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });

            ImGui::NextColumn();
# endif

# if 1
            ImGui::Text("Horizontal\n"
                "Alignment");

            BoundedWidget(ImVec2(boundsV.x, refHeightWithSprings), [&](ImVec2 size)
            {
                ImGui::BeginVertical("cv1", applySizeRules(size), alignment);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, 0.0f));
                if (showA)
                    Region("A", ImVec2(size.x, 80), ImColor(255, 128, 128));
                if (showSA)
                    ImGui::Spring(0.0f, itemSpacing->y);
                if (showB)
                    Region("B", ImVec2(size.x / 4, 80), ImColor(128, 255, 128));
                if (showSB)
                    ImGui::Spring(0.0f, itemSpacing->y);
                if (showC)
                    Region("C", ImVec2(size.x, 80), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });

            ImGui::NextColumn();
# endif

            ImGui::Columns(1);
        }
        else if (tabIndex == 1)
        {
            static auto itemSpacing   = MakeProperty("Layout Item Spacing##complex", style.ItemSpacing, m_Settings);
            static auto springWeight  = MakeProperty("Spring Weight##complex", 0.5f, m_Settings);
            static auto alignment     = MakeProperty("Alignment##complex", 0.5f, m_Settings);
            static auto showA         = MakeProperty("A##complex", true, m_Settings);
            static auto showB         = MakeProperty("B##complex", true, m_Settings);
            static auto showC         = MakeProperty("C##complex", true, m_Settings);
            static auto showSA        = MakeProperty("Spring A##complex", true, m_Settings);
            static auto showSB        = MakeProperty("Spring B##complex", true, m_Settings);
            static auto autoWidth     = MakeProperty("Auto Width##complex", false, m_Settings);
            static auto autoHeight    = MakeProperty("Auto Height##complex", false, m_Settings);
            static auto width         = MakeProperty("Width##complex", 400.0f, m_Settings);
            static auto height        = MakeProperty("Height##comple", 350.0f, m_Settings);

            static auto itemSpacing2  = MakeProperty("Layout Item Spacing##complex#inner1", style.ItemSpacing, m_Settings);
            static auto springWeight2 = MakeProperty("Spring Weight##complex#inner1", 0.5f, m_Settings);
            static auto alignment2    = MakeProperty("Alignment##complex#inner1", 0.5f, m_Settings);
            static auto showA2        = MakeProperty("A##complex#inner1", true, m_Settings);
            static auto showB2        = MakeProperty("B##complex#inner1", true, m_Settings);
            static auto showC2        = MakeProperty("C##complex#inner1", true, m_Settings);
            static auto showSA2       = MakeProperty("Spring A##complex#inner1", true, m_Settings);
            static auto showSB2       = MakeProperty("Spring B##complex#inner1", true, m_Settings);
            static auto autoWidth2    = MakeProperty("Auto Width##complex#inner1", false, m_Settings);
            static auto autoHeight2   = MakeProperty("Auto Height##complex#inner1", false, m_Settings);
            static auto width2        = MakeProperty("Width##complex#inner1", 350.0f, m_Settings);
            static auto height2       = MakeProperty("Height##complex#inner1", 110.0f, m_Settings);

            static auto itemSpacing3  = MakeProperty("Layout Item Spacing##complex#inner2", style.ItemSpacing, m_Settings);
            static auto springWeight3 = MakeProperty("Spring Weight##complex#inner2", 0.5f, m_Settings);
            static auto alignment3    = MakeProperty("Alignment##complex#inner2", 0.5f, m_Settings);
            static auto showA3        = MakeProperty("A##complex#inner2", true, m_Settings);
            static auto showB3        = MakeProperty("B##complex#inner2", true, m_Settings);
            static auto showC3        = MakeProperty("C##complex#inner2", true, m_Settings);
            static auto showSA3       = MakeProperty("Spring A##complex#inner2", true, m_Settings);
            static auto showSB3       = MakeProperty("Spring B##complex#inner2", true, m_Settings);
            static auto autoWidth3    = MakeProperty("Auto Width##complex#inner2", false, m_Settings);
            static auto autoHeight3   = MakeProperty("Auto Height##complex#inner2", false, m_Settings);
            static auto width3        = MakeProperty("Width##complex#inner2", 100.0f, m_Settings);
            static auto height3       = MakeProperty("Height##complex#inner2", 100.0f, m_Settings);

            if (ImGui::Checkbox(enableSleep.Name.c_str(), &enableSleep))
                enableSleep.Save(m_Settings);
            if (!enableSleep)
                sleep = 0;
            if (sleep)
            {
                ImGui::SameLine();
                auto drawList = ImGui::GetWindowDrawList();
                drawList->ChannelsSplit(2);
                drawList->ChannelsSetCurrent(1);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
                ImGui::Text("Sleeping...");
                ImGui::PopStyleColor();
                drawList->ChannelsSetCurrent(0);
                fillItemRect(ImColor(255, 255, 128), 4);
                drawList->ChannelsMerge();
            }

            ImGui::Separator();

            ImGui::Columns(2);

            auto inner2 = [&]()
            {
                auto innerSize2 = ImVec2(width3, height3);
                if (autoWidth3)
                    innerSize2.x = 0;
                if (autoHeight3)
                    innerSize2.y = 0;
                ImGui::BeginVertical("v2", innerSize2, alignment3);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing3);
                if (showA3)
                    Region("A", ImVec2(50, 20), ImColor(255, 160, 160));
                if (showSA3)
                    ImGui::Spring(springWeight3, itemSpacing3->y);
                if (showB3)
                    Region("B", ImVec2(50, 20), ImColor(160, 255, 160));
                if (showSB3)
                    ImGui::Spring(1.0f - springWeight3, itemSpacing3->y);
                if (showC3)
                    Region("C", ImVec2(50, 20), ImColor(160, 160, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            };

            auto inner1 = [&]()
            {
                auto innerSize1 = ImVec2(width2, height2);
                if (autoWidth2)
                    innerSize1.x = 0;
                if (autoHeight2)
                    innerSize1.y = 0;
                ImGui::BeginHorizontal("h1", innerSize1, alignment2);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing2);
                if (showA2)
                    Region("A", ImVec2(50, 70), ImColor(255, 160, 160));
                if (showSA2)
                    ImGui::Spring(springWeight2, itemSpacing2->x);
                if (showB2)
                    inner2();
                if (showSB2)
                    ImGui::Spring(1.0f - springWeight2, itemSpacing2->x);
                if (showC2)
                    Region("C", ImVec2(50, 70), ImColor(160, 160, 255));
                ImGui::PopStyleVar();
                ImGui::EndHorizontal();
            };

            BoundedWidget(ImVec2(0.0f, 0.0f), [&](ImVec2 size)
            {
                size = ImVec2(width, height);
                if (autoWidth)
                    size.x = 0.0f;
                if (autoHeight)
                    size.y = 0.0f;

                ImGui::BeginVertical("v1", size, alignment);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);
                if (showA)
                {
                    Region("A", ImVec2(300, 100), ImColor(255, 128, 128));
                }
                if (showSA)
                    ImGui::Spring(springWeight, itemSpacing->y);
                if (showB)
                    inner1();
                if (showSB)
                    ImGui::Spring(1.0f - springWeight, itemSpacing->y);
                if (showC)
                {
                    Region("C", ImVec2(300, 100), ImColor(128, 128, 255));
                }
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });

            ImGui::NextColumn();

            if (ImGui::DragFloat2(itemSpacing.Name.c_str(), &itemSpacing->x, 0.1f, 0.0f, 50.0f))
                itemSpacing.Save(m_Settings);
            if (ImGui::DragFloat(springWeight.Name.c_str(), &springWeight, 0.002f, 0.0f, 1.0f))
                springWeight.Save(m_Settings);
            if (ImGui::DragFloat(alignment.Name.c_str(), &alignment, 0.002f, 0.0f, 1.0f))
                alignment.Save(m_Settings);
            if (ImGui::Checkbox(showA.Name.c_str(),  &showA))  { sleep = 1; showA.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showB.Name.c_str(),  &showB))  { sleep = 1; showB.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showC.Name.c_str(),  &showC))  { sleep = 1; showC.Save(m_Settings);  }
            if (ImGui::Checkbox(showSA.Name.c_str(), &showSA)) { sleep = 1; showSA.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(showSB.Name.c_str(), &showSB)) { sleep = 1; showSB.Save(m_Settings); }
            if (ImGui::Checkbox(autoWidth.Name.c_str(), &autoWidth)) { sleep = 1; autoWidth.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoHeight.Name.c_str(), &autoHeight)) { sleep = 1; autoHeight.Save(m_Settings); }
            if (ImGui::DragFloat(width.Name.c_str(), &width, 1.0f, 1.0f, 550.0f)) width.Save(m_Settings);
            if (ImGui::DragFloat(height.Name.c_str(), &height, 1.0f, 1.0f, 550.0f)) height.Save(m_Settings);
            ImGui::NewLine();
            if (!enableSleep)
                sleep = 0;
            //ImGui::NextColumn();
            ImGui::Columns(1);

            ImGui::Separator();

            ImGui::Columns(2);

            BoundedWidget(ImVec2(0.0f, 0.0f), [&](ImVec2)
            {
                inner1();
            });

            ImGui::NextColumn();

            if (ImGui::DragFloat2(itemSpacing2.Name.c_str(), &itemSpacing2->x, 0.1f, 0.0f, 50.0f))
                itemSpacing2.Save(m_Settings);
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
                springWeight2.Value -= 0.002f;
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
                springWeight2.Value += 0.002f;
            if (ImGui::DragFloat(springWeight2.Name.c_str(), &springWeight2, 0.002f, 0.0f, 1.0f))
                springWeight2.Save(m_Settings);
            //ImGui::LogText("%g\n", springWeight2.Value);
            if (ImGui::DragFloat(alignment2.Name.c_str(), &alignment2, 0.002f, 0.0f, 1.0f))
                alignment2.Save(m_Settings);
            if (ImGui::Checkbox(showA2.Name.c_str(),  &showA2))  { sleep = 1; showA2.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showB2.Name.c_str(),  &showB2))  { sleep = 1; showB2.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showC2.Name.c_str(),  &showC2))  { sleep = 1; showC2.Save(m_Settings);  }
            if (ImGui::Checkbox(showSA2.Name.c_str(), &showSA2)) { sleep = 1; showSA2.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(showSB2.Name.c_str(), &showSB2)) { sleep = 1; showSB2.Save(m_Settings); }
            if (ImGui::Checkbox(autoWidth2.Name.c_str(), &autoWidth2)) { sleep = 1; autoWidth2.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoHeight2.Name.c_str(), &autoHeight2)) { sleep = 1; autoHeight2.Save(m_Settings); }
            if (ImGui::DragFloat(width2.Name.c_str(), &width2, 1.0f, 1.0f, 550.0f)) width2.Save(m_Settings);
            if (ImGui::DragFloat(height2.Name.c_str(), &height2, 1.0f, 1.0f, 550.0f)) height2.Save(m_Settings);
            ImGui::NewLine();
            if (!enableSleep)
                sleep = 0;
            //ImGui::NextColumn();
            ImGui::Columns(1);

            ImGui::Separator();

            ImGui::Columns(2);

            BoundedWidget(ImVec2(0.0f, 0.0f), [&](ImVec2)
            {
                inner2();
            });

            ImGui::NextColumn();

            if (ImGui::DragFloat2(itemSpacing3.Name.c_str(), &itemSpacing3->x, 0.1f, 0.0f, 50.0f))
                itemSpacing3.Save(m_Settings);
            if (ImGui::DragFloat(springWeight3.Name.c_str(), &springWeight3, 0.003f, 0.0f, 1.0f))
                springWeight3.Save(m_Settings);
            if (ImGui::DragFloat(alignment3.Name.c_str(), &alignment3, 0.003f, 0.0f, 1.0f))
                alignment3.Save(m_Settings);
            if (ImGui::Checkbox(showA3.Name.c_str(),  &showA3))  { sleep = 1; showA3.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showB3.Name.c_str(),  &showB3))  { sleep = 1; showB3.Save(m_Settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showC3.Name.c_str(),  &showC3))  { sleep = 1; showC3.Save(m_Settings);  }
            if (ImGui::Checkbox(showSA3.Name.c_str(), &showSA3)) { sleep = 1; showSA3.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(showSB3.Name.c_str(), &showSB3)) { sleep = 1; showSB3.Save(m_Settings); }
            if (ImGui::Checkbox(autoWidth3.Name.c_str(), &autoWidth3)) { sleep = 1; autoWidth3.Save(m_Settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoHeight3.Name.c_str(), &autoHeight3)) { sleep = 1; autoHeight3.Save(m_Settings); }
            if (ImGui::DragFloat(width3.Name.c_str(), &width3, 1.0f, 1.0f, 550.0f)) width3.Save(m_Settings);
            if (ImGui::DragFloat(height3.Name.c_str(), &height3, 1.0f, 1.0f, 550.0f)) height3.Save(m_Settings);
            ImGui::NewLine();
            if (!enableSleep)
                sleep = 0;
            //ImGui::NextColumn();
            ImGui::Columns(1);
        }
        if (tabIndex == 2)
        {
            ImVec2 itemSpacing = ImVec2(0.0f, 0.0f);
            float springWeight = 0.5f;
            float animationTime = t * 1.0f;
            float alignment = 0.5f + 0.5f * sinf(cosf(animationTime * 2.5f));

            BoundedWidget(ImVec2(0.0f, 0.0f), [&](ImVec2 size)
            {
                ImGui::BeginVertical("v1", ImVec2(0, 0), alignment);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);
                if (fmod(animationTime, 2.0f) > 1.0f)
                    Region("A", ImVec2(300, 100 * sinf((fmod(animationTime, 2.0f) - 1.0f) * 3.14f)), ImColor(255, 128, 128));
                ImGui::Spring(springWeight, itemSpacing.y);
                Region("B", ImVec2(200 + sinf(animationTime) * 180, 200 + cosf(animationTime * 1.1f) * 180), ImColor(128, 255, 128));
                ImGui::Spring(1.0f - springWeight, itemSpacing.y);
                Region("C", ImVec2(100, 100), ImColor(128, 128, 255));
                ImGui::PopStyleVar();
                ImGui::EndVertical();
            });
            ImGui::SameLine();
            Region("Dummy", ImVec2(200, 200), ImColor(128, 128, 128));
            Region("Dummy", ImVec2(200, 200), ImColor(128, 128, 128));
        }

        if (showSprings)
            DrawAllSprings();
    }

private:
    int sleep = 0;
};

std::unique_ptr<Sample> CreateLayoutsSample(ax::Settings& m_Settings)
{
    return std::make_unique<LayoutSample>(m_Settings);
}
