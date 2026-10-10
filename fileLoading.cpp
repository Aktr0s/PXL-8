//
// Created by Aktr0s on 24.09.2026.
//
#include <cstdlib>      // calloc/free for RL_CALLOC / RL_FREE
#include <cstring>
#include <vector>
#include <string>

#include "raylib.h"
#include "gui_PXL8_Layout.h"   // pulls in raygui.h

#ifndef RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#endif

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#include "fileLoading.hpp"
#include "messageBox.h"
#include "VM.hpp"

static char *loadedFileText = nullptr;
static std::vector<char *> loadedLines;
static GuiWindowFileDialogState fileDialogState;

void initFileDialog()
{
    fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    strcpy(fileDialogState.filterExt, ".pxasm");
}

static void handleScrollBox(GuiPXL8State *ui, const char *filename)  // NOT TO BE CALLED EXTERNALLY
{
    {
        // Free previous file
        if (loadedFileText)
        {
            UnloadFileText(loadedFileText);
            loadedFileText = nullptr;
        }

        loadedLines.clear();

        // Load entire file
        loadedFileText = LoadFileText(filename);

        if (!loadedFileText)
        {
            ui->codeLines = nullptr;
            ui->codeLineCount = 0;
            return;
        }

        char *lineStart = loadedFileText;

        for (char *p = loadedFileText; ; ++p)
        {
            if (*p == '\r' || *p == '\n' || *p == '\0')
            {
                char terminator = *p;

                *p = '\0';

                loadedLines.push_back(lineStart);

                if (terminator == '\0')
                    break;

                // Windows CRLF
                if (terminator == '\r' && p[1] == '\n')
                    ++p;

                lineStart = p + 1;
            }
        }

        ui->codeLines = loadedLines.data();
        ui->codeLineCount = static_cast<int>(loadedLines.size());

        loadFile();
    }
}

void attemptFileDialog(GuiPXL8State *ui)
{
    if (fileDialogState.windowActive) GuiUnlock();
    if (fileDialogState.SelectFilePressed)
    {
        strcpy(ui->fileLocation, TextFormat("%s" PATH_SEPERATOR "%s",fileDialogState.dirPathText, fileDialogState.fileNameText));
        bool validFile = (fileDialogState.fileNameText[0] != '\0') && FileExists(ui->fileLocation) && !DirectoryExists(ui->fileLocation) && IsFileExtension(ui->fileLocation, ".pxasm");
        if (validFile)
        {
            GuiPXL8SetHighlightLine(ui, 0);
            handleScrollBox(ui, ui->fileLocation);

        } else {

            //showMessageBox = true;
            //strcpy(errorMessage,"Wrong file or no file selected");
            invokeMessageBox("Wrong file or no file selected", MessageBoxType::Warning);
        }
        fileDialogState.SelectFilePressed = false;
    }
    GuiWindowFileDialog(&fileDialogState);
    if (ui->loadFileRequested)
    {
        fileDialogState.windowActive = true;
        ui->loadFileRequested = false;
    }
    if (ui->fileReloadRequested)
    {
        bool validFile = (fileDialogState.fileNameText[0] != '\0') && FileExists(ui->fileLocation) && !DirectoryExists(ui->fileLocation) && IsFileExtension(ui->fileLocation, ".pxasm");
        ui->fileReloadRequested = false;
        if (validFile){
            handleScrollBox(ui, ui->fileLocation);
        } else
        {
            //strcpy(errorMessage,"No file to reload");
            //showMessageBox = true;
            invokeMessageBox("No file to reload", MessageBoxType::Info);
        }
    }
    if (fileDialogState.windowActive) GuiLock();
}

void loadFile()
{
    std::vector<std::string> lines;
    lines.reserve(loadedLines.size());
    for (char* l : loadedLines) lines.emplace_back(l);
    LoadCode(lines);
    ResetProgram();
}


