#include "GameFunctions.h"
#include "globals/globals.h"

GameFunctions::GameFunctions(MonoRuntime* runtime) {
    m_runtime = runtime;
    m_localUserManagerClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUserManager");
    m_localUserClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUser");
    m_characterMasterClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterMaster");
    m_characterBodyClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterBody");
    m_cursorClass = runtime->GetClass("UnityEngine.CoreModule", "UnityEngine", "Cursor");
}


void GameFunctions::CharacterMaster_UpdateBodyGodMode(void* m_characterMaster) {
    std::function<void()> task = [this, m_characterMaster]() {
        if (!m_characterMasterClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_characterMasterClass, "UpdateBodyGodMode", 0);
        if (!method) {
            G::logger.LogError("Failed to find UpdateBodyGodMode method");
            return;
        }

        m_runtime->InvokeMethod(method, m_characterMaster, nullptr);
    };
    G::queuedActions.push(task);
}

void GameFunctions::Cursor_SetLockState(int lockState) {
    std::function<void()> task = [this, lockState]() {
        if (!m_cursorClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_cursorClass, "set_lockState", 1);
        if (!method) {
            G::logger.LogError("Failed to find set_lockState method");
            return;
        }

        int localLockState = lockState;
        void* args[1];
        args[0] = &localLockState;

        m_runtime->InvokeMethod(method, nullptr, args);
    };
    G::queuedActions.push(task);
}

void GameFunctions::Cursor_SetVisible(bool visible) {
    std::function<void()> task = [this, visible]() {
        if (!m_cursorClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_cursorClass, "set_visible", 1);
        if (!method) {
            G::logger.LogError("Failed to find set_visible method");
            return;
        }

        bool localVisible = visible;
        void* args[1];
        args[0] = &localVisible;

        m_runtime->InvokeMethod(method, nullptr, args);
    };
    G::queuedActions.push(task);
}