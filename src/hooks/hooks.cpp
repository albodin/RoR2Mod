#include "globals/globals.h"
#include "hooks.h"
#include "kiero.h"
#include "imgui/imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <map>
#include "minhook/include/MinHook.h"
#include "menu/menu.h"
#include "game/GameStructs.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 

std::map<std::string, LPVOID> hooks;
std::map<std::string, LPVOID> hookTargets;

bool CreateHook(
    const char* assemblyName,
    const char* nameSpace,
    const char* className,
    const char* methodName,
    int paramCount,
    const char* returnType,
    const char** paramTypes,
    LPVOID pDetour,
    LPVOID* ppOriginal
) {
    std::string hookName = std::string(nameSpace) + std::string(className) + std::string(methodName);
    bool hook_success = false;
    
    while (true) {
        LPVOID pTarget = G::g_monoRuntime->GetMethodAddress(
            assemblyName, nameSpace, className, methodName, paramCount, returnType, paramTypes);
        
        if (!pTarget) {
            G::logger.LogError("Failed to get method address for " + std::string(assemblyName) + "::" +
                               std::string(nameSpace) + "::" + std::string(className) + "::" + std::string(methodName));
            return false;
        }

        // Try to create the hook
        MH_STATUS create_status = MH_CreateHook(pTarget, pDetour, ppOriginal);
        if (create_status != MH_OK) {
            continue;
        }

        hooks[hookName] = *ppOriginal;
        hookTargets[hookName] = pTarget;
        hook_success = true;
        break;
    }
    
    if (!hook_success) {
        G::logger.LogError("Hook creation failed for " + std::string(assemblyName) + "::" +
                           std::string(nameSpace) + "::" + std::string(className) + "::" + std::string(methodName));
        // G::running = false;
        return false;
    }
    
    return true;
}

#define STRINGIFY(x) #x
#define HOOK(assembly, ns, class, method, paramcount, returntype, ...) \
{ \
    const char* paramTypes[] = __VA_ARGS__; \
    void (*fp ## ns ## class ## method)(void); \
    if (CreateHook(#assembly, #ns, #class, #method, paramcount, returntype, paramTypes, \
               reinterpret_cast<LPVOID>(&Hooks::hk ## ns ## class ## method), \
               reinterpret_cast<LPVOID*>(&fp ## ns ## class ## method))) { \
        G::logger.LogInfo(STRINGIFY(Hooked ns##class##method)); \
    } \
    else { \
        G::logger.LogError(STRINGIFY(Failed to hook ns##class##method)); \
    } \
}


//#define UNHOOK(name) (MH_DisableHook(#name))

void Hooks::Init() {
    bool hooked = false;
    while (!hooked)
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&G::oPresent, (void*)hkPresent11);
            hooked = true;
        }
    }

    G::g_monoRuntime = new MonoRuntime();
    while (!G::g_monoRuntime->Initialize())
    {
        Sleep(100);
    }

    
    HOOK(Rewired_Core, Rewired, Player, GetButtonDown, 1, "System.Boolean", {"System.Int32"});
    HOOK(RoR2, RoR2, LocalUser, RebuildControlChain, 0, "System.Void", {});

    for (auto& target: hookTargets) {
        MH_STATUS enable_status = MH_EnableHook(target.second);
        if (enable_status == MH_OK) {
            G::logger.LogInfo("Hook successfully enabled for " + target.first);
        } else {
            G::logger.LogError("Hook enabling failed for " + target.first + ", error: " + 
                             MH_StatusToString(enable_status) + " (code: " + MH_StatusToString(enable_status) + ")");
        }
    }

    G::gameFunctions = new GameFunctions(G::g_monoRuntime);
}

void Hooks::Unhook() {
    G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (LONG_PTR)G::oWndProc);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    for (auto& target: hookTargets) {
        MH_STATUS disable_status = MH_DisableHook(target.second);
        if (disable_status == MH_OK) {
            G::logger.LogInfo("Hook successfully disabled for " + target.first);
        } else {
            G::logger.LogError("Hook disabling failed for " + target.first + ", error: " + 
                             MH_StatusToString(disable_status) + " (code: " + MH_StatusToString(disable_status) + ")");
        }
    }

    kiero::shutdown();
    delete G::g_monoRuntime;
    delete G::gameFunctions;
}

bool Hooks::hkRewiredPlayerGetButtonDown(void* instance, int key) {
    if (G::showMenu)
        return false;
    
    static auto originalFunc = reinterpret_cast<bool(*)(void*, int)>(hooks["RewiredPlayerGetButtonDown"]);
    return originalFunc(instance, key);
}

void Hooks::hkRoR2LocalUserRebuildControlChain(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2LocalUserRebuildControlChain"]);
    originalFunc(instance);

    LocalUser* localUser_ptr = (LocalUser*)instance;
    if (!localUser_ptr->cachedMaster_backing || !localUser_ptr->cachedBody_backing || !localUser_ptr->cachedBody_backing->healthComponent_backing) {
        return;
    }
    localUser_ptr->cachedMaster_backing->godMode = G::godMode;
    localUser_ptr->cachedBody_backing->healthComponent_backing->godMode_backing = G::godMode;

    localUser_ptr->cachedBody_backing->baseMoveSpeed = G::baseMoveSpeed;
    localUser_ptr->cachedBody_backing->baseDamage = G::baseDamage;
    localUser_ptr->cachedBody_backing->baseAttackSpeed = G::baseAttackSpeed;
    localUser_ptr->cachedBody_backing->baseCrit = G::baseCrit;
    localUser_ptr->cachedBody_backing->baseJumpCount = G::baseJumpCount;

    //const int items_count = 122;
    //for (int i = 0; i < items_count; i++) {
    //    localUser_ptr->cachedBody_backing->inventory_backing->itemStacks[i] = 100;
    //}
}


LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (G::showMenu)
        return TRUE;
    
    return CallWindowProc(G::oWndProc, hWnd, uMsg, wParam, lParam);
}

long __stdcall Hooks::hkPresent11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!G::initialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&G::pDevice))) {
            ImGui::CreateContext();

            G::pDevice->GetImmediateContext(&G::pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            G::windowHwnd = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = NULL;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            G::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &G::mainRenderTargetView);
            pBackBuffer->Release();

            ImGui_ImplWin32_Init(G::windowHwnd);
            ImGui_ImplDX11_Init(G::pDevice, G::pContext);
            ImGui_ImplDX11_CreateDeviceObjects();

            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            
            G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
            G::initialized = true;    
        }
        else {
            return G::oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = G::showMenu;

    if (ImGui::IsKeyPressed(ImGuiKey_Insert)) {
        G::showMenu = !G::showMenu;
    }

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos;
        window_pos.x = work_pos.x + PAD;
        window_pos.y = work_pos.y + PAD;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
        window_flags |= ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("Watermark", 0, window_flags);
        ImGui::Text("RoR2Mod V1.0");
        ImGui::End();

        if (G::showMenu)
            DrawMenu();
    }

    ImGui::Render();

    G::pContext->OMSetRenderTargets(1, &G::mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return G::oPresent(pSwapChain, SyncInterval, Flags);
}