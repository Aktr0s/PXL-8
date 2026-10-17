/*******************************************************************************************
*
*   PXL8 v1.0.0 - no
*
*   MODULE USAGE:
*       #define GUI_PXL8_LAYOUT_IMPLEMENTATION
*       #include "gui_PXL8_Layout.h"
*
*       INIT: GuiPXL8State state = InitGuiPXL8();
*       DRAW: GuiPXL8(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 Aktr0s. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/


#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string>
#include <vector>

#pragma once

#ifndef GUI_PXL8_LAYOUT_H
#define GUI_PXL8_LAYOUT_H

#include "VM.hpp"


struct GuiPXL8State{
    // Define anchors
    Vector2 anchor01;            // ANCHOR ID:1
    
    // Define controls variables
    Rectangle CodeScrollPanelScrollView;
    Vector2 CodeScrollPanelScrollOffset;
    Vector2 CodeScrollPanelBoundsOffset;    // ScrollPanel: CodeScrollPanel
    bool SPFSpinnerEditMode;
    int SPFSpinnerValue;
    std::vector<std::string> codeLines;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    int highlightLine;          // Line currently highlighted (-1 = none)
    bool scrollToHighlight;     // Set when highlightLine changes, consumed on next draw
    bool loadFileRequested;
    bool fileReloadRequested;
    bool helpWindowRequested;
    char fileLocation[512] = {"/"};

};

GuiPXL8State InitGuiPXL8(void); //


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void GuiPXL8(GuiPXL8State *state);
void GuiPXL8SetHighlightLine(GuiPXL8State *state, int line);    // Highlight a line (-1 = none) and scroll to it
static void StartButton();                // Button: StartButton logic
static void StopButton();                // Button: StopButton logic
static void StepButton();                // Button: StepButton logic
static void ResetButton();                // Button: ResetButton logic
static void LoadFileButton(GuiPXL8State *state);// Button: LoadFileButton logic
static void ReloadFileButton(GuiPXL8State *state);// Button: ReloadFileButton logic
static void HelpButton(GuiPXL8State *state);                   // Button: HelpButton logic

#ifdef __cplusplus
}
#endif

#endif // GUI_PXL8_LAYOUT_H

/***********************************************************************************
*
*   GUI_PXL8_LAYOUT IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_PXL8_LAYOUT_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiPXL8State InitGuiPXL8(void)
{
    GuiPXL8State state = { 0 };

    // Init anchors
    state.anchor01 = (Vector2){ 0, 0 };            // ANCHOR ID:1

    // Initilize controls variables
    state.CodeScrollPanelScrollView = (Rectangle){ 0, 0, 0, 0 };
    state.CodeScrollPanelScrollOffset = (Vector2){ 0, 0 };
    state.CodeScrollPanelBoundsOffset = (Vector2){ 0, 0 };            // ScrollPanel: CodeScrollPanel
    state.SPFSpinnerEditMode = false;
    state.SPFSpinnerValue = 100;
    // Custom variables initialization
    state.highlightLine = -1;
    state.scrollToHighlight = false;
    state.loadFileRequested = false;
    state.fileReloadRequested = false;

    return state;
}

// Highlights a line and requests a scroll so it becomes visible.
// Only flags a scroll when the line actually changes, so the user can still
// scroll manually between steps without being snapped back.
void GuiPXL8SetHighlightLine(GuiPXL8State *state, int line)
{
    if (line != state->highlightLine)
    {
        state->highlightLine = line;
        state->scrollToHighlight = (line >= 0);
    }
}

bool blinkFunc(bool enabled) // Helper function (piece of shit)
{
    static float timer = 0.0f;
    static bool blink = false;

    if (!enabled) {
        timer = 0.0f;
        blink = false;
        return false;
    }

    timer += GetFrameTime();

    if (timer >= 0.5f) {
        timer -= 0.5f;
        blink = !blink;
    }

    return !blink;
}


// Button: StartButton logic
static void StartButton() // Unused, added calls directly for cleaner code
{
    AssembleRunButtonPass();
}
// Button: StopButton logic
static void StopButton()
{
    StopProgram();
}
// Button: StepButton logic
static void StepButton()
{
    StepProgram();
}
// Button: ResetButton logic
static void ResetButton()
{
    ResetProgram();
}
// Button: LoadFileButton logic
static void LoadFileButton(GuiPXL8State *state)
{
    state->loadFileRequested = true;
}
// Button: ReloadFileButton logic
static void ReloadFileButton(GuiPXL8State *state)
{
    state->fileReloadRequested = true;
}
// Button: HelpButton logic
static void HelpButton(GuiPXL8State *state)
{
    state->helpWindowRequested = true;
}



#define CODE_LINE_HEIGHT 30
#define CODE_TEXT_SIZE   30

#define CODE_HIGHLIGHT_COLOR  ((Color){ 60, 120, 220, 160 })

void GuiPXL8(GuiPXL8State *state)
{
    int oldBg = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
    int oldSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    int oldTextColor = GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL);

    GuiPanel((Rectangle){ state->anchor01.x + 0, state->anchor01.y + 0, 1280, 720 }, NULL);
    GuiGroupBox((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 128, 415, 420 }, "Graphical Sandbox");
    
     if (running && !AssembleRequired){
        GuiLock();
        if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 88, 120, 24 }, GuiIconText(ICON_LOCK_CLOSE, "Run"))) StartButton();
    } else if (!running && AssembleRequired){
        if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 88, 120, 24 }, GuiIconText(ICON_FILETYPE_BINARY, "Assemble"))) StartButton();
    } else if (!running && !AssembleRequired){
        if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 88, 120, 24 }, GuiIconText(ICON_ARROW_RIGHT_FILL, "Run"))) StartButton();
    } else {
        // This should never be reached!
    }
    GuiUnlock();


    if (!running) GuiLock();
    if (GuiButton((Rectangle){ state->anchor01.x + 984, state->anchor01.y + 88, 64, 24 }, "Stop")) StopButton();
    GuiUnlock();
    
    if (running) GuiLock();
    if (GuiButton((Rectangle){ state->anchor01.x + 1056, state->anchor01.y + 88, 56, 24 }, "Step")) StepButton();
    GuiUnlock();

    GuiGroupBox((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 560, 400, 104 }, "Registers");
    GuiLabel((Rectangle){ state->anchor01.x + 864, state->anchor01.y + 568, 25, 24 }, "R0: ");
    GuiLabel((Rectangle){ state->anchor01.x + 992, state->anchor01.y + 568, 25, 24 }, "R1: ");
    GuiLabel((Rectangle){ state->anchor01.x + 1120, state->anchor01.y + 568, 25, 24 }, "R2: ");
    GuiLabel((Rectangle){ state->anchor01.x + 864, state->anchor01.y + 600, 25, 24 }, "R3: ");
    GuiLabel((Rectangle){ state->anchor01.x + 992, state->anchor01.y + 600, 25, 24 }, "R4: ");
    GuiLabel((Rectangle){ state->anchor01.x + 1120, state->anchor01.y + 600, 25, 24 }, "R5: ");
    GuiLabel((Rectangle){ state->anchor01.x + 864, state->anchor01.y + 632, 25, 24 }, "R6: ");
    GuiLabel((Rectangle){ state->anchor01.x + 992, state->anchor01.y + 632, 25, 24 }, "R7: ");
    if (GuiButton((Rectangle){ state->anchor01.x + 1120, state->anchor01.y + 88, 64, 24 }, "Reset")) ResetButton();
    GuiLabel((Rectangle){ state->anchor01.x + 1120, state->anchor01.y + 632, 25, 24 }, "PC: ");
    if (running) {
        GuiLock();
        if (GuiButton((Rectangle){ state->anchor01.x + 16, state->anchor01.y + 16, 120, 24 }, GuiIconText(ICON_LOCK_CLOSE, "Load File"))) LoadFileButton(state);
        if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 56, 120, 24 }, GuiIconText(ICON_LOCK_CLOSE, "Reload File"))) ReloadFileButton(state);
    } else {
        if (GuiButton((Rectangle){ state->anchor01.x + 16, state->anchor01.y + 16, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Load File"))) LoadFileButton(state);
        if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 56, 120, 24 }, GuiIconText(ICON_RESTART, "Reload File"))) ReloadFileButton(state);
    }

    GuiUnlock();
    if (GuiButton((Rectangle){ state->anchor01.x + 856, state->anchor01.y + 16, 24, 24 }, GuiIconText(ICON_HELP,""))) HelpButton(state);




    GuiSetStyle(DEFAULT, TEXT_SIZE, CODE_TEXT_SIZE);

    if (blinkFunc(running)) {
        GuiSetStyle(DEFAULT,TEXT_COLOR_NORMAL, ColorToInt(CODE_HIGHLIGHT_COLOR));
        GuiLabel((Rectangle){ state->anchor01.x + 980, state->anchor01.y + 16, 300, 24 }, GuiIconText(ICON_ROM,"EXECUTING..."));
    }

    GuiSetStyle(DEFAULT, BACKGROUND_COLOR,GuiGetStyle(DEFAULT, LINE_COLOR));
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, oldBg);


    Rectangle codeBounds = {
        state->anchor01.x + 16, state->anchor01.y + 64,
        824 - state->CodeScrollPanelBoundsOffset.x,
        632 - state->CodeScrollPanelBoundsOffset.y
    };

    // Content size drives the scrollbar range. Width is a bit smaller than the panel
    // so no horizontal scrollbar shows up; height grows with the number of lines.
    int border = GuiGetStyle(DEFAULT, BORDER_WIDTH);
    int scrollBarW = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    float innerW = codeBounds.width - 2*border;
    float innerH = codeBounds.height - 2*border;

    float contentH = state->codeLines.size()*CODE_LINE_HEIGHT;
    bool needsVScroll = (contentH > innerH);
    if (contentH < innerH) contentH = innerH;                 // fill the panel when the file is short

    // Leave room for the vertical scrollbar (if shown) so no horizontal scrollbar appears
    Rectangle codeContent = { 0, 0, innerW - (needsVScroll ? scrollBarW : 0), contentH };

    // Scroll so the highlighted line is visible (only when the highlight changed).
    // Scroll offset is negative when scrolled down; GuiScrollPanel clamps it to valid bounds.
    if (state->scrollToHighlight && (state->highlightLine >= 0))
    {
        // View height is 0 on the very first frame, so fall back to the inner bounds
        float viewH = (state->CodeScrollPanelScrollView.height > 0)
                    ? state->CodeScrollPanelScrollView.height
                    : innerH;

        float lineTop    = state->highlightLine*CODE_LINE_HEIGHT;
        float lineBottom = lineTop + CODE_LINE_HEIGHT;
        float scrollY    = -state->CodeScrollPanelScrollOffset.y;

        if ((lineTop < scrollY) || (lineBottom > scrollY + viewH))   // out of view
        {
            // Put the middle of the line at the middle of the view
            scrollY = lineTop + CODE_LINE_HEIGHT/2.0f - viewH/2.0f;

            // Can't scroll before the start or past the end ("if possible")
            float maxScroll = contentH - viewH;
            if (maxScroll < 0) maxScroll = 0;
            if (scrollY > maxScroll) scrollY = maxScroll;
            if (scrollY < 0) scrollY = 0;

            state->CodeScrollPanelScrollOffset.y = -scrollY;
        }

        state->scrollToHighlight = false;
    }

    GuiScrollPanel(codeBounds, NULL, codeContent, &state->CodeScrollPanelScrollOffset, &state->CodeScrollPanelScrollView);
    Rectangle view = state->CodeScrollPanelScrollView;
    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    float ox = codeBounds.x + state->CodeScrollPanelScrollOffset.x;
    float oy = codeBounds.y + state->CodeScrollPanelScrollOffset.y;

    // Only draw the lines that are actually visible
    int first = (int)(-state->CodeScrollPanelScrollOffset.y/CODE_LINE_HEIGHT);
    if (first < 0) first = 0;
    int last = first + (int)(view.height/CODE_LINE_HEIGHT) + 2;
    if (last > state->codeLines.size()) last = state->codeLines.size();

    for (int i = first; i < last; i++)
    {
        float lineY = oy + i*CODE_LINE_HEIGHT;

        // Highlight goes first so the text is drawn on top of it
        if (i == state->highlightLine)
        {
            DrawRectangle((int)view.x, (int)lineY, (int)view.width, CODE_LINE_HEIGHT, CODE_HIGHLIGHT_COLOR);
        }

        GuiLabel((Rectangle){ ox + 8, lineY, 800, CODE_LINE_HEIGHT },
                 state->codeLines[i].c_str());
    }

    EndScissorMode();
    GuiSetStyle(DEFAULT, TEXT_SIZE, oldSize);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, oldBg);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, GuiGetStyle(DEFAULT, LINE_COLOR));

    GuiGroupBox((Rectangle){ state->anchor01.x + 16, state->anchor01.y + 53, 824, 643 }, "Assembly");
    GuiSetStyle(DEFAULT,TEXT_COLOR_NORMAL, ColorToInt(CODE_HIGHLIGHT_COLOR));
    GuiLabel((Rectangle){ state->anchor01.x + 896, state->anchor01.y + 568, 80, 24 }, TextFormat("%d", vm.regs[0]));
    GuiLabel((Rectangle){ state->anchor01.x + 1024, state->anchor01.y + 568, 80, 24 }, TextFormat("%d", vm.regs[1]));
    GuiLabel((Rectangle){ state->anchor01.x + 1152, state->anchor01.y + 568, 80, 24 }, TextFormat("%d", vm.regs[2]));
    GuiLabel((Rectangle){ state->anchor01.x + 896, state->anchor01.y + 600, 80, 24 }, TextFormat("%d", vm.regs[3]));
    GuiLabel((Rectangle){ state->anchor01.x + 1024, state->anchor01.y + 600, 80, 24 }, TextFormat("%d", vm.regs[4]));
    GuiLabel((Rectangle){ state->anchor01.x + 1152, state->anchor01.y + 600, 80, 24 }, TextFormat("%d", vm.regs[5]));
    GuiLabel((Rectangle){ state->anchor01.x + 896, state->anchor01.y + 632, 80, 24 }, TextFormat("%d", vm.regs[6]));
    GuiLabel((Rectangle){ state->anchor01.x + 1024, state->anchor01.y + 632, 80, 24 }, TextFormat("%d", vm.regs[7]));
    GuiLabel((Rectangle){ state->anchor01.x + 1152, state->anchor01.y + 632, 80, 24 }, TextFormat("0x%02X", vm.pc));
    GuiStatusBar((Rectangle){ state->anchor01.x + 160, state->anchor01.y + 16, 680, 24 }, TextFormat("Path: %s", state->fileLocation));

    int before = state->SPFSpinnerValue;
    if (GuiSpinner((Rectangle){ state->anchor01.x + 1064, state->anchor01.y + 56, 120, 24 }, "Steps/Second ", &state->SPFSpinnerValue, 0, 1000000, state->SPFSpinnerEditMode)) state->SPFSpinnerEditMode = !state->SPFSpinnerEditMode;
    int delta = state->SPFSpinnerValue - before;
    if (delta == 1) state->SPFSpinnerValue = before + 5;
    if (delta == -1) state->SPFSpinnerValue = before - 5;
    if (state->SPFSpinnerValue > 1000000) {state->SPFSpinnerValue = 1000000;}
    if (state->SPFSpinnerValue < 0) {state->SPFSpinnerValue = 0;}
    GuiSetStyle(DEFAULT,TEXT_COLOR_NORMAL, oldTextColor);

}

#endif // GUI_PXL8_LAYOUT_IMPLEMENTATION