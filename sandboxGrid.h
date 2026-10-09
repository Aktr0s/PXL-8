//
// Created by Aktr0s on 21.09.2026.
//

#ifndef PXL_8_SANDBOXGRID_H
#define PXL_8_SANDBOXGRID_H
#include "stdint.h"

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

constexpr int GRID = 100;
extern Color SandboxPlane[GRID][GRID];

void drawSandboxGrid();
void testPatternOverride(); // Testing pattern, currently ununsed
void fillSandboxGrid(Color selectedColor);
void initSandboxGrid(); //No need for this function just so i can fill it in black at startt
void syncSandboxGridFromVM(const uint8_t* vmFramebuffer);

#ifdef __cplusplus
}
#endif

#endif //PXL_8_SANDBOXGRID_H
