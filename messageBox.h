//
// Created by 5700XT on 24.09.2026.
//

#ifndef PXL_8_MESSAGEBOX_H
#define PXL_8_MESSAGEBOX_H
#include "gui_PXL8_Layout.h"
#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

inline bool showMessageBoxVisible = false;
inline char errorMessage[128];

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
            GuiIconText(ICON_WARNING, "Error"),
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

inline void invokeMessageBox(const char *message)
{
    strcpy(errorMessage, message);
    showMessageBoxVisible = true;
}


#ifdef __cplusplus
}
#endif

#endif //PXL_8_MESSAGEBOX_H
