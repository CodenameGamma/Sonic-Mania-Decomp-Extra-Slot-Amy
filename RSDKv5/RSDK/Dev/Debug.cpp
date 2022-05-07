#include "RSDK/Core/RetroEngine.hpp"

#if RETRO_PLATFORM == RETRO_WIN
#include <Windows.h>

#undef PRINT_ERROR // causes conflicts
#endif
#if RETRO_PLATFORM == RETRO_ANDROID
#include <android/log.h>
#endif

bool32 engineDebugMode = true;
bool32 useEndLine      = true;
char outputString[0x400];

#if RETRO_REV02
int32 debugValueCount = 0;
DebugValueInfo debugValues[DEBUGVALUE_LIST_COUNT];
#endif

DevMenu devMenu = DevMenu();

void PrintLog(int32 severity, const char *message, ...)
{
#ifndef RETRO_DISABLE_LOG
    if (engineDebugMode) {
        // make the full string
        va_list args;
        va_start(args, message);
        vsprintf(outputString, message, args);
        if (useEndLine)
            sprintf(outputString, "%s\n", outputString);
        else
            sprintf(outputString, "%s", outputString);
        va_end(args);

#if RETRO_REV02
        switch (severity) {
            default:
            case PRINT_NORMAL: break;

            case PRINT_POPUP:
                if (sceneInfo.state & 3) {
                    CreateEntity(RSDK::DevOutput->classID, outputString, 0, 0);
                }
                break;

            case PRINT_ERROR:
                if (sceneInfo.state & 3) {
                    engine.prevEngineMode = sceneInfo.state;
                    sceneInfo.state       = ENGINESTATE_ERRORMSG;
                }
                break;

            case PRINT_FATAL:
                if (sceneInfo.state & 3) {
                    engine.prevEngineMode = sceneInfo.state;
                    sceneInfo.state       = ENGINESTATE_ERRORMSG_FATAL;
                }
                break;
        }
#endif
        if (engine.consoleEnabled) {
            PrintConsole(outputString);
        }
        else {
#if RETRO_PLATFORM == RETRO_WIN
            OutputDebugStringA(outputString);
#elif RETRO_PLATFORM == RETRO_ANDROID
            int32 as = ANDROID_LOG_INFO;
            switch (severity) {
                case PRINT_ERROR: as = ANDROID_LOG_ERROR; break;
                case PRINT_FATAL: as = ANDROID_LOG_FATAL; break;
                default: break;
            }
            __android_log_print(as, "RSDKv5", "%s", outputString);
#elif RETRO_PLATFORM == RETRO_SWITCH
            printf("%s", outputString);
#endif
        }

#if !RETRO_USE_ORIGINAL_CODE
        char pathBuffer[0x100];
        sprintf(pathBuffer, BASE_PATH "log.txt");
        FileIO *file = fOpen(pathBuffer, "a");
        if (file) {
            fWrite(&outputString, 1, strlen(outputString), file);
            fClose(file);
        }
#endif
    }
#endif
}

#if RETRO_REV02
void SetDebugValue(const char *name, void *valPtr, int32 type, int32 min, int32 max)
{
    if (debugValueCount < DEBUGVALUE_COUNT) {
        DebugValueInfo *value = &debugValues[debugValueCount++];
        strncpy(value->name, name, 0x10);
        value->value = valPtr;
        switch (type) {
            case DTYPE_BOOL:
                value->type = DEBUGDISPLAY_BOOL;
                value->size = sizeof(bool32);
                break;

            case DTYPE_UINT8:
                value->type = DEBUGDISPLAY_UNSIGNED;
                value->size = sizeof(uint8);
                break;

            case DTYPE_UINT16:
                value->type = DEBUGDISPLAY_UNSIGNED;
                value->size = sizeof(uint16);
                break;

            case DTYPE_UINT32:
                value->type = DEBUGDISPLAY_UNSIGNED;
                value->size = sizeof(uint32);
                break;

            case DTYPE_INT8:
                value->type = DEBUGDISPLAY_SIGNED;
                value->size = sizeof(int8);
                break;

            case DTYPE_INT16:
                value->type = DEBUGDISPLAY_SIGNED;
                value->size = sizeof(int16);
                break;

            case DTYPE_INT32:
                value->type = DEBUGDISPLAY_SIGNED;
                value->size = sizeof(int32);
                break;

            default: break;
        }

        value->min = min;
        value->max = max;
    }
}
#endif

#if !RETRO_REV02
void PrintMessage(void *msg, int32 staticVars)
{
    if (msg && engine.consoleEnabled) {
        switch (staticVars) {
            case 0: PrintLog(PRINT_NORMAL, "%s", (const char *)msg); break;
            case 1: PrintLog(PRINT_NORMAL, "%i", *(int32 *)msg); break;
            case 2: PrintLog(PRINT_NORMAL, "%i", *(uint32 *)msg, 0); break;
            case 3: PrintLog(PRINT_NORMAL, "%f", *(float *)msg); break;
            default: break;
        }
    }
}
#endif

#if !RETRO_USE_ORIGINAL_CODE
uint8 touchTimer = 0;

