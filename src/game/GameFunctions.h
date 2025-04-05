#pragma once

#include "core/MonoRuntime.h"



class GameFunctions {
private:
    MonoRuntime* m_runtime;
    MonoClass* m_localUserManagerClass;
    MonoClass* m_localUserClass;
    MonoClass* m_characterMasterClass;
    MonoClass* m_characterBodyClass;

public:
    GameFunctions(MonoRuntime* runtime);
    ~GameFunctions() = default;

    void CharacterMaster_UpdateBodyGodMode(void* m_characterMaster);
};