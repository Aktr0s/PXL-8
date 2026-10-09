//
// Created by Aktr0s on 23.09.2026.
//
#include "sandboxGrid.h"

Color SandboxPlane[GRID][GRID];

static const int xoffset = 863;
static const int yoffset = 140;
static const double SCALE = 4; // each "pixel" drawn as a 4x4 square

static Color palette[] = {
    {0, 0, 0, 255}, // black
    {29, 43, 83, 255}, // dark blue
    {126, 37, 83, 255}, // purple
    {0, 135, 81, 255}, // green
    {171, 82, 54, 255}, // brown
    {95, 87, 79, 255}, // gray
    {194, 195, 199, 255}, // light gray
    {255, 241, 232, 255}, // white
    {255, 0, 77, 255}, // red
    {255, 163, 0, 255}, // orange
    {255, 236, 39, 255}, // yellow
    {0, 228, 54, 255}, // green
    {41, 173, 255, 255}, // blue
    {131, 118, 156, 255}, // purple-gray
    {255, 119, 168, 255}, // pink
    {255, 204, 170, 255}, // peach
};

void drawSandboxGrid()
{
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            DrawRectangle(i * SCALE + xoffset, j * SCALE + yoffset, SCALE, SCALE, SandboxPlane[i][j]);
        }
    }
}

void testPatternOverride()
{
    constexpr int PALETTE_SIZE = sizeof(palette) / sizeof(palette[0]);
    for (int y = 0; y < GRID; y++) {
        for (int x = 0; x < GRID; x++) {
            SandboxPlane[x][y] = palette[x % PALETTE_SIZE];
        }
    }
}

void fillSandboxGrid(Color selectedColor)
{
    for (int y = 0; y < GRID; y++) {
        for (int x = 0; x < GRID; x++) {
            SandboxPlane[x][y] = selectedColor;
        }
    }
}

void initSandboxGrid() //No need for this function just so i can fill it in black at the start
{
    fillSandboxGrid(BLACK);
}

void syncSandboxGridFromVM(const uint8_t* vmFramebuffer)
{
    constexpr int PALETTE_SIZE = sizeof(palette) / sizeof(palette[0]);
    for (int y = 0; y < GRID; y++) {
        for (int x = 0; x < GRID; x++) {
            uint8_t idx = vmFramebuffer[y * GRID + x]; // VM stores row-major: y*width + x
            SandboxPlane[x][y] = palette[idx % PALETTE_SIZE];
        }
    }
}