void DevMenu_HandleTouchControls()
{
    if (!controller[CONT_ANY].keyStart.down && !controller[CONT_ANY].keyUp.down && !controller[CONT_ANY].keyDown.down) {
        for (int32 t = 0; t < touchMouseData.count; ++t) {
            if (touchMouseData.down[t] && !(touchTimer % 8)) {
                int32 tx = touchMouseData.x[t] * screens->size.x;
                int32 ty = touchMouseData.y[t] * screens->size.y;

                if (tx < screens->center.x) {
                    if (ty >= screens->center.y) {
                        if (!controller[CONT_ANY].keyDown.down)
                            controller[CONT_ANY].keyDown.press = true;

                        controller[CONT_ANY].keyDown.down = true;
                        break;
                    }
                    else {
                        if (!controller[CONT_ANY].keyUp.down)
                            controller[CONT_ANY].keyUp.press = true;

                        controller[CONT_ANY].keyUp.down = true;
                        break;
                    }
                }
                else if (tx > screens->center.x) {
                    if (ty > screens->center.y) {
                        if (!controller[CONT_ANY].keyStart.down)
                            controller[CONT_ANY].keyStart.press = true;

                        controller[CONT_ANY].keyStart.down = true;
                        break;
                    }
                    else {
                        if (!controller[CONT_ANY].keyB.down)
                            controller[CONT_ANY].keyB.press = true;

                        controller[CONT_ANY].keyB.down = true;
                        break;
                    }
                }
            }
        }
    }

    touchTimer++;
}
#endif

