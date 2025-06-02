# Risk of Rain 2 Mod

## Features

### Player
- Godmode
- Base move speed, damage, attack speed, crit, jump count modification
- Teleport to cursor
- Item spawning
- Huntress tracking range and FOV

### ESP
- Players & Enemies (distance filtering, customizable colors, health bars, distance indicators, visibility options, tracelines)
- Interactables organized by category:
  - Chests (regular, large, legendary, equipment barrels, lunar pods, etc.)
  - Shops & Printers (3D printers, multishop terminals, cauldrons, scrapper)
  - Drones (gunner, healing, missile, incinerator, equipment drones, turrets)
  - Shrines (blood, chance, mountain, woods, order, etc.)
  - Special (newt altars, teleporter, pillars, portals, etc.)

### Other
- Configuration saving/loading
- Custom fonts

## Building

### Method 1: Using the terminal
```bash
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake ..
cmake --build .
```

### Method 2: Using VSCode Tasks
1. In VSCode, press Ctrl+Shift+B to build

or

1. In VSCode, press Ctrl+Shift+P, select "Tasks: Run Task", and select "Build All"

## Running

### Method 1: Use VSCode Tasks
1. In VSCode, press Ctrl+Shift+P, select "Tasks: Run Task", and select "Inject and Run"

### Method 2: Run manually 
```bash
# From the build directory:
protontricks-launch --no-bwrap --appid 632360 ./injector.exe "Risk of Rain 2.exe" "Z:/path/to/RoR2Mod.dll"
```

## Debugging

1. Inject the mod using one of the options above
2. In VSCode run the `Attach to Risk of Rain 2` launch target
3. Select the "Risk of Rain 2.exe" process
4. Pause execution and switch to the debug console
5. Run: `-exec load-dll` to load the debug symbols into GDB
6. You can now set breakpoints and step through the code in VSCode