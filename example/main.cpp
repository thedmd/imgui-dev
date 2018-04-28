// ImGui - standalone example application for DirectX 11
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#undef IMGUI_EXPERIMENTAL

#include <imgui.h>
#include "imgui_impl_dx11.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <string>
#include "support/Settings.h"

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

HRESULT CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return E_FAIL;

    CreateRenderTarget();

    return S_OK;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                ImGui_ImplDX11_InvalidateDeviceObjects();
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
                ImGui_ImplDX11_CreateDeviceObjects();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//inline ImMatrix operator * (const ImMatrix& lhs, const ImMatrix& rhs)
//{
//    return ImMatrix::Combine(lhs, rhs);
//}

inline std::ostream& operator <<(std::ostream& stream, const ImVec2& value)
{
    stream << value.x << ' ' << value.y;
    return stream;
}

inline std::istream& operator >>(std::istream& stream, ImVec2& value)
{
    stream >> value.x >> value.y;
    return stream;
}

template <typename T>
struct Property
{
    std::string Name;
    T           Value;

    Property(std::string_view name, T value)
        : Name(name)
        , Value(value)
    {
    }

    Property(std::string_view name, T value, const ax::Settings& settings)
        : Name(name)
        , Value(settings.Get<T>(name).value_or(value))
    {
    }

    void Save(ax::Settings& settings)
    {
        settings.Set(Name, Value);
    }

    operator const T&() const { return Value; }
    operator       T&()       { return Value; }

    const T* operator &() const { return &Value; }
          T* operator &()       { return &Value; }

    const T* operator->() const { return &Value; }
          T* operator->()       { return &Value; }
};

template <typename T, typename... Args>
auto MakeProperty(std::string_view name, T&& value, Args&&... args)
{
    return Property<std::decay_t<T>>(name, std::forward<T>(value), std::forward<Args>(args)...);
}


void DrawRegion(const char* name, ImColor color, float expand = 0.0f)
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

void Region(const char* name, ImVec2 size, ImColor color)
{
    ImGui::Dummy(size);
    DrawRegion(name, color);
}

void DrawItemRect(ImColor color, float expand = 0.0f)
{
    ImGui::GetWindowDrawList()->AddRect(
        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        color);
};

template <typename F>
void BoundedWidget(ImVec2 size, F&& f)
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


int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
//int main(int, char**)
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(_T("ImGui Example"), _T("ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (CreateDeviceD3D(hwnd) < 0)
    {
        CleanupDeviceD3D();
        UnregisterClass(_T("ImGui Example"), wc.hInstance);
        return 1;
    }

    // Show the window
    //ShowWindow(hwnd, SW_SHOWDEFAULT);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui_ImplDX11_Init(hwnd, g_pd3dDevice, g_pd3dDeviceContext);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ax::Settings settings;
    settings.Load("example.json");

    //ImGuiIO& io = ImGui::GetIO();
    //io.SaveIniCb = [](const char* buffer, size_t size)
    //{
    //    mySettings.assign(buffer, size);
    //};

    //io.LoadIniCb = [](char* buffer, size_t size) -> size_t
    //{
    //    if (buffer)
    //        memcpy(buffer, mySettings.data(), size);
    //    return mySettings.size();
    //};



    // Setup ImGui binding
    ImGui_ImplDX11_Init(hwnd, g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

//     bool show_test_window = true;
//     bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f * 0.5f, 0.55f * 0.5f, 0.60f * 0.5f, 1.00f);

//     auto& style = ImGui::GetStyle();
//     style.Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255);
//     style.ItemSpacing = ImVec2(10, 10);
//     style.Colors[ImGuiCol_Text] = ImColor(0, 0, 0);
//     style.Colors[ImGuiCol_CheckMark] = ImColor(110, 110, 110);
    int sleep = 0;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        ImGui_ImplDX11_NewFrame();

        if (sleep > 0)
        {
            Sleep(1000);
            --sleep;
        }

        auto origin = ImGui::GetCursorScreenPos();

# if defined(IMGUI_EXPERIMENTAL)
        ImGui::GetCurrentContext()->OverlayDrawList.AddText(ImVec2(10, 10), IM_COL32_WHITE, "Experimental");
# else
        ImGui::GetCurrentContext()->OverlayDrawList.AddText(ImVec2(10, 10), IM_COL32_WHITE, "Master");
# endif
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

# if defined(IMGUI_EXPERIMENTAL) || 1
        auto borderColor = ImColor(0, 0, 0, 255);

        ImVec2 bounds  = ImVec2(     600,      400);
        ImVec2 boundsH = ImVec2(bounds.x,       40);
        ImVec2 boundsV = ImVec2(      80, bounds.y);

        auto& style = ImGui::GetStyle();

        static auto tabIndex      = MakeProperty("Tab Index", 0, settings);
        static auto itemSpacing   = MakeProperty("Layout Item Spacing", style.ItemSpacing, settings);
        static auto springWeight  = MakeProperty("Spring Weight", 0.5f, settings);
        static auto alignment     = MakeProperty("Alignment", 0.5f, settings);
        static auto showA         = MakeProperty("A", true, settings);
        static auto showB         = MakeProperty("B", true, settings);
        static auto showC         = MakeProperty("C", true, settings);
        static auto showSA        = MakeProperty("Spring A", true, settings);
        static auto showSB        = MakeProperty("Spring B", true, settings);
        static auto autoWidth     = MakeProperty("Auto Width", false, settings);
        static auto autoHeight    = MakeProperty("Auto Height", false, settings);
        static auto enableSleep   = MakeProperty("Enable Sleep", false, settings);

        if (ImGui::RadioButton("Basic", tabIndex == 0)) { tabIndex.Value = 0; tabIndex.Save(settings); }
        if (ImGui::RadioButton("Composite", tabIndex == 1)) { tabIndex.Value = 1; tabIndex.Save(settings); }

        if (tabIndex == 0)
        {
            ImGui::Separator();

            if (ImGui::DragFloat2(itemSpacing.Name.c_str(), &itemSpacing->x, 0.1f, 0.0f, 50.0f))
                itemSpacing.Save(settings);
            if (ImGui::DragFloat(springWeight.Name.c_str(), &springWeight, 0.002f, 0.0f, 1.0f))
                springWeight.Save(settings);
            if (ImGui::DragFloat(alignment.Name.c_str(), &alignment, 0.002f, 0.0f, 1.0f))
                alignment.Save(settings);
            if (ImGui::Checkbox(showA.Name.c_str(),  &showA))  { sleep = 1; showA.Save(settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showB.Name.c_str(),  &showB))  { sleep = 1; showB.Save(settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showC.Name.c_str(),  &showC))  { sleep = 1; showC.Save(settings);  } ImGui::SameLine();
            if (ImGui::Checkbox(showSA.Name.c_str(), &showSA)) { sleep = 1; showSA.Save(settings); } ImGui::SameLine();
            if (ImGui::Checkbox(showSB.Name.c_str(), &showSB)) { sleep = 1; showSB.Save(settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoWidth.Name.c_str(), &autoWidth)) { sleep = 1; autoWidth.Save(settings); } ImGui::SameLine();
            if (ImGui::Checkbox(autoHeight.Name.c_str(), &autoHeight)) { sleep = 1; autoHeight.Save(settings); } ImGui::SameLine();
            if (ImGui::Checkbox(enableSleep.Name.c_str(), &enableSleep))
                enableSleep.Save(settings);
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

            static float t = 0.0f;
            t += ImGui::GetIO().DeltaTime;

            ImGui::Separator();

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




        //ImGui::Text("Basic Vertical");

        //ImGui::Dummy(ImVec2(5, 3 * boundsH.y + 2 * floorf(itemSpacing->y)));
        //drawRegion("ref", ImColor(255, 255, 255));
        //ImGui::SameLine();
        //ImGui::BeginVertical("v1", boundsH);
        //if (!useSprings)
        //    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, itemSpacing->y));
        //ImGui::Dummy(ImVec2(200, boundsH.y));
        //drawRegion("A", ImColor(255, 128, 128));
        //if (useSprings)
        //    ImGui::Spring(1.0f);
        //ImGui::Dummy(ImVec2(200, boundsH.y));
        //drawRegion("B", ImColor(128, 255, 128));
        //if (useSprings)
        //    ImGui::Spring(1.0f);
        //ImGui::Dummy(ImVec2(200, boundsH.y));
        //drawRegion("C", ImColor(128, 128, 255));
        //if (!useSprings)
        //    ImGui::PopStyleVar();
        //ImGui::EndVertical();

        /*
        drawRect(600, boundsH.y, borderColor, 2);
        ImGui::BeginHorizontal("h1", boundsH);
            if (h_first)
            {
                ImGui::Dummy(ImVec2(200, boundsH.y));
                fillItemRect(ImColor(255, 128, 128));
            }
            //ImGui::BeginVertical("v1");
            //ImGui::Dummy(ImVec2(200, boundsH.y / 2));
            //fillItemBounds(ImColor(255, 128, 128));
            ImGui::Spring(0, 0);
            //ImGui::Dummy(ImVec2(200, boundsH.y / 2));
            //fillItemBounds(ImColor(128, 128, 255));
            //ImGui::EndVertical();
        //ImGui::Spring(0,0);
            //ImGui::BeginHorizontal("h1");
                ImGui::BeginVertical("v1");
                ImGui::Dummy(ImVec2(h0_width, 40));
                fillItemRect(ImColor(128, 255, 128));
                ImGui::Spring(0, 20);
                ImGui::Dummy(ImVec2(h1_width, 20));
                fillItemRect(ImColor(128, 128, 255));
                ImGui::Spring(1, 0);
                ImGui::EndVertical();
            ImGui::Spring(0, 0);
            //ImGui::Dummy(ImVec2(200, boundsH.y));
            //fillItemBounds(ImColor(255, 128, 255));
            //ImGui::EndHorizontal();
            //ImGui::Spring(1, 0);
            if (!h_first)
            {
                ImGui::Dummy(ImVec2(200, boundsH.y));
                fillItemRect(ImColor(255, 128, 128));
            }
        ImGui::EndHorizontal();
        drawItemRect(borderColor, 2);

        const bool testHorizontal = fmodf(t, 1.0f) > 0.5f;
        if (testHorizontal)
            ImGui::SameLine();
        ImGui::Dummy(ImVec2(40, 40));
        fillItemRect(ImColor(255, 64, 255));

        if (testHorizontal)
            ImGui::Dummy(ImVec2(40, 40));

        */

        //ImGui::EndGroup();

        ImGui::NewLine();
# endif

        // Vertical
        //drawRect(boundsV.x, 400, borderColor, 2);
        //ImGui::BeginGroup();
        //ImGui::Dummy(ImVec2(boundsV.x, 100));
        //fillItemRect(ImColor(255, 128, 128));
        //ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
        ////ImGui::Spacing();
        ////ImGui::SameColumn();
        //ImGui::Dummy(ImVec2(boundsV.x, 300));
        //fillItemRect(ImColor(128, 255, 128));
        //ImGui::EndGroup();

        ///*
# if defined(IMGUI_EXPERIMENTAL)
        ImGui::SameLine();
        drawRect(boundsV.x, 400, borderColor, 2);
        ImGui::BeginVertical("v1", boundsH);
        if (v_first)
        {
            ImGui::Dummy(ImVec2(boundsV.x, 100));
            fillItemRect(ImColor(255, 128, 128));
        }
        ImGui::Spring(0, 0);
            ImGui::BeginHorizontal("h1");
            ImGui::Dummy(ImVec2(40, v0_width));
            fillItemRect(ImColor(128, 255, 128));
            ImGui::Spring(0, 20);
            ImGui::Dummy(ImVec2(20, v1_width));
            fillItemRect(ImColor(128, 128, 255));
            ImGui::Spring(1, 0);
            ImGui::EndHorizontal();
            ImGui::Spring(0, 0);

        if (!v_first)
        {
            ImGui::Dummy(ImVec2(boundsV.x, 100));
            fillItemRect(ImColor(255, 128, 128));
        }
        //ImGui::Dummy(ImVec2(boundsV.x, 300));
        //fillItemBounds(ImColor(128, 255, 128));
        ImGui::EndVertical();
        drawItemRect(borderColor, 2);

        if (testHorizontal)
            ImGui::SameLine();
        ImGui::Dummy(ImVec2(40,40));
        fillItemRect(ImColor(255, 64, 255));

        if (testHorizontal)
            ImGui::Dummy(ImVec2(40, 40));

# if 0
        ImGui::SetCursorPos(ImVec2(350, 450));

        ImGui::BeginVertical("node");
            ImGui::BeginHorizontal("header");
                ImGui::TextUnformatted("Do N");
                ImGui::Spring();
                ImGui::Button("X");
            ImGui::EndHorizontal();
            drawItemRect(borderColor, 2);
            ImGui::Spring(0,0);

            ImGui::BeginHorizontal("content");
                ImGui::BeginVertical("inputs");
                    ImGui::Button("X"); ImGui::SameLine(); ImGui::TextUnformatted("Enter");
                    ImGui::Spring(0);
                    ImGui::Button("X"); ImGui::SameLine(); ImGui::TextUnformatted("N");
                    ImGui::Spring(0);
                    ImGui::Button("X"); ImGui::SameLine(); ImGui::TextUnformatted("Reset");
                    ImGui::Spring(1);
                ImGui::EndVertical();
                fillItemRect(ImColor(0,255,0,64));
                //fillBounds(ImColor(1.0f, 1.0f, 1.0f, 0.2f));
                //showBounds(ImColor(255, 0, 0));
                ImGui::Spring();
                ImGui::BeginVertical("outputs");
                    ImGui::TextUnformatted("Exit");    ImGui::SameLine(); ImGui::Button("X");
                    ImGui::Spring(0);
                    ImGui::TextUnformatted("Counter"); ImGui::SameLine(); ImGui::Button("X");
                    ImGui::Spring(1);
                    ImGui::Button("+ Add Output");
                ImGui::EndVertical();
                fillItemRect(ImColor(0, 255, 0, 64));
                //fillBounds(ImColor(1.0f, 1.0f, 1.0f, 0.2f));
                //showBounds(ImColor(255, 0, 0));
            ImGui::EndHorizontal();

        ImGui::EndVertical();
        drawItemRect(borderColor, 2);
# endif



# endif

# if 0 // hit test
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





        ImGui::SameLine();

        static ImVec2 emptyRect;
        emptyRect = ImGui::GetCursorScreenPos();
        ImGui::SetWindowHitTest("Debug##Default", [](const ImVec2& point, const ImVec2& min, const ImVec2& max, void*)
        {
            if (point.x >= emptyRect.x && point.y >= emptyRect.y && point.x <= (emptyRect.x + 150.0f) && point.y <= (emptyRect.y + 150.0f))
                return false;

            return true;
        });

        ImGui::GetWindowDrawList()->AddRect(
            ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2(150, 150), IM_COL32(255, 0, 0, 255));

        ImGui::GetWindowDrawList()->AddText(
            ImGui::GetCursorScreenPos() + ImVec2(0, 150 + ImGui::GetStyle().ItemSpacing.y),
            IM_COL32_WHITE, "This is a hole in window.");
# endif

        //ImGui::SetWindowHitTest("Debug", nullptr);


        ImGui::ShowTestWindow();

# if 0 // transformation
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
# endif

        // Rendering
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
                                     //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(_T("ImGui Example"), wc.hInstance);

    settings.Save("example.json");

    return 0;
}
