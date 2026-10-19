//
// Created by 5700XT on 24.09.2026.
//

#ifndef PXL_8_MESSAGEBOX_H
#define PXL_8_MESSAGEBOX_H
#include "gui_PXL8_Layout.h"
#include "raylib.h"
#include <cstring>

inline bool showMessageBoxVisible = false;
inline char errorMessage[128];
inline char errorTitle[32];

enum MessageBoxType
{
    Warning,
    Info,
    Error,
    Debug
};

inline void attemptMessageBox(const GuiPXL8State *ui)
{
    if (showMessageBoxVisible) {
        GuiUnlock();
        int btnActive = -1;

        int msgLength = (int)strlen(errorMessage);
        int extraChars = (msgLength > 38) ? msgLength - 38 : 0;

        float boxWidth = 250.0f + extraChars * 6.0f;

        float boxX = (ui->anchor01.x + 515.0f + 125.0f) - boxWidth / 2.0f;

        GuiMessageBox(
            (Rectangle){
                boxX,
                ui->anchor01.y + 310.0f,
                boxWidth,
                100
            },
            errorTitle,
            errorMessage,
            "Ok",
            &btnActive
        );

        if (btnActive >= 0)
            showMessageBoxVisible = false;
        else
            GuiLock();
    }
}

inline void invokeMessageBox(const char *message, MessageBoxType type)
{
    switch (type) {
    case Warning:
        strncpy(errorTitle, GuiIconText(ICON_CIRCLE_WARNING_FILL, "Warning"),
                sizeof(errorTitle) - 1);
        break;

    case Info:
        strncpy(errorTitle, GuiIconText(ICON_INFO, "Info"),
                sizeof(errorTitle) - 1);
        break;

    case Error:
        strncpy(errorTitle, GuiIconText(ICON_WARNING, "Error"),
                sizeof(errorTitle) - 1);
        break;

    case Debug:
        strncpy(errorTitle, GuiIconText(ICON_GEAR, "Debug"),
                sizeof(errorTitle) - 1);
        break;
    }
    strcpy(errorMessage, message);
    showMessageBoxVisible = true;
}


#endif //PXL_8_MESSAGEBOX_H