void DevMenu_MainMenu()
{
#if !RETRO_USE_MOD_LOADER
    int32 selectionCount         = 5;
    uint32 selectionColors[]     = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    const char *selectionNames[] = { "Resume", "Restart", "Stage Select", "Options", "Exit" };
#else
    int32 selectionCount         = 6;
    uint32 selectionColors[]     = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    const char *selectionNames[] = { "Resume", "Restart", "Stage Select", "Options", "Mods", "Exit" };
#endif
    selectionColors[devMenu.selection] = 0xF0F0F0;

    // Info Box
    int32 y = currentScreen->center.y - 80;
    DrawRectangle(currentScreen->center.x - 128, currentScreen->center.y - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);
    DrawDevString("RETRO ENGINE v5", currentScreen->center.x, y, ALIGN_CENTER, 0xF0F0F0);

    y += 8;
    DrawDevString("Dev Menu", currentScreen->center.x, y, ALIGN_CENTER, 0xF0F0F0);

    y += 16;
    DrawDevString(RSDK::gameVerInfo.gameName, currentScreen->center.x, y, ALIGN_CENTER, 0x808090);

    y += 8;
    DrawDevString(RSDK::gameVerInfo.gameVersion, currentScreen->center.x, y, ALIGN_CENTER, 0x808090);

    // Selections Box
    y += 24;
    DrawRectangle(currentScreen->center.x - 128, y - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

#if RETRO_USE_MOD_LOADER
    y -= 6;
#endif
    for (int32 i = 0; i < selectionCount; ++i) {
        DrawDevString(selectionNames[i], currentScreen->center.x, y, ALIGN_CENTER, selectionColors[i]);
        y += 12;
    }
    y += 20;

    // Storage box

    // Stage Storage
    int32 stgUsed =
        (sizeof(int32) * RSDK::dataStorage[RSDK::DATASET_STG].usedStorage) / (float)RSDK::dataStorage[RSDK::DATASET_STG].storageLimit * 126.0;
    DrawRectangle(currentScreen->center.x - 40, y, 0x80, 0x08, 0x80, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x - 39, y + 1, stgUsed, 6, 0xF0F0F0, 0xFF, INK_NONE, true);
    DrawDevString("STG", currentScreen->center.x - 64, y, 0, 0xF0F080);

    // Music Storage
    int32 musUsed =
        (sizeof(int32) * RSDK::dataStorage[RSDK::DATASET_MUS].usedStorage) / (float)RSDK::dataStorage[RSDK::DATASET_MUS].storageLimit * 126.0;
    y += 10;
    DrawRectangle(currentScreen->center.x - 40, y, 0x80, 0x08, 0x80, 0xFF5, INK_NONE, true);
    DrawRectangle(currentScreen->center.x - 39, y + 1, musUsed, 6, 0xF0F0F0, 0xFF, INK_NONE, true);
    DrawDevString("MUS", currentScreen->center.x - 64, y, 0, 0xF0F080);

    // SoundFX Storage
    int32 sfxUsed =
        (sizeof(int32) * RSDK::dataStorage[RSDK::DATASET_SFX].usedStorage) / (float)RSDK::dataStorage[RSDK::DATASET_SFX].storageLimit * 126.0;
    y += 10;
    DrawRectangle(currentScreen->center.x - 40, y, 0x80, 0x08, 0x80, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x - 39, y + 1, sfxUsed, 6, 0xF0F0F0, 0xFF, INK_NONE, true);
    DrawDevString("SFX", currentScreen->center.x - 64, y, 0, 0xF0F080);

    // String Storage
    int32 strUsed =
        (sizeof(int32) * RSDK::dataStorage[RSDK::DATASET_STR].usedStorage) / (float)RSDK::dataStorage[RSDK::DATASET_STR].storageLimit * 126.0;
    y += 10;
    DrawRectangle(currentScreen->center.x - 40, y, 0x80, 0x08, 0x80, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x - 39, y + 1, strUsed, 6, 0xF0F0F0, 0xFF, INK_NONE, true);
    DrawDevString("STR", currentScreen->center.x - 64, y, 0, 0xF0F080);

    // Temp Storage
    int32 tmpUsed =
        (sizeof(int32) * RSDK::dataStorage[RSDK::DATASET_TMP].usedStorage) / (float)RSDK::dataStorage[RSDK::DATASET_TMP].storageLimit * 126.0;
    y += 10;
    DrawRectangle(currentScreen->center.x - 40, y, 0x80, 0x08, 0x80, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x - 39, y + 1, tmpUsed, 6, 0xF0F0F0, 0xFF, INK_NONE, true);
    DrawDevString("TMP", currentScreen->center.x - 64, y, 0, 0xF0F080);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        devMenu.selection--;
        devMenu.timer = 1;

        if (devMenu.selection < 0)
            devMenu.selection += selectionCount;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (devMenu.timer) {
            devMenu.timer = ++devMenu.timer & 7;
        }
        else {
            devMenu.selection--;
            devMenu.timer = ++devMenu.timer & 7;

            if (devMenu.selection < 0)
                devMenu.selection += selectionCount;
        }
    }

    if (controller[CONT_ANY].keyDown.press) {
        devMenu.selection++;
        devMenu.timer = 1;

        if (devMenu.selection >= selectionCount)
            devMenu.selection -= selectionCount;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (devMenu.timer) {
            devMenu.timer = ++devMenu.timer & 7;
        }
        else {
            devMenu.selection++;
            devMenu.timer = ++devMenu.timer & 7;

            if (devMenu.selection >= selectionCount)
                devMenu.selection -= selectionCount;
        }
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm) {
        switch (devMenu.selection) {
            case 0: sceneInfo.state = devMenu.sceneState; break;

            case 1: sceneInfo.state = ENGINESTATE_LOAD; break;

            case 2:
                devMenu.state     = DevMenu_CategorySelectMenu;
                devMenu.selection = 0;
                devMenu.timer     = 1;
                break;

            case 3:
                devMenu.state     = DevMenu_OptionsMenu;
                devMenu.selection = 0;
                devMenu.timer     = 1;
                break;

#if !RETRO_USE_MOD_LOADER
            case 4: engine.running = false; break;
#else
            case 4:
                RSDK::LoadMods(); // reload our mod list real quick
                devMenu.state     = DevMenu_ModsMenu;
                devMenu.selection = 0;
                devMenu.timer     = 1;
                break;

            case 5: RenderDevice::isRunning = false; break;
#endif

            default: break;
        }
    }
}
void DevMenu_CategorySelectMenu()
{
    uint32 selectionColors[] = {
        0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090,
    };
    selectionColors[devMenu.selection - devMenu.scrollPos] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("SELECT STAGE CATEGORY", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);
    DrawRectangle(currentScreen->center.x - 128, dy + 36, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    int32 y = dy + 40;
    for (int32 i = 0; i < 8; ++i) {
        if (devMenu.scrollPos + i < sceneInfo.categoryCount) {
            DrawDevString(sceneInfo.listCategory[devMenu.scrollPos + i].name, currentScreen->center.x - 64, y, ALIGN_LEFT, selectionColors[i]);
            y += 8;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection += sceneInfo.categoryCount;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (devMenu.timer) {
            devMenu.timer = (devMenu.timer + 1) & 7;

            if (devMenu.selection >= devMenu.scrollPos) {
                if (devMenu.selection > devMenu.scrollPos + 7)
                    devMenu.scrollPos = devMenu.selection - 7;
            }
            else {
                devMenu.scrollPos = devMenu.selection;
            }
        }
        else {
            if (--devMenu.selection < 0)
                devMenu.selection += sceneInfo.categoryCount;

            devMenu.timer = (devMenu.timer + 1) & 7;

            if (devMenu.selection >= devMenu.scrollPos) {
                if (devMenu.selection > devMenu.scrollPos + 7)
                    devMenu.scrollPos = devMenu.selection - 7;
            }
            else {
                devMenu.scrollPos = devMenu.selection;
            }
        }
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection == sceneInfo.categoryCount)
            devMenu.selection = 0;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (devMenu.timer) {
            devMenu.timer = (devMenu.timer + 1) & 7;

            if (devMenu.selection >= devMenu.scrollPos) {
                if (devMenu.selection > devMenu.scrollPos + 7)
                    devMenu.scrollPos = devMenu.selection - 7;
            }
            else {
                devMenu.scrollPos = devMenu.selection;
            }
        }
        else {
            if (++devMenu.selection == sceneInfo.categoryCount)
                devMenu.selection = 0;

            devMenu.timer = (devMenu.timer + 1) & 7;

            if (devMenu.selection >= devMenu.scrollPos) {
                if (devMenu.selection > devMenu.scrollPos + 7)
                    devMenu.scrollPos = devMenu.selection - 7;
            }
            else {
                devMenu.scrollPos = devMenu.selection;
            }
        }
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm) {
        if (sceneInfo.listCategory[devMenu.selection].sceneCount) {
            devMenu.state     = DevMenu_SceneSelectMenu;
            devMenu.listPos   = devMenu.selection;
            devMenu.scrollPos = 0;
            devMenu.selection = 0;
        }
    }
#if !RETRO_USE_ORIGINAL_CODE
    else if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_MainMenu;
        devMenu.listPos   = 0;
        devMenu.scrollPos = 0;
        devMenu.selection = 2;
    }
#endif
}
void DevMenu_SceneSelectMenu()
{
    uint32 selectionColors[] = {
        0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090,
    };
    selectionColors[devMenu.selection - devMenu.scrollPos] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("SELECT STAGE SCENE", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);
    DrawRectangle(currentScreen->center.x - 128, dy + 36, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    int32 y             = dy + 40;
    SceneListInfo *list = &sceneInfo.listCategory[devMenu.listPos];
    int32 start         = list->sceneOffsetStart;
    for (int32 i = 0; i < 8; ++i) {
        if (devMenu.scrollPos + i < list->sceneCount) {
            DrawDevString(sceneInfo.listData[start + (devMenu.scrollPos + i)].name, currentScreen->center.x + 96, y, ALIGN_RIGHT, selectionColors[i]);
            y += 8;
            devMenu.scrollPos = devMenu.scrollPos;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (start + --devMenu.selection < list->sceneOffsetStart)
            devMenu.selection = list->sceneCount - 1;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7) {
                devMenu.scrollPos = devMenu.selection - 7;
            }
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && start + --devMenu.selection < list->sceneOffsetStart)
            devMenu.selection = list->sceneCount - 1;

        devMenu.timer = (devMenu.timer + 1) & 7;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection >= list->sceneCount)
            devMenu.selection = 0;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection >= list->sceneCount)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm) {
        sceneInfo.activeCategory = devMenu.listPos;
        sceneInfo.listPos        = devMenu.selection + list->sceneOffsetStart;
        sceneInfo.state          = ENGINESTATE_LOAD;

        // Bug Details(?):
        // rev01 had this here, rev02 does not.
        // This can cause an annoying popup when starting a stage, so it's re-added for anyone who doesn't like that popup
#if !RETRO_REV02 || !RETRO_USE_ORIGINAL_CODE
        AssignControllerID(CONT_P1, CONT_AUTOASSIGN);
#endif
    }
#if !RETRO_USE_ORIGINAL_CODE
    else if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_CategorySelectMenu;
        devMenu.scrollPos = 0;
        devMenu.selection = 0;
        devMenu.listPos   = 0;
    }
#endif
}
void DevMenu_OptionsMenu()
{
    const uint8 selectionCount = RETRO_REV02 ? 5 : 4;
#if RETRO_REV02
    uint32 selectionColors[] = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
#else
    uint32 selectionColors[] = { 0x808090, 0x808090, 0x808090, 0x808090 };
#endif
    selectionColors[devMenu.selection] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 256, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("OPTIONS", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 44;
    DrawRectangle(currentScreen->center.x - 128, dy - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    DrawDevString("Video Settings", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[0]);

    dy += 12;
    DrawDevString("Audio Settings", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[1]);

    dy += 12;
    DrawDevString("Configure Input", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[2]);

#if RETRO_REV02
    dy += 12;
    DrawDevString("Debug Flags", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[3]);

#endif
    DrawDevString("Back", currentScreen->center.x, dy + 12, ALIGN_CENTER, selectionColors[selectionCount - 1]);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = selectionCount - 1;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = selectionCount - 1;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection >= selectionCount)
            devMenu.selection = 0;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection >= selectionCount)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm) {
        switch (devMenu.selection) {
            case 0: {
                devMenu.windowed    = RSDK::videoSettings.windowed;
                devMenu.windowScale = (RSDK::videoSettings.windowWidth / RSDK::videoSettings.pixWidth) - 1;

                int32 aspect = (int32)((RSDK::videoSettings.windowWidth / (float)RSDK::videoSettings.windowHeight) * (float)SCREEN_YSIZE) >> 3;
                switch (aspect) {
                    default:
                    case 40: devMenu.windowAspect = 0; break; // 4:3
                    case 45: devMenu.windowAspect = 1; break; // 3:2
                    case 48: devMenu.windowAspect = 2; break; // 16:10
                    case 50: devMenu.windowAspect = 3; break; // 5:3
                    case 53: devMenu.windowAspect = 4; break; // 16:9
                }

                devMenu.state     = DevMenu_VideoOptionsMenu;
                devMenu.selection = 0;
                break;
            }

            case 1:
                devMenu.state     = DevMenu_AudioOptionsMenu;
                devMenu.selection = 0;
                break;

            case 2:
                devMenu.state     = DevMenu_InputOptionsMenu;
                devMenu.selection = 0;
                break;

#if RETRO_REV02
            case 3: devMenu.state = DevMenu_DebugOptionsMenu; devMenu.selection = 0;

#if !RETRO_USE_ORIGINAL_CODE
                // reset this just to be sure there's no crashing since we can go back from prev menus unlike original RSDKv5
                devMenu.scrollPos = 0;
#endif
                break;

            case 4:
#else
            case 3:
#endif
                devMenu.state     = DevMenu_MainMenu;
                devMenu.selection = 0;
                break;
        }
    }
#if !RETRO_USE_ORIGINAL_CODE
    else if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_MainMenu;
        devMenu.listPos   = 0;
        devMenu.scrollPos = 0;
        devMenu.selection = 3;
    }
#endif
}
void DevMenu_VideoOptionsMenu()
{
    uint32 selectionColors[]           = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    selectionColors[devMenu.selection] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("VIDEO SETTINGS", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 44;
    DrawRectangle(currentScreen->center.x - 128, dy - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    DrawDevString("Window Size:", currentScreen->center.x - 96, dy, 0, selectionColors[0]);

    const char *scale = "unknown";
    switch (devMenu.windowScale) {
        case 0: scale = "1x"; break;
        case 1: scale = "2x"; break;
        case 2: scale = "3x"; break;
        case 3: scale = "4x"; break;
        default: break;
    }
    DrawDevString(scale, currentScreen->center.x + 80, dy, ALIGN_CENTER, 0xF0F080);

    dy += 8;
    DrawDevString("Window Aspect:", currentScreen->center.x - 96, dy, 0, selectionColors[1]);

    const char *aspect = "unknown";
    switch (devMenu.windowAspect) {
        case 0: aspect = "4:3"; break;
        case 1: aspect = "3:2"; break;
        case 2: aspect = "16:10"; break;
        case 3: aspect = "5:3"; break;
        case 4: aspect = "16:9"; break;
        default: break;
    }
    DrawDevString(aspect, currentScreen->center.x + 80, dy, ALIGN_CENTER, 0xF0F080);

    dy += 8;
    DrawDevString("Fullscreen:", currentScreen->center.x - 96, dy, 0, selectionColors[2]);
    DrawDevString(devMenu.windowed ? "NO" : "YES", currentScreen->center.x + 80, dy, ALIGN_CENTER, 0xF0F080);

    dy += 8;
    DrawDevString("Screen Shader:", currentScreen->center.x - 96, dy, ALIGN_LEFT, selectionColors[3]);
    DrawDevString(shaderList[RSDK::videoSettings.shaderID].name, currentScreen->center.x + 80, dy, ALIGN_CENTER, 0xF0F080);

    dy += 16;
    DrawDevString("Confirm", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[4]);
    DrawDevString("Cancel", currentScreen->center.x, dy + 8, ALIGN_CENTER, selectionColors[5]);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = 5;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = 5;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection > 5)
            devMenu.selection = 0;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection > 5)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    switch (devMenu.selection) {
        case 0: // scale
            if (controller[CONT_ANY].keyLeft.press) {
                devMenu.windowScale        = (devMenu.windowScale - 1) & 3;
                RSDK::changedVideoSettings = true;
            }
            else if (controller[CONT_ANY].keyRight.press) {
                devMenu.windowScale        = (devMenu.windowScale + 1) & 3;
                RSDK::changedVideoSettings = true;
            }
            break;

        case 1: // aspect
            if (controller[CONT_ANY].keyLeft.press) {
                devMenu.windowAspect--;
                RSDK::changedVideoSettings = true;
            }
            else if (controller[CONT_ANY].keyRight.press) {
                devMenu.windowAspect++;
                RSDK::changedVideoSettings = true;
            }

            if (devMenu.windowAspect > 4)
                devMenu.windowAspect = 0;
            else if (devMenu.windowAspect < 0)
                devMenu.windowAspect = 4;
            break;

        case 2: // fullscreen
            if (controller[CONT_ANY].keyLeft.press || controller[CONT_ANY].keyRight.press) {
                devMenu.windowed ^= 1;
                RSDK::changedVideoSettings = true;
            }
            break;

        case 3: // screenShader
            if (controller[CONT_ANY].keyLeft.press) {
                RSDK::videoSettings.shaderID--;
                RSDK::changedVideoSettings = true;
            }
            else if (controller[CONT_ANY].keyRight.press) {
                RSDK::videoSettings.shaderID++;
                RSDK::changedVideoSettings = true;
            }

            if (RSDK::videoSettings.shaderID >= userShaderCount)
                RSDK::videoSettings.shaderID = SHADER_NONE;
            else if (RSDK::videoSettings.shaderID < SHADER_NONE)
                RSDK::videoSettings.shaderID = userShaderCount - 1;
            break;

        case 4: // confirm
        {
            bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
            if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
            if (RSDK::SKU::GetConfirmButtonFlip())
#endif
                confirm = controller[CONT_ANY].keyB.press;

            if (controller[CONT_ANY].keyStart.press || confirm) {
                // do confirm
                RSDK::videoSettings.windowed = devMenu.windowed;
                shaderList[0].linear         = !devMenu.windowed;
                if (!devMenu.windowScale)
                    RSDK::videoSettings.shaderID = SHADER_NONE;

                int32 width = 0;
                switch (devMenu.windowAspect) {
                    default: width = RSDK::videoSettings.windowWidth; break;
                    case 0: width = 3 - (RSDK::videoSettings.pixHeight * -1.3333334f); break; // 16:9
                    case 1: width = 3 - (RSDK::videoSettings.pixHeight * -1.5f); break;       // 4:3
                    case 2: width = 3 - (RSDK::videoSettings.pixHeight * -1.6f); break;       // 3:2
                    case 3: width = 3 - (RSDK::videoSettings.pixHeight * -1.6666666f); break; // 16:10
                    case 4: width = 3 - (RSDK::videoSettings.pixHeight * -1.7777778f); break; // 5:3
                }
                width &= 0x7FF8;
                if (width > 424)
                    width = 424;
                RSDK::videoSettings.windowWidth  = width * (devMenu.windowScale + 1);
                RSDK::videoSettings.windowHeight = RSDK::videoSettings.pixHeight * (devMenu.windowScale + 1);
                UpdateGameWindow();

                devMenu.state     = DevMenu_OptionsMenu;
                devMenu.selection = 0;
            }
            break;
        }

        case 5: // cancel
        {
            bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
            if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
            if (RSDK::SKU::GetConfirmButtonFlip())
#endif
                confirm = controller[CONT_ANY].keyB.press;

            if (controller[CONT_ANY].keyStart.press || confirm) {
                devMenu.state     = DevMenu_OptionsMenu;
                devMenu.selection = 0;
            }
            break;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_OptionsMenu;
        devMenu.selection = 0;
    }
#endif
}
void DevMenu_AudioOptionsMenu()
{
    uint32 selectionColors[]           = { 0x808090, 0x808090, 0x808090, 0x808090 };
    selectionColors[devMenu.selection] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("AUDIO SETTINGS", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 44;
    DrawRectangle(currentScreen->center.x - 128, dy - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    DrawDevString("Streams Enabled:", currentScreen->center.x - 96, dy, 0, selectionColors[0]);
    DrawDevString(engine.streamsEnabled ? "YES" : "NO", currentScreen->center.x + 80, dy, ALIGN_CENTER, 0xF0F080);

    dy += 16;
    DrawDevString("Streams Vol:", currentScreen->center.x - 96, dy, 0, selectionColors[1]);
    DrawRectangle(currentScreen->center.x + 8, dy, 112, 8, 0x000000, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x + 9, dy + 1, engine.streamVolume * 110.0, 6, 0xF0F0F0, 255, INK_NONE, true);

    dy += 16;
    DrawDevString("SoundFX Vol:", currentScreen->center.x - 96, dy, 0, selectionColors[2]);
    DrawRectangle(currentScreen->center.x + 8, dy, 112, 8, 0x000000, 0xFF, INK_NONE, true);
    DrawRectangle(currentScreen->center.x + 9, dy + 1, engine.soundFXVolume * 110.0, 6, 0xF0F0F0, 255, INK_NONE, true);
    DrawDevString("Back", currentScreen->center.x, dy + 16, ALIGN_CENTER, selectionColors[3]);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = 3;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = 3;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection > 3)
            devMenu.selection = 0;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection > 3)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    switch (devMenu.selection) {
        case 0:
            if (controller[CONT_ANY].keyLeft.press || controller[CONT_ANY].keyRight.press) {
                engine.streamsEnabled ^= 1;
                RSDK::changedVideoSettings = true;
            }
            break;

        case 1:
            if (controller[CONT_ANY].keyLeft.down) {
                engine.streamVolume -= 1.0 / 64.0;
                if (engine.streamVolume < 0.0)
                    engine.streamVolume = 0.0;

                RSDK::changedVideoSettings = true;
            }
            else {
                if (controller[CONT_ANY].keyRight.down) {
                    engine.streamVolume += 1.0 / 64.0;
                    if (engine.streamVolume > 1.0)
                        engine.streamVolume = 1.0;

                    RSDK::changedVideoSettings = true;
                }
            }
            break;

        case 2:
            if (controller[CONT_ANY].keyLeft.down) {
                engine.soundFXVolume -= 1.0 / 64.0;
                if (engine.soundFXVolume < 0.0)
                    engine.soundFXVolume = 0.0;

                RSDK::changedVideoSettings = true;
            }
            else {
                if (controller[CONT_ANY].keyRight.down) {
                    engine.soundFXVolume += 1.0 / 64.0;
                    if (engine.soundFXVolume > 1.0)
                        engine.soundFXVolume = 1.0;

                    RSDK::changedVideoSettings = true;
                }
            }
            break;

        case 3: {
            bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
            if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
            if (RSDK::SKU::GetConfirmButtonFlip())
#endif
                confirm = controller[CONT_ANY].keyB.press;

            if (controller[CONT_ANY].keyStart.press || confirm) {
                devMenu.state     = DevMenu_OptionsMenu;
                devMenu.selection = 1;
            }
            break;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_OptionsMenu;
        devMenu.selection = 1;
    }
#endif
}
void DevMenu_InputOptionsMenu()
{
    uint32 selectionColors[]           = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    selectionColors[devMenu.selection] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("CONFIGURE INPUT", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 44;
    DrawRectangle(currentScreen->center.x - 128, dy - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    DrawDevString("Set Keys For Input 1", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[0]);

    dy += 10;
    DrawDevString("Set Keys For Input 2", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[1]);

    dy += 10;
    DrawDevString("Set Keys For Input 3", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[2]);

    dy += 10;
    DrawDevString("Set Keys For Input 4", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[3]);

    DrawDevString("Back", currentScreen->center.x, dy + 18, ALIGN_CENTER, selectionColors[4]);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = 4;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = 4;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection > 4)
            devMenu.selection = 0;

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection > 4)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm) {
        if (devMenu.selection == 4) {
            devMenu.state     = DevMenu_OptionsMenu;
            devMenu.selection = 3;
        }
        else {
            devMenu.state              = DevMenu_KeyMappingsMenu;
            devMenu.scrollPos          = 0;
            RSDK::changedVideoSettings = true;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_OptionsMenu;
        devMenu.selection = 2;
    }
#endif
}
void DevMenu_KeyMappingsMenu()
{
#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("SET KEY BINDING", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 44;
    DrawRectangle(currentScreen->center.x - 128, dy - 8, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    int32 controllerID = devMenu.selection + 1;
    switch (devMenu.scrollPos) {
        case 0:
            DrawDevString("Press Key For UP", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyUp.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyDown.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 1:
            DrawDevString("Press Key For DOWN", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyDown.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyLeft.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 2:
            DrawDevString("Press Key For LEFT", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyLeft.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyRight.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 3:
            DrawDevString("Press Key For RIGHT", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyRight.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyA.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 4:
            DrawDevString("Press Key For BUTTON A", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyA.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyB.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 5:
            DrawDevString("Press Key For BUTTON B", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyB.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyC.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 6:
            DrawDevString("Press Key For BUTTON C", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyC.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyX.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 7:
            DrawDevString("Press Key For BUTTON X", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyX.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyY.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 8:
            DrawDevString("Press Key For BUTTON Y", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyY.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyZ.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 9:
            DrawDevString("Press Key For BUTTON Z", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyZ.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keyStart.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 10:
            DrawDevString("Press Key For START", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keyStart.keyMap != KEYMAP_AUTO_MAPPING) {
                controller[controllerID].keySelect.keyMap = KEYMAP_AUTO_MAPPING;
                ++devMenu.scrollPos;
            }
            break;

        case 11:
            DrawDevString("Press Key For SELECT", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F080);
            if (controller[controllerID].keySelect.keyMap != KEYMAP_AUTO_MAPPING)
                devMenu.state = DevMenu_InputOptionsMenu;
            break;

        default: break;
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_OptionsMenu;
        devMenu.selection = 3;
    }
#endif
}
#if RETRO_REV02
void DevMenu_DebugOptionsMenu()
{
    uint32 selectionColors[]                               = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    selectionColors[devMenu.selection - devMenu.scrollPos] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("CONFIGURE DEBUG FLAGS", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);

    dy += 40;
    DrawRectangle(currentScreen->center.x - 128, dy - 4, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    bool32 confirm = controller[CONT_ANY].keyA.press;
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
        confirm = controller[CONT_ANY].keyB.press;

    confirm |= controller[CONT_ANY].keyStart.press;

    for (int32 i = 0; i < 8; ++i) {
        if (devMenu.scrollPos + i < debugValueCount) {
            DebugValueInfo *value = &debugValues[devMenu.scrollPos + i];
            DrawDevString(value->name, currentScreen->center.x - 96, dy, ALIGN_LEFT, selectionColors[i]);

            if (!value->value) {
                DrawDevString("--------", currentScreen->center.x + 96, dy, ALIGN_RIGHT, 0xF0F080);
            }
            else {
                char valueStr[0x10];
                strcpy(valueStr, "--------");

                switch (value->size) {
                    default: DrawDevString("--------", currentScreen->center.x + 96, dy, ALIGN_RIGHT, 0xF0F080); break;

                    case sizeof(uint8): {
                        uint8 *v = (uint8 *)value->value;

                        switch (value->type) {
                            case DEBUGDISPLAY_BOOL:
                                valueStr[0] = *v ? 'Y' : 'N';
                                valueStr[1] = 0;
                                break;

                            default:
                            case DEBUGDISPLAY_UNSIGNED: valueStr[0] = ' '; break;

                            case DEBUGDISPLAY_SIGNED:
                                valueStr[0] = *v > 0x7F ? '-' : ' ';
                                *v &= 0x7F;
                                break;
                        }
                        break;
                    }

                    case sizeof(uint16): {
                        uint16 *v = (uint16 *)value->value;

                        switch (value->type) {
                            case DEBUGDISPLAY_BOOL:
                                valueStr[0] = *v ? 'Y' : 'N';
                                valueStr[1] = 0;
                                break;

                            default:
                            case DEBUGDISPLAY_UNSIGNED: valueStr[0] = ' '; break;

                            case DEBUGDISPLAY_SIGNED:
                                valueStr[0] = *v > 0x7FFF ? '-' : ' ';
                                *v &= 0x7FFF;
                                break;
                        }
                        break;
                    }

                    case sizeof(uint32): {
                        uint32 *v = (uint32 *)value->value;

                        switch (value->type) {
                            case DEBUGDISPLAY_BOOL:
                                valueStr[0] = *v ? 'Y' : 'N';
                                valueStr[1] = 0;
                                break;

                            default:
                            case DEBUGDISPLAY_UNSIGNED: valueStr[0] = ' '; break;

                            case DEBUGDISPLAY_SIGNED:
                                valueStr[0] = *v > 0x7FFFFFFF ? '-' : ' ';
                                *v &= 0x7FFFFFFF;
                                break;
                        }
                        break;
                    }
                }

                if (value->type != DEBUGDISPLAY_BOOL) {
                    if (2 * value->size) {
                        char *curChar                   = &valueStr[2 * value->size];
                        valueStr[(2 * value->size) + 1] = 0;

                        switch (value->size) {
                            default: break;

                            case sizeof(uint8): {
                                uint8 *valuePtr = (uint8 *)value->value;

                                for (int32 v = 0; v < 2 * value->size; ++v) *curChar-- = ((v & 0xF) > 9 ? '7' : '0') + ((*valuePtr >> 4 * v) & 0xF);
                                break;
                            }

                            case sizeof(uint16): {
                                uint16 *valuePtr = (uint16 *)value->value;

                                for (int32 v = 0; v < 2 * value->size; ++v) *curChar-- = ((v & 0xF) > 9 ? '7' : '0') + ((*valuePtr >> 4 * v) & 0xF);
                                break;
                            }

                            case sizeof(uint32): {
                                uint32 *valuePtr = (uint32 *)value->value;

                                for (int32 v = 0; v < 2 * value->size; ++v) *curChar-- = ((v & 0xF) > 9 ? '7' : '0') + ((*valuePtr >> 4 * v) & 0xF);
                                break;
                            }
                        }
                    }
                }

                DrawDevString(valueStr, currentScreen->center.x + 96, dy, ALIGN_CENTER, selectionColors[i]);
            }
            dy += 8;
        }
        else {
            DrawDevString("Back", currentScreen->center.x, dy, ALIGN_CENTER, selectionColors[i]);
        }
    }

    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = debugValueCount;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = debugValueCount;

        devMenu.timer = (devMenu.timer + 1) & 7;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection > debugValueCount)
            devMenu.selection = 0;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection >= debugValueCount)
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;
        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    if (devMenu.selection < debugValueCount) {
        DebugValueInfo *value = &debugValues[devMenu.selection];

        switch (value->size) {
            default: DrawDevString("--------", currentScreen->center.x + 96, dy, ALIGN_RIGHT, 0xF0F080); break;

            case sizeof(int8): {
                int8 *valuePtr = (int8 *)value->value;

                if (controller[CONT_ANY].keyLeft.press) {
                    if (value->type == DEBUGDISPLAY_BOOL)
                        *valuePtr ^= 1;
                    else if (*valuePtr - 1 >= value->min)
                        *valuePtr--;
                }

                if (controller[CONT_ANY].keyRight.press) {
                    if (value->type == DEBUGDISPLAY_BOOL)
                        *valuePtr ^= 1;
                    else if (*valuePtr + 1 <= value->max)
                        *valuePtr++;
                }
                break;
            }

            case sizeof(int16): {
                int16 *valuePtr = (int16 *)value->value;

                if (controller[CONT_ANY].keyLeft.press) {
                    if (value->type == DEBUGDISPLAY_BOOL)
                        *valuePtr ^= 1;
                    else if (*valuePtr - 1 >= value->min)
                        *valuePtr--;
                }

                if (controller[CONT_ANY].keyRight.press) {
                    if (value->type == DEBUGDISPLAY_BOOL)
                        *valuePtr ^= 1;
                    else if (*valuePtr + 1 <= value->max)
                        *valuePtr++;
                }
                break;
            }

            case sizeof(int32): {
                int32 *valuePtr = (int32 *)value->value;

                if (controller[CONT_ANY].keyLeft.press) {
                    if (value->type == DEBUGDISPLAY_BOOL)
                        *valuePtr ^= 1;
                    else if (*valuePtr - 1 >= value->min)
                        *valuePtr--;
                }

                if (controller[CONT_ANY].keyRight.press) {
                    if (!value->type)
                        *valuePtr ^= 1;
                    else if (*valuePtr + 1 <= value->max)
                        *valuePtr++;
                }
                break;
            }
        }
    }
    else {
        if (confirm) {
            devMenu.state     = DevMenu_OptionsMenu;
            devMenu.selection = 4;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_OptionsMenu;
        devMenu.selection = 4;
    }
#endif
}
#endif

#if RETRO_USE_MOD_LOADER
void DevMenu_ModsMenu()
{
    uint32 selectionColors[]                               = { 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090, 0x808090 };
    selectionColors[devMenu.selection - devMenu.scrollPos] = 0xF0F0F0;

    int32 dy = currentScreen->center.y;
    DrawRectangle(currentScreen->center.x - 128, dy - 84, 0x100, 0x30, 0x80, 0xFF, INK_NONE, true);

    dy -= 68;
    DrawDevString("MANAGE MODS", currentScreen->center.x, dy, ALIGN_CENTER, 0xF0F0F0);
    DrawRectangle(currentScreen->center.x - 128, dy + 36, 0x100, 0x48, 0x80, 0xFF, INK_NONE, true);

    int32 y = dy + 40;
    for (int32 i = 0; i < 8; ++i) {
        if (devMenu.scrollPos + i < RSDK::modList.size()) {
            DrawDevString(RSDK::modList[(devMenu.scrollPos + i)].name.c_str(), currentScreen->center.x - 96, y, ALIGN_LEFT, selectionColors[i]);
            DrawDevString(RSDK::modList[(devMenu.scrollPos + i)].active ? "Y" : "N", currentScreen->center.x + 96, y, ALIGN_RIGHT, selectionColors[i]);

            y += 8;
            devMenu.scrollPos = devMenu.scrollPos;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    DevMenu_HandleTouchControls();
#endif

    int32 preselection = devMenu.selection;
    if (controller[CONT_ANY].keyUp.press) {
        if (--devMenu.selection < 0)
            devMenu.selection = (int32)(RSDK::modList.size() - 1);

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyUp.down) {
        if (!devMenu.timer && --devMenu.selection < 0)
            devMenu.selection = (int32)(RSDK::modList.size() - 1);

        devMenu.timer = (devMenu.timer + 1) & 7;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    if (controller[CONT_ANY].keyDown.press) {
        if (++devMenu.selection >= RSDK::modList.size())
            devMenu.selection = 0;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }

        devMenu.timer = 1;
    }
    else if (controller[CONT_ANY].keyDown.down) {
        if (!devMenu.timer && ++devMenu.selection >= RSDK::modList.size())
            devMenu.selection = 0;

        devMenu.timer = (devMenu.timer + 1) & 7;

        if (devMenu.selection >= devMenu.scrollPos) {
            if (devMenu.selection > devMenu.scrollPos + 7)
                devMenu.scrollPos = devMenu.selection - 7;
        }
        else {
            devMenu.scrollPos = devMenu.selection;
        }
    }

    bool32 confirm = controller[CONT_ANY].keyA.press;
#if RETRO_REV02
    if (RSDK::SKU::userCore->GetConfirmButtonFlip())
#else
    if (RSDK::SKU::GetConfirmButtonFlip())
#endif
        confirm = controller[CONT_ANY].keyB.press;

    if (controller[CONT_ANY].keyStart.press || confirm || controller[CONT_ANY].keyLeft.press || controller[CONT_ANY].keyRight.press) {
        RSDK::modList[devMenu.selection].active ^= true;
        devMenu.modsChanged = true;
    }
    else if (controller[CONT_ANY].keyC.down) {
        RSDK::ModInfo swap               = RSDK::modList[preselection];
        RSDK::modList[preselection]      = RSDK::modList[devMenu.selection];
        RSDK::modList[devMenu.selection] = swap;
        devMenu.modsChanged              = true;
    }
    else if (controller[CONT_ANY].keyB.press) {
        devMenu.state     = DevMenu_MainMenu;
        devMenu.scrollPos = 0;
        devMenu.selection = 4;
        RSDK::SaveMods();
        LoadGameConfig();
    }
}
#endif
