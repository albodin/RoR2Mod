#include "GameFunctions.h"
#include "globals/globals.h"

GameFunctions::GameFunctions(MonoRuntime* runtime) {
    m_runtime = runtime;
    m_localUserManagerClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUserManager");
    m_localUserClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUser");
    m_characterMasterClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterMaster");
    m_characterBodyClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterBody");
}

void GameFunctions::CharacterMaster_UpdateBodyGodMode(void* m_characterMaster) {
    if (!m_characterMasterClass) return;

    MonoMethod* method = m_runtime->GetMethod(m_characterMasterClass, "UpdateBodyGodMode", 0);
    if (!method) {
        G::logger.LogError("Failed to find UpdateBodyGodMode method");
        return;
    }

    m_runtime->InvokeMethod(method, m_characterMaster, nullptr);
}