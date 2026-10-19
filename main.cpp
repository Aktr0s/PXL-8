#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION
#define GUI_PXL8_LAYOUT_IMPLEMENTATION
#define GUI_WINDOW_HELP_IMPLEMENTATION
#include "gui_window_help.h"
#include <string>

#include "fileLoading.hpp"
#include "gui_PXL8_Layout.h"
#include "messageBox.hpp"
#include "style_candy.h"
#include "style_dark.h"
#include "sandboxGrid.h"
#include "VM.hpp"

static void UpdateKeyState()
{
    vm.keyState = 0;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) vm.keyState |= (1 << 0);
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) vm.keyState |= (1 << 1);
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) vm.keyState |= (1 << 2);
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) vm.keyState |= (1 << 3);
    if (IsKeyDown(KEY_SPACE)) vm.keyState |= (1 << 4);
    if (IsKeyDown(KEY_Z))     vm.keyState |= (1 << 5);
    if (IsKeyDown(KEY_X))     vm.keyState |= (1 << 6);
}

int main() {

    InitWindow(1280, 720, "PXL-8: Assembly Graphical Sandbox");
    GuiLoadStyleCandy();
    //GuiLoadStyleDark();
    GuiWindowHelpState helpUI = InitGuiWindowHelp();
    helpUI.windowActive = true;
    GuiPXL8State ui = InitGuiPXL8();
    ui.codeLines = {"To begin, Load a file with PXL-8 Assembly Code (.pxasm)"};
    GuiPXL8SetHighlightLine(&ui, 0);
    initFileDialog();
    SetTargetFPS(100);
    initSandboxGrid();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F1)) {
            if (helpUI.windowActive) helpUI.windowActive = false;
            else helpUI.windowActive = true;
        }
        UpdateKeyState();
        //TickProgram(ui.SPFSpinnerValue);
        TickProgramAtRate(ui.SPFSpinnerValue);
        GuiPXL8SetHighlightLine(&ui, lastExecutedLine > 0 ? lastExecutedLine - 1 : -1);
        syncSandboxGridFromVM(vm.framebuffer);
        BeginDrawing();
        ClearBackground(BLACK);
        GuiPXL8(&ui);
        drawSandboxGrid();
        attemptHelpWindow(&ui, &helpUI);
        attemptFileDialog(&ui);
        attemptMessageBox(&ui);
        //GuiWindowHelp(&helpUI);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
