/*******************************************************************************************
*
*   Window Help
*
*   MODULE USAGE:
*       #define GUI_WINDOW_HELP_IMPLEMENTATION
*       #include "gui_window_help.h"
*
*   On game init call:  GuiWindowHelpState state = InitGuiWindowHelp();
*   On game draw call:  GuiWindowHelp(&state);
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2022-2024 raylib technologies (@raylibtech) / Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "../gui_PXL8_Layout.h"

// WARNING: raygui implementation is expected to be defined before including this header

#ifndef GUI_WINDOW_HELP_H
#define GUI_WINDOW_HELP_H

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Gui window structure declaration
typedef struct {
    bool windowActive;

    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;
    
    int contentHeight;
    Vector2 scrollPanelOffset;

} GuiWindowHelpState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiWindowHelpState InitGuiWindowHelp(void);
void GuiWindowHelp(GuiWindowHelpState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_WINDOW_HELP_H

/***********************************************************************************
*
*   GUI_WINDOW_HELP IMPLEMENTATION
*
************************************************************************************/

#if defined(GUI_WINDOW_HELP_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define GUIHELPWINDOW_MAX_LINES            128

#define GUIHELPWINDOW_LINE_HEIGHT           24
#define GUIHELPWINDOW_LINE_EMPTY_HEIGHT     12

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// Tool help info
static const char *helpLines[] = {
    "-About",
    "PXL-8 - Assembly Graphical Sandbox",
    "F1 - Opens this window",
    "ESC - Close window / exit",

    "-Buttons",
    "Load File - Open a .pxasm file",
    "Reload File - Re-read the file from disk",
    "Assemble and Run - Assemble and start running",
    "Stop - Pause execution",
    "Step - Run a single instruction",
    "Reset - Clear registers, memory and screen",
    "Ticks/Second - Speed, changes in steps of 5",
    "Highlighted line - Instruction that just ran",

    "-Syntax",
    "; text - Comment until end of line",
    "name: - Label, target for jumps",
    "OPCODE a, b - Operands separated by commas",
    "Opcodes must be UPPERCASE (MOV, not mov)",
    "Labels are case-sensitive",

    "-Operands",
    "r0 .. r7 - Register (32-bit signed)",
    "#10  #0x1F  #-5 - Immediate value (dec or hex)",
    "[0x80] - Memory byte, address 0x00-0xFF",
    "10 - A bare number counts as #10",

    "-Data",
    "MOV d, s - Copy s into d",
    "LOAD d, [a] - Read memory byte into d",
    "STORE [a], s - Write s to memory (low byte)",
    "NOP - Do nothing",
    "MOV cannot use [mem], use LOAD/STORE",

    "-Math (result goes into first operand)",
    "ADD SUB MUL DIV MOD d, s",
    "AND OR XOR d, s",
    "NOT INC DEC d",
    "DIV / MOD by zero stops with an error",

    "-Compare and Jump",
    "CMP a, b - Computes a-b, sets flags only",
    "JMP label - Always jump",
    "JZ / JNZ label - Jump if zero / not zero",
    "JG / JL label - Jump if greater / less",
    "JGE / JLE label - Greater-equal / less-equal",
    "Flags Z and N are set by math, logic,",
    "INC, DEC and CMP (not by MOV/LOAD/STORE)",

    "-Screen (100 x 100)",
    "PIX x, y, c - Set pixel, off-screen ignored",
    "CLS c - Fill the whole screen with color c",
    "Colors 0-15 (palette index, wraps after 15)",

    "-Input and Control",
    "KEY d - d = bitmask of held keys",
    "Bits: 1 Up/W, 2 Down/S, 4 Left/A, 8 Right/D",
    "16 Space, 32 Z, 64 X",
    "WAIT - End this frame's ticks (frame sync)",
    "HALT - Stop the program",

    "-Example (diagonal line)",
    "  MOV r0, #0",
    "loop:",
    "  PIX r0, r0, #7",
    "  INC r0",
    "  CMP r0, #99",
    "  JLE loop",
    "  HALT",
    NULL
};

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
// Init window Help
GuiWindowHelpState InitGuiWindowHelp(void)
{
    GuiWindowHelpState state = { 0 };

    state.windowActive = false;
    state.supportDrag = false;

    state.windowBounds = (Rectangle){ (float)GetScreenWidth()/2 - 360/2, 0, 360, 0 };
    state.panOffset = (Vector2){ 0, 0 };
    state.dragMode = false;

    // Calculate content height
    state.contentHeight = 0;
    for (int i = 0; i < GUIHELPWINDOW_MAX_LINES; i++)
    {
        if (helpLines[i] == NULL) break;
        else if ((helpLines[i][0] == '-') && (helpLines[i][1] == '\0'))  state.contentHeight += GUIHELPWINDOW_LINE_EMPTY_HEIGHT;
        else state.contentHeight += GUIHELPWINDOW_LINE_HEIGHT;
    }
    state.contentHeight += 8;    // Marging at the end

    // Calculate window height if not specifically defined
    if (state.windowBounds.height == 0) state.windowBounds.height = (float)(state.contentHeight + 24 + 4);
    state.windowBounds.y = GetScreenHeight()/2 - state.windowBounds.height/2;

    // Review size if it does not fit on the screen
    if (state.windowBounds.height > (GetScreenHeight() - 80))
    {
        state.windowBounds.height = (float)GetScreenHeight() - 80;
        state.windowBounds.y = GetScreenHeight()/2 - state.windowBounds.height/2;
    }
   
    // Init scroll offset
    state.scrollPanelOffset = (Vector2){ 0, 0 };

    return state;
}

