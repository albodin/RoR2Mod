#pragma once
#include <string>

class ModuleBase {
  public:
    ModuleBase() {};
    virtual ~ModuleBase() {};

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void DrawUI() = 0;
};
