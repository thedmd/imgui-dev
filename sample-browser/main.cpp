// ImGui - standalone example application for DirectX 11
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#undef IMGUI_EXPERIMENTAL

#include <imgui.h>
#include "imgui_impl_dx11.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
struct IUnknown; // needed if compiled with /permissive-
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <string>
#include "support/settings.h"
#include "support/property.h"
#include "samples/samples.h"

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

    auto samples = CreateSamples(settings);

    auto configCurrentSample = MakeProperty<std::string>("Main.CurrentSample", "", settings);
    Sample* currentSample = nullptr;

    auto findSample = [&samples](std::string_view name) -> Sample*
    {
        for (auto& sample : samples)
        {
            if (sample->Name() == name)
                return sample.get();
        }

        return nullptr;
    };

    auto setCurrentSample = [&configCurrentSample, &currentSample](Sample* sample)
    {
        if (currentSample)
        {
            currentSample->Finalize();
            configCurrentSample.Value.clear();
        }

        currentSample = sample;

        if (currentSample)
        {
            configCurrentSample.Value = currentSample->Name();
            currentSample->Initialize();
        }
    };

    if (auto lastSample = findSample(configCurrentSample.Value))
        setCurrentSample(lastSample);
    else if (!samples.empty())
        setCurrentSample(samples[0].get());

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


    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->AddFontFromFileTTF(R"(c:\Windows\Fonts\arial.ttf)", 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->Build();
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();

//     bool show_test_window = true;
//     bool show_another_window = false;
    //ImVec4 clear_color = ImVec4(0.45f * 0.5f, 0.55f * 0.5f, 0.60f * 0.5f, 1.00f);
    ImVec4 clear_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

//     auto& style = ImGui::GetStyle();
//     style.Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255);
//     style.ItemSpacing = ImVec2(10, 10);
//     style.Colors[ImGuiCol_Text] = ImColor(0, 0, 0);
//     style.Colors[ImGuiCol_CheckMark] = ImColor(110, 110, 110);

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

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
        auto windowFlags =
             ImGuiWindowFlags_NoMove
            |ImGuiWindowFlags_NoResize
            |ImGuiWindowFlags_NoTitleBar
            |ImGuiWindowFlags_NoSavedSettings
            |ImGuiWindowFlags_NoFocusOnAppearing
            |ImGuiWindowFlags_NoBringToFrontOnFocus
            |ImGuiWindowFlags_AlwaysAutoResize
            |ImGuiWindowFlags_NoNav;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        auto showWindows = ImGui::Begin("Sample View", nullptr, windowFlags);
        ImGui::PopStyleVar();
        if (showWindows)
        {
            //auto drawList = ImGui::GetWindowDrawList();
            //drawList->PushClipRectFullScreen();
            //drawList->AddRectFilled(ImVec2(0.0f, 0.0f), io.DisplaySize, IM_COL32(0, 255, 0, 128));
            //drawList->PopClipRect();

            //drawList->AddRectFilled(ImVec2(0.0f, 0.0f), ImGui::GetIO().DisplaySize, IM_COL32(255, 0, 0, 255));

            //drawList->AddRectFilled(
            //    ImGui::GetCursorScreenPos(),
            //    ImGui::GetWindowContentRegionMax(),
            //    IM_COL32(0, 0, 128, 255));

            //drawList->AddRectFilled(
            //    ImGui::GetCursorScreenPos(),
            //    ImGui::GetContentRegionAvail(),
            //    IM_COL32(0, 128, 128, 255));

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Sample:");
            ImGui::SameLine();

            if (ImGui::BeginCombo("##Sample", currentSample ? currentSample->Name() : ""))
            {
                for (auto& sample : samples)
                {
                    auto isCurrentSample = sample.get() == currentSample;
                    if (ImGui::Selectable(sample->Name(), isCurrentSample))
                        setCurrentSample(sample.get());
                    if (isCurrentSample)
                    if (isCurrentSample)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            if (currentSample)
                currentSample->Frame();

            ImGui::End();
        }

        ///*
# if defined(IMGUI_EXPERIMENTAL)

        auto borderColor = ImColor(255, 255, 255, 64);

        ImVec2 bounds  = ImVec2(     600,      400);
        ImVec2 boundsH = ImVec2(bounds.x,      340);
        ImVec2 boundsV = ImVec2(      80, bounds.y);

        bool v_first = false;
        auto v0_width = 200.0f;
        auto v1_width = 100.0f;

        auto drawRect = [](float w, float h, ImColor color, float expand = 0.0f)
        {
            ImGui::GetWindowDrawList()->AddRect(
                ImGui::GetCursorScreenPos() - ImVec2(expand, expand),
                ImGui::GetCursorScreenPos() + ImVec2(w + expand, h + expand),
                color);
        };

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

        // Rendering
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
                                     //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    configCurrentSample.Save(settings);

    setCurrentSample(nullptr);

    //for (auto& sample : samples)
    //    sample->Finalize();

    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(_T("ImGui Example"), wc.hInstance);

    settings.Save("example.json");

    return 0;
}