// Gui window help
void GuiWindowHelp(GuiWindowHelpState *state)
{
    if (state->windowActive)
    {
        // Update window dragging
        //----------------------------------------------------------------------------------------
        if (state->supportDrag)
        {
            Vector2 mousePosition = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Window can be dragged from the top window bar
                if (CheckCollisionPointRec(mousePosition, (Rectangle){ state->windowBounds.x, state->windowBounds.y, state->windowBounds.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }))
                {
                    state->dragMode = true;
                    state->panOffset.x = mousePosition.x - state->windowBounds.x;
                    state->panOffset.y = mousePosition.y - state->windowBounds.y;
                }
            }

            if (state->dragMode)
            {
                state->windowBounds.x = (mousePosition.x - state->panOffset.x);
                state->windowBounds.y = (mousePosition.y - state->panOffset.y);

                // Check screen limits to avoid moving out of screen
                if (state->windowBounds.x < 0) state->windowBounds.x = 0;
                else if (state->windowBounds.x > (GetScreenWidth() - state->windowBounds.width)) state->windowBounds.x = GetScreenWidth() - state->windowBounds.width;

                if (state->windowBounds.y < 40) state->windowBounds.y = 40;
                else if (state->windowBounds.y > (GetScreenHeight() - state->windowBounds.height - 24)) state->windowBounds.y = GetScreenHeight() - state->windowBounds.height - 24;

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) state->dragMode = false;
            }
        }
        //----------------------------------------------------------------------------------------

        // Update window bounds and size: center to screen and allow scrolling
        //----------------------------------------------------------------------------------------
        state->windowBounds.x = GetScreenWidth()/2 - state->windowBounds.width/2;
        state->windowBounds.height = (float)(state->contentHeight + 24 + 4);
        state->windowBounds.y = GetScreenHeight()/2 - state->windowBounds.height/2;

        // Review size if it does not fit on the screen
        if (state->windowBounds.height > (GetScreenHeight() - 80))
        {
            state->windowBounds.height = (float)GetScreenHeight() - 80;
            state->windowBounds.y = GetScreenHeight()/2 - state->windowBounds.height/2;
        }
        //----------------------------------------------------------------------------------------

        // Draw window and controls
        //----------------------------------------------------------------------------------------
        state->windowActive = !GuiWindowBox(state->windowBounds, GuiIconText(ICON_HELP, "PXL-8 Help"));

        int nextLineY = (GUIHELPWINDOW_LINE_HEIGHT + 4);

        // Draw scroll panel considering window bounds and content size (only if required)
        Rectangle scissor = { 0 };
        if (state->contentHeight > (state->windowBounds.height - 24))
        {
            GuiScrollPanel((Rectangle){ state->windowBounds.x, state->windowBounds.y + GUIHELPWINDOW_LINE_HEIGHT - 1, state->windowBounds.width, state->windowBounds.height - 24 + 1 }, NULL,
                       (Rectangle){ state->windowBounds.x, state->windowBounds.y + GUIHELPWINDOW_LINE_HEIGHT, state->windowBounds.width - 16, (float)state->contentHeight }, &state->scrollPanelOffset, &scissor);
        }

        // Draw help info and separation lines
        // WARNING: We only scissor if scrolling is required, scissor mode forces a new draw call
        if (state->contentHeight > (state->windowBounds.height - 24)) BeginScissorMode((int)scissor.x, (int)scissor.y, (int)scissor.width + 2, (int)scissor.height);

            for (int i = 0; i < GUIHELPWINDOW_MAX_LINES; i++)
            {
                if (helpLines[i] == NULL) break;
                else if ((helpLines[i][0] == '-') && (helpLines[i][1] == '\0')) GuiLine((Rectangle){ state->windowBounds.x, state->windowBounds.y + nextLineY + state->scrollPanelOffset.y, state->windowBounds.width, GUIHELPWINDOW_LINE_EMPTY_HEIGHT }, NULL);
                else if (helpLines[i][0] == '-') GuiLine((Rectangle){ state->windowBounds.x, state->windowBounds.y + nextLineY + state->scrollPanelOffset.y, state->windowBounds.width, GUIHELPWINDOW_LINE_HEIGHT }, helpLines[i] + 1);
                else GuiLabel((Rectangle){ state->windowBounds.x + 12, state->windowBounds.y + nextLineY + state->scrollPanelOffset.y, state->windowBounds.width, GUIHELPWINDOW_LINE_HEIGHT }, helpLines[i]);

                if ((helpLines[i][0] == '-') && (helpLines[i][1] == '\0')) nextLineY += GUIHELPWINDOW_LINE_EMPTY_HEIGHT;
                else nextLineY += GUIHELPWINDOW_LINE_HEIGHT;
            }

        if (state->contentHeight > (state->windowBounds.height - 24)) EndScissorMode();
    }
}

void attemptHelpWindow(GuiPXL8State *state,GuiWindowHelpState *state2)
{
    if (state->helpWindowRequested) {
        state->helpWindowRequested = false;
        state2->windowActive = true;
    }
    GuiWindowHelp(state2);
}


#endif // GUI_WINDOW_HELP_IMPLEMENTATION