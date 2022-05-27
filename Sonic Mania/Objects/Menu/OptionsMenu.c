// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: OptionsMenu Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

#if MANIA_USE_PLUS
ObjectOptionsMenu *OptionsMenu;

void OptionsMenu_Update(void) {}

void OptionsMenu_LateUpdate(void) {}

void OptionsMenu_StaticUpdate(void) {}

void OptionsMenu_Draw(void) {}

void OptionsMenu_Create(void *data) {}

void OptionsMenu_StageLoad(void) {}

void OptionsMenu_Initialize(void)
{
    LogHelpers_Print("ManiaModeMenu_Initialize()");

    String string;
    INIT_STRING(string);

    foreach_all(UIControl, control)
    {
        RSDK.SetString(&string, "Options");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->optionsControl = control;

        RSDK.SetString(&string, "Language");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->languageControl = control;

        RSDK.SetString(&string, "Language Old");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->languageControl_Legacy = control;

        RSDK.SetString(&string, "Video");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->videoControl = control;

        RSDK.SetString(&string, "Video WIN");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->videoControl_Windows = control;

        RSDK.SetString(&string, "Sound");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->soundControl = control;

        RSDK.SetString(&string, "Controls WIN");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_Windows = control;

        RSDK.SetString(&string, "Controls KB");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_KB = control;

        RSDK.SetString(&string, "Controls PS4");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_PS4 = control;

        RSDK.SetString(&string, "Controls XB1");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_XB1 = control;

        RSDK.SetString(&string, "Controls NX");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_NX = control;

        RSDK.SetString(&string, "Controls NX Grip");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_NXGrip = control;

        RSDK.SetString(&string, "Controls NX Joycon");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_NXJoycon = control;

        RSDK.SetString(&string, "Controls NX Pro");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->controlsControl_NXPro = control;

        RSDK.SetString(&string, "Data Options");
        if (RSDK.CompareStrings(&string, &control->tag, false))
            OptionsMenu->dataOptionsControl = control;
    }

    Hitbox hitbox;
    foreach_all(UIButtonPrompt, prompt)
    {
        EntityUIControl *control = OptionsMenu->optionsControl;
        int32 x                  = control->startPos.x - control->cameraOffset.x;
        int32 y                  = control->startPos.y - control->cameraOffset.y;

        hitbox.right  = control->size.x >> 17;
        hitbox.left   = -(control->size.x >> 17);
        hitbox.bottom = control->size.y >> 17;
        hitbox.top    = -(control->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, prompt->position.x, prompt->position.y, FLIP_NONE, &hitbox) && prompt->buttonID == 3)
            OptionsMenu->helpPrompt = prompt;
    }

    foreach_all(UIDiorama, diorama)
    {
        EntityUIControl *control = OptionsMenu->videoControl;
        int32 x                  = control->startPos.x - control->cameraOffset.x;
        int32 y                  = control->startPos.y - control->cameraOffset.y;

        hitbox.right  = control->size.x >> 17;
        hitbox.left   = -(control->size.x >> 17);
        hitbox.bottom = control->size.y >> 17;
        hitbox.top    = -(control->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, diorama->position.x, diorama->position.y, FLIP_NONE, &hitbox)) {
            OptionsMenu->diorama = diorama;
            diorama->parent      = OptionsMenu->videoControl;
        }
    }

    foreach_all(UIInfoLabel, label)
    {
        EntityUIControl *control = OptionsMenu->dataOptionsControl;
        int32 x                  = control->startPos.x - control->cameraOffset.x;
        int32 y                  = control->startPos.y - control->cameraOffset.y;

        hitbox.right  = control->size.x >> 17;
        hitbox.left   = -(control->size.x >> 17);
        hitbox.bottom = control->size.y >> 17;
        hitbox.top    = -(control->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, label->position.x, label->position.y, FLIP_NONE, &hitbox))
            OptionsMenu->selectDataToEraseLabel = label;
    }
}

void OptionsMenu_HandleUnlocks(void)
{
    EntityUIControl *control = OptionsMenu->dataOptionsControl;

    // Time Attack Data Button
    control->buttons[5]  = control->buttons[4];
    control->buttonCount = 6;

    if (!API.CheckDLC(DLC_PLUS)) {
        EntityUIButton *replaysButton = control->buttons[2];
        replaysButton->invisible      = true;
        replaysButton->visible        = false;

        EntityUIButton *allGameDataButton = control->buttons[3];
        control->buttons[2]               = allGameDataButton;
        allGameDataButton->position.x     = control->buttons[4]->position.x;
        allGameDataButton->size.x         = control->buttons[4]->size.x;
    }
}

void OptionsMenu_SetupActions(void)
{
    EntityUIControl *optionsControl      = OptionsMenu->optionsControl;
    EntityUIControl *languageControl     = OptionsMenu->languageControl;
    EntityUIControl *languageControl_old = OptionsMenu->languageControl_Legacy;
    EntityUIControl *videoControl        = OptionsMenu->videoControl;
    EntityUIControl *controlsControl_Win = OptionsMenu->controlsControl_Windows;
    EntityUIControl *videoControl_Win    = OptionsMenu->videoControl_Windows;
    EntityUIControl *soundControl        = OptionsMenu->soundControl;
    EntityUIControl *dataControl         = OptionsMenu->dataOptionsControl;

    Hitbox hitbox;
    String string;
    INIT_STRING(string);

    foreach_all(UIButton, button)
    {
        int32 x       = controlsControl_Win->startPos.x - controlsControl_Win->cameraOffset.x;
        int32 y       = controlsControl_Win->startPos.y - controlsControl_Win->cameraOffset.y;
        hitbox.right  = controlsControl_Win->size.x >> 17;
        hitbox.left   = -(controlsControl_Win->size.x >> 17);
        hitbox.bottom = controlsControl_Win->size.y >> 17;
        hitbox.top    = -(controlsControl_Win->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox))
            button->actionCB = OptionsMenu_KeyboardIDButton_Win_ActionCB;

        x             = optionsControl->startPos.x - optionsControl->cameraOffset.x;
        y             = optionsControl->startPos.y - optionsControl->cameraOffset.y;
        hitbox.right  = optionsControl->size.x >> 17;
        hitbox.left   = -(optionsControl->size.y >> 17);
        hitbox.bottom = optionsControl->size.x >> 17;
        hitbox.top    = -(optionsControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 3
            && button->frameID == 3) {
            button->actionCB = OptionsMenu_LanguageMenuButton_ActionCB;
        }

        x             = optionsControl->startPos.x - optionsControl->cameraOffset.x;
        y             = optionsControl->startPos.y - optionsControl->cameraOffset.y;
        hitbox.right  = optionsControl->size.x >> 17;
        hitbox.left   = -(optionsControl->size.x >> 17);
        hitbox.bottom = optionsControl->size.y >> 17;
        hitbox.top    = -(optionsControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 3 && !button->frameID)
            button->actionCB = OptionsMenu_VideoMenuButton_ActionCB;

        x             = optionsControl->startPos.x - optionsControl->cameraOffset.x;
        y             = optionsControl->startPos.y - optionsControl->cameraOffset.y;
        hitbox.right  = optionsControl->size.x >> 17;
        hitbox.left   = -(optionsControl->size.x >> 17);
        hitbox.bottom = optionsControl->size.y >> 17;
        hitbox.top    = -(optionsControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 3
            && button->frameID == 1) {
            button->actionCB = OptionsMenu_SoundMenuButton_ActionCB;
        }

        x             = optionsControl->startPos.x - optionsControl->cameraOffset.x;
        y             = optionsControl->startPos.y - optionsControl->cameraOffset.y;
        hitbox.right  = optionsControl->size.x >> 17;
        hitbox.left   = -(optionsControl->size.x >> 17);
        hitbox.bottom = optionsControl->size.y >> 17;
        hitbox.top    = -(optionsControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox)) {
            if (button->listID == 3 && button->frameID == 2) {
                button->actionCB = OptionsMenu_ControlsMenuButton_ActionCB;

                if (sku_platform == PLATFORM_DEV || sku_platform == PLATFORM_PC)
                    button->transition = false;
            }
        }

        x             = optionsControl->startPos.x - optionsControl->cameraOffset.x;
        y             = optionsControl->startPos.y - optionsControl->cameraOffset.y;
        hitbox.right  = optionsControl->size.x >> 17;
        hitbox.left   = -(optionsControl->size.x >> 17);
        hitbox.bottom = optionsControl->size.y >> 17;
        hitbox.top    = -(optionsControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 3
            && button->frameID == 4) {
            button->actionCB = OptionsMenu_DataOptionsMenuButton_ActionCB;
        }

        x             = languageControl->startPos.x - languageControl->cameraOffset.x;
        y             = languageControl->startPos.y - languageControl->cameraOffset.y;
        hitbox.right  = languageControl->size.x >> 17;
        hitbox.left   = -(languageControl->size.x >> 17);
        hitbox.bottom = languageControl->size.y >> 17;
        hitbox.top    = -(languageControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox))
            button->actionCB = OptionsMenu_LanguageButton_ActionCB;

        x             = languageControl_old->startPos.x - languageControl_old->cameraOffset.x;
        y             = languageControl_old->startPos.y - languageControl_old->cameraOffset.y;
        hitbox.right  = languageControl_old->size.x >> 17;
        hitbox.left   = -(languageControl_old->size.x >> 17);
        hitbox.bottom = languageControl_old->size.y >> 17;
        hitbox.top    = -(languageControl_old->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox))
            button->actionCB = OptionsMenu_LanguageButton_ActionCB;

        x             = videoControl->startPos.x - videoControl->cameraOffset.x;
        y             = videoControl->startPos.y - videoControl->cameraOffset.y;
        hitbox.right  = videoControl->size.x >> 17;
        hitbox.left   = -(videoControl->size.x >> 17);
        hitbox.bottom = videoControl->size.y >> 17;
        hitbox.top    = -(videoControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 3 && !button->frameID)
            button->choiceChangeCB = OptionsMenu_ShaderButton_ActionCB;

        x             = controlsControl_Win->startPos.x - controlsControl_Win->cameraOffset.x;
        y             = controlsControl_Win->startPos.y - controlsControl_Win->cameraOffset.y;
        hitbox.right  = controlsControl_Win->size.x >> 17;
        hitbox.left   = -(controlsControl_Win->size.x >> 17);
        hitbox.bottom = controlsControl_Win->size.y >> 17;
        hitbox.top    = -(controlsControl_Win->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 17
            && button->frameID == 1) {
            button->actionCB = OptionsMenu_SetDefaultMappings;
        }

        x             = videoControl_Win->startPos.x - videoControl_Win->cameraOffset.x;
        y             = videoControl_Win->startPos.y - videoControl_Win->cameraOffset.y;
        hitbox.right  = videoControl_Win->size.x >> 17;
        hitbox.left   = -(videoControl_Win->size.x >> 17);
        hitbox.bottom = videoControl_Win->size.y >> 17;
        hitbox.top    = -(videoControl_Win->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 17) {
            switch (button->frameID) {
                case 2: button->choiceChangeCB = OptionsMenu_ShaderButton_ActionCB; break;
                case 7: button->choiceChangeCB = OptionsMenu_WindowScaleButton_ActionCB; break;
                case 13: button->choiceChangeCB = OptionsMenu_BorderlessButton_ActionCB; break;
                case 14: button->choiceChangeCB = OptionsMenu_FullScreenButton_ActionCB; break;
                case 15: button->choiceChangeCB = OptionsMenu_VSyncButton_ActionCB; break;
                case 16: button->choiceChangeCB = OptionsMenu_TripleBufferButton_ActionCB; break;
            }
        }

        x             = dataControl->startPos.x - dataControl->cameraOffset.x;
        y             = dataControl->startPos.y - dataControl->cameraOffset.y;
        hitbox.right  = dataControl->size.x >> 17;
        hitbox.left   = -(dataControl->size.x >> 17);
        hitbox.bottom = dataControl->size.y >> 17;
        hitbox.top    = -(dataControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, button->position.x, button->position.y, FLIP_NONE, &hitbox) && button->listID == 18) {
            switch (button->frameID) {
                case 0: button->actionCB = OptionsMenu_EraseSaveGameButton_ActionCB; break;
                case 1: button->actionCB = OptionsMenu_EraseMedallionsButton_ActionCB; break;
                case 2: button->actionCB = OptionsMenu_EraseTimeAttackButton_ActionCB; break;
                case 3: button->actionCB = OptionsMenu_EraseReplaysButton_ActionCB; break;
                case 4: button->actionCB = OptionsMenu_EraseAllButton_ActionCB; break;
            }
        }
    }

    foreach_all(UISlider, slider)
    {
        int32 x = soundControl->startPos.x - soundControl->cameraOffset.x;
        int32 y = soundControl->startPos.y - soundControl->cameraOffset.y;

        hitbox.right  = soundControl->size.x >> 17;
        hitbox.left   = -(soundControl->size.x >> 17);
        hitbox.bottom = soundControl->size.y >> 17;
        hitbox.top    = -(soundControl->size.y >> 17);
        if (MathHelpers_PointInHitbox(x, y, slider->position.x, slider->position.y, FLIP_NONE, &hitbox) && slider->listID == 5)
            slider->sliderChangedCB = OptionsMenu_UISlider_ChangedCB;
    }

    optionsControl->menuSetupCB = OptionsMenu_MenuSetupCB;

    EntityUIInfoLabel *label = OptionsMenu->selectDataToEraseLabel;
    Localization_GetString(&string, STR_SELECTDATATOERASE);
    UIInfoLabel_SetString(label, &string);

    videoControl_Win->menuUpdateCB = OptionsMenu_VideoControl_Win_MenuUpdateCB;
    videoControl_Win->yPressCB     = OptionsMenu_VideoControl_Win_YPressCB;
    videoControl_Win->backPressCB  = OptionsMenu_VideoControl_Win_BackPressCB;

    if (sku_platform == PLATFORM_SWITCH || sku_platform == PLATFORM_DEV) {
        optionsControl->yPressCB = OptionsMenu_ShowManual;
    }
    else {
        EntityUIButtonPrompt *prompt = OptionsMenu->helpPrompt;
        prompt->visible              = false;
    }
}

void OptionsMenu_HandleMenuReturn(void)
{
    EntityUIControl *languageControl     = OptionsMenu->languageControl;
    EntityUIControl *languageControl_old = OptionsMenu->languageControl_Legacy;
    EntityUIControl *videoControl        = OptionsMenu->videoControl;
    EntityUIControl *soundControl        = OptionsMenu->soundControl;

    if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_DEV)
        OptionsMenu_InitVideoOptionsMenu();

    EntityUIButton *button = videoControl->buttons[0];
    UIButton_SetChoiceSelection(button, RSDK.GetVideoSetting(VIDEOSETTING_SHADERID));

    EntityUISlider *musSlider = (EntityUISlider *)soundControl->buttons[0];
    musSlider->sliderPos      = RSDK.GetVideoSetting(VIDEOSETTING_STREAM_VOL);

    EntityUISlider *sfxSlider = (EntityUISlider *)soundControl->buttons[1];
    sfxSlider->sliderPos      = RSDK.GetVideoSetting(VIDEOSETTING_SFX_VOL);

    languageControl->startingID = Localization->language;
    languageControl->buttonID   = Localization->language;

    languageControl_old->startingID = Localization->language;
    languageControl_old->buttonID   = Localization->language;
}
void OptionsMenu_InitVideoOptionsMenu(void)
{
    if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_DEV) {
        EntityUIControl *videoControl_Win = OptionsMenu->videoControl_Windows;
        EntityOptions *optionsRAM         = (EntityOptions *)globals->optionsRAM;

        Options_GetWinSize();

        int32 options[7];

        options[0] = RSDK.GetVideoSetting(VIDEOSETTING_SHADERID); // filter
        options[1] = optionsRAM->windowSize;                   // window size
        options[2] = RSDK.GetVideoSetting(VIDEOSETTING_BORDERED); // bordered

        // fullscreen
        options[3] = 0;
        if (!RSDK.GetVideoSetting(VIDEOSETTING_WINDOWED) || optionsRAM->windowSize == 4)
            options[3] = 1;

        options[4] = 0;                                              // fullscreen res
        options[5] = RSDK.GetVideoSetting(VIDEOSETTING_VSYNC);          // vsync
        options[6] = RSDK.GetVideoSetting(VIDEOSETTING_TRIPLEBUFFERED); // triple buffered

        for (int32 i = 0; i < videoControl_Win->buttonCount; ++i) {
            EntityUIButton *button = videoControl_Win->buttons[i];

            if (i == 4) {
                EntityUIResPicker *resPicker = (EntityUIResPicker *)UIButton_GetChoicePtr(button, button->selection);
                UIResPicker_GetDisplayInfo(resPicker);
            }
            else if (i == 1) {
                EntityUIWinSize *winSize = (EntityUIWinSize *)UIButton_GetChoicePtr(button, button->selection);
                winSize->selection       = RSDK.GetVideoSetting(VIDEOSETTING_WINDOW_HEIGHT) / SCREEN_YSIZE;
            }
            else if (button->selection != options[i]) {
                UIButton_SetChoiceSelection(button, options[i]);
            }
        }
    }
}

void OptionsMenu_VideoControl_Win_MenuUpdateCB(void) { OptionsMenu_InitVideoOptionsMenu(); }

void OptionsMenu_VideoControl_Win_YPressCB(void)
{
    String message;
    INIT_STRING(message);

    if (RSDK.GetVideoSetting(VIDEOSETTING_CHANGED)) {
        RSDK.UpdateWindow();

        Localization_GetString(&message, STR_VIDEOCHANGESAPPLIED);
        EntityUIDialog *dialog =
            UIDialog_CreateDialogYesNo(&message, OptionsMenu_ApplyChangesDlg_Win_YesCB, OptionsMenu_ApplyChangesDlg_Win_NoCB, true, true);
        if (dialog)
            dialog->closeDelay = 15 * 60; // 15 seconds at 60 FPS
    }
}
void OptionsMenu_DlgRunnerCB_RevertVideoChanges(void)
{
    String message;
    INIT_STRING(message);

    RSDK.UpdateWindow();

    Localization_GetString(&message, STR_VIDEOCHANGESAPPLIED);
    EntityUIDialog *dialog = UIDialog_CreateDialogYesNo(&message, OptionsMenu_ApplyChangesDlg_YesCB, OptionsMenu_ApplyChangesDlg_NoCB, true, true);
    if (dialog)
        dialog->closeDelay = 15 * 60; // 15 seconds at 60 FPS
}

bool32 OptionsMenu_VideoControl_Win_BackPressCB(void)
{
    String message;
    INIT_STRING(message);

    if (RSDK.GetVideoSetting(VIDEOSETTING_CHANGED)) {
        Localization_GetString(&message, STR_APPLYCHANGEDSETTINGS);
        EntityUIDialog *dialog =
            UIDialog_CreateDialogYesNo(&message, OptionsMenu_ApplyChangesDlg_YesCB, OptionsMenu_ApplyChangesDlg_BackPress_NoCB, true, true);
        if (dialog)
            return true;
    }
    else {
        UITransition_StartTransition(UIControl_ReturnToParentMenu, 0);
    }

    return false;
}

void OptionsMenu_ApplyChangesDlg_BackPress_NoCB(void)
{
    EntityDialogRunner *dialogRunner = CREATE_ENTITY(DialogRunner, DialogRunner_HandleCallback, 0, 0);

    dialogRunner->callback    = OptionsMenu_DlgRunnerCB_RevertVideoChanges;
    dialogRunner->timer       = 0;
    dialogRunner->isPermanent = true;
}

void OptionsMenu_ApplyChangesDlg_Win_NoCB(void)
{
    RSDK.SetVideoSetting(VIDEOSETTING_RELOAD, false);
    RSDK.UpdateWindow();

    OptionsMenu_InitVideoOptionsMenu();

    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
}

void OptionsMenu_ApplyChangesDlg_NoCB(void)
{
    RSDK.SetVideoSetting(VIDEOSETTING_RELOAD, false);
    RSDK.UpdateWindow();

    OptionsMenu_InitVideoOptionsMenu();

    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
    UITransition_StartTransition(UIControl_ReturnToParentMenu, 0);
}

void OptionsMenu_ApplyChangesDlg_Win_YesCB(void)
{
    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
    RSDK.SetVideoSetting(VIDEOSETTING_WRITE, true);
    RSDK.SetVideoSetting(VIDEOSETTING_STORE, false);
}

void OptionsMenu_ApplyChangesDlg_YesCB(void)
{
    RSDK.SetVideoSetting(VIDEOSETTING_RELOAD, false);

    OptionsMenu_InitVideoOptionsMenu();

    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
    UITransition_StartTransition(UIControl_ReturnToParentMenu, 0);
}

void OptionsMenu_VideoMenuButton_ActionCB(void)
{
    if (sku_platform == PLATFORM_PC || sku_platform == PLATFORM_DEV) {
        RSDK.SetVideoSetting(VIDEOSETTING_STORE, false);
        UIControl_MatchMenuTag("Video WIN");
    }
    else {
        UIControl_MatchMenuTag("Video");
    }
}
void OptionsMenu_SoundMenuButton_ActionCB(void) { UIControl_MatchMenuTag("Sound"); }
void OptionsMenu_LanguageMenuButton_ActionCB(void) { UIControl_MatchMenuTag("Language"); }
void OptionsMenu_DataOptionsMenuButton_ActionCB(void) { UIControl_MatchMenuTag("Data Options"); }
void OptionsMenu_ControlsMenuButton_DefaultKB_ActionCB(void) { UIControl_MatchMenuTag("Controls WIN"); }
void OptionsMenu_ControlsMenuButton_Default_ActionCB(void) { UIControl_MatchMenuTag("Controls XB1"); }

void OptionsMenu_ControlsMenuButton_ActionCB(void)
{
    int32 id          = API_MostRecentActiveControllerID(true, false, 5);
    int32 gamepadType = API_GetControllerType(id);

    String message;
    INIT_STRING(message);

    switch ((gamepadType >> 8) & 0xFF) {
        default:
        case DEVICE_TYPE_CONTROLLER:
            switch (gamepadType & 0xFF) {
                case DEVICE_XBOX: UITransition_SetNewTag("Controls XB1"); break;
                case DEVICE_PS4: UITransition_SetNewTag("Controls PS4"); break;
                case DEVICE_SATURN: UITransition_SetNewTag("Controls WIN"); break;
                case DEVICE_SWITCH_HANDHELD: UITransition_SetNewTag("Controls NX"); break;
                case DEVICE_SWITCH_JOY_GRIP: UITransition_SetNewTag("Controls NX Grip"); break;
                case DEVICE_SWITCH_JOY_L:
                case DEVICE_SWITCH_JOY_R: UITransition_SetNewTag("Controls NX Joycon"); break;
                case DEVICE_SWITCH_PRO: UITransition_SetNewTag("Controls NX Pro"); break;
                default: UITransition_StartTransition(OptionsMenu_ControlsMenuButton_Default_ActionCB, 0); break;
            }
            break;

        case DEVICE_TYPE_KEYBOARD: UITransition_StartTransition(OptionsMenu_ControlsMenuButton_DefaultKB_ActionCB, 0); break;

        case DEVICE_TYPE_STEAMOVERLAY:
            if (!API.IsOverlayEnabled(id)) {
                Localization_GetString(&message, STR_STEAMOVERLAYUNAVALIABLE);
                UIDialog_CreateDialogOk(&message, StateMachine_None, true);
            }
            break;
    }
}

void OptionsMenu_SetDefaultMappings(void)
{
    ControllerInfo[CONT_P1].keyUp.keyMap    = KEYMAP_UP;
    ControllerInfo[CONT_P1].keyDown.keyMap   = KEYMAP_DOWN;
    ControllerInfo[CONT_P1].keyLeft.keyMap   = KEYMAP_LEFT;
    ControllerInfo[CONT_P1].keyRight.keyMap  = KEYMAP_RIGHT;
    ControllerInfo[CONT_P1].keyA.keyMap      = KEYMAP_A;
    ControllerInfo[CONT_P1].keyB.keyMap      = KEYMAP_S;
    ControllerInfo[CONT_P1].keyC.keyMap      = KEYMAP_NO_MAPPING;
    ControllerInfo[CONT_P1].keyX.keyMap      = KEYMAP_Q;
    ControllerInfo[CONT_P1].keyY.keyMap      = KEYMAP_W;
    ControllerInfo[CONT_P1].keyZ.keyMap      = KEYMAP_NO_MAPPING;
    ControllerInfo[CONT_P1].keyStart.keyMap  = KEYMAP_RETURN;
    ControllerInfo[CONT_P1].keySelect.keyMap = KEYMAP_TAB;

    ControllerInfo[CONT_P2].keyUp.keyMap     = KEYMAP_NUMPAD8;
    ControllerInfo[CONT_P2].keyDown.keyMap   = KEYMAP_NUMPAD5;
    ControllerInfo[CONT_P2].keyLeft.keyMap   = KEYMAP_NUMPAD4;
    ControllerInfo[CONT_P2].keyRight.keyMap  = KEYMAP_NUMPAD6;
    ControllerInfo[CONT_P2].keyA.keyMap      = KEYMAP_J;
    ControllerInfo[CONT_P2].keyB.keyMap      = KEYMAP_K;
    ControllerInfo[CONT_P2].keyC.keyMap      = KEYMAP_NO_MAPPING;
    ControllerInfo[CONT_P2].keyX.keyMap      = KEYMAP_U;
    ControllerInfo[CONT_P2].keyY.keyMap      = KEYMAP_I;
    ControllerInfo[CONT_P2].keyZ.keyMap      = KEYMAP_NO_MAPPING;
    ControllerInfo[CONT_P2].keyStart.keyMap  = KEYMAP_OEM_4;
    ControllerInfo[CONT_P2].keySelect.keyMap = KEYMAP_OEM_6;

    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, true);
}

void OptionsMenu_SetupKBControlsMenu(int32 playerID)
{
    EntityUIControl *control = OptionsMenu->controlsControl_KB;

    foreach_all(UISubHeading, subHeading)
    {
        int32 x = control->startPos.x - control->cameraOffset.x;
        int32 y = control->startPos.y - control->cameraOffset.y;

        Hitbox hitbox;
        hitbox.right  = control->size.x >> 17;
        hitbox.left   = -(control->size.x >> 17);
        hitbox.bottom = control->size.y >> 17;
        hitbox.top    = -(control->size.y >> 17);

        if (MathHelpers_PointInHitbox(x, y, subHeading->position.x, subHeading->position.y, FLIP_NONE, &hitbox)) {
            subHeading->frameID = playerID + 19;
            foreach_break;
        }
    }

    for (int32 b = 0; b < control->buttonCount; ++b) {
        EntityUIKeyBinder *binder = (EntityUIKeyBinder *)control->buttons[b];

        if (binder->classID == UIKeyBinder->classID)
            binder->inputID = playerID;
    }
}

void OptionsMenu_KeyboardIDButton_Win_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityUIControl *control = OptionsMenu->controlsControl_Windows;

    for (int32 i = 0; i < control->buttonCount; ++i) {
        if (self == control->buttons[i]) {
            OptionsMenu_SetupKBControlsMenu(i);
            UIControl_MatchMenuTag("Controls KB");
        }
    }
}

void OptionsMenu_MenuSetupCB(void)
{
    if (Options->changed) {
        UIWaitSpinner_StartWait();
        Options_SaveOptionsBin(OptionsMenu_SaveOptionsCB_Load);
    }

    EntityUIControl *control = OptionsMenu->optionsControl;

    EntityUIButton *eraseDataButton = control->buttons[4];
    if (eraseDataButton)
        eraseDataButton->disabled = API_GetNoSave();
}

void OptionsMenu_SaveOptionsCB_Load(bool32 success) { UIWaitSpinner_FinishWait(); }

void OptionsMenu_TransitionCB_ReloadScene(void)
{
    globals->suppressAutoMusic = true;

    RSDK.ForceHardReset(true);
    RSDK.LoadScene();
}

void OptionsMenu_SaveOptionsCB_Action(bool32 success)
{
    EntityMenuParam *param = (EntityMenuParam *)globals->menuParam;

    UIWaitSpinner_FinishWait();

    EntityUIControl *control = OptionsMenu->languageControl;
    control->startingID      = control->buttonID;

    TimeAttackData_Clear();

    strcpy(param->menuTag, "Options");
    MenuSetup_StartTransition(OptionsMenu_TransitionCB_ReloadScene, 32);
}

void OptionsMenu_LanguageButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityUIControl *control = (EntityUIControl *)self->parent;

    Options_SetLanguage(control->buttonID);
    Localization->language     = control->buttonID;
    control->selectionDisabled = true;

    UIWaitSpinner_StartWait();

    Options_SaveOptionsBin(OptionsMenu_SaveOptionsCB_Action);
}

void OptionsMenu_ShaderButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    options->screenShader   = self->selection;
    options->overrideShader = true;

    RSDK.SetVideoSetting(VIDEOSETTING_SHADERID, self->selection);
    RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
    Options->changed = true;
}

void OptionsMenu_WindowScaleButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;
    if (self->selection != 4) {
        RSDK.SetVideoSetting(VIDEOSETTING_WINDOW_WIDTH, WIDE_SCR_XSIZE * (self->selection + 1));
        RSDK.SetVideoSetting(VIDEOSETTING_WINDOW_HEIGHT, SCREEN_YSIZE * (self->selection + 1));

        options->windowSize = self->selection;
        Options->changed    = true;
    }
}

void OptionsMenu_BorderlessButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    options->windowBorder = self->selection;
    RSDK.SetVideoSetting(VIDEOSETTING_BORDERED, self->selection);

    Options->changed = true;
}

void OptionsMenu_FullScreenButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    options->windowed = self->selection ^ 1;
    RSDK.SetVideoSetting(VIDEOSETTING_WINDOWED, self->selection ^ 1);

    Options->changed = true;
}

void OptionsMenu_VSyncButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    options->vSync = self->selection;
    RSDK.SetVideoSetting(VIDEOSETTING_VSYNC, self->selection);

    Options->changed = true;
}

void OptionsMenu_TripleBufferButton_ActionCB(void)
{
    RSDK_THIS(UIButton);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    options->tripleBuffering = self->selection;
    RSDK.SetVideoSetting(VIDEOSETTING_TRIPLEBUFFERED, self->selection);

    Options->changed = true;
}

void OptionsMenu_UISlider_ChangedCB(void)
{
    RSDK_THIS(UISlider);

    EntityOptions *options = (EntityOptions *)globals->optionsRAM;

    // Bug Details (?):
    // what the hell is up with this???????
    // it'd only ever be 0 or 1 why are 0xF1, 0xF2, 0xF4, 0xF5 & 0xFC options?????
    // this is a CB for the slider why are the boolean values here???

    bool32 value = self->frameID != 1;
    switch (value) {
        case 0xF1:
            options->windowed = self->sliderPos;
            RSDK.SetVideoSetting(VIDEOSETTING_WINDOWED, options->windowed);
            break;

        case 0xF2:
            options->windowBorder = self->sliderPos;
            RSDK.SetVideoSetting(VIDEOSETTING_BORDERED, options->windowBorder);
            break;

        case 0xF4:
            options->vSync = self->sliderPos;
            RSDK.SetVideoSetting(VIDEOSETTING_VSYNC, options->vSync);
            break;

        case 0xF5:
            options->tripleBuffering = self->sliderPos;
            RSDK.SetVideoSetting(VIDEOSETTING_TRIPLEBUFFERED, options->tripleBuffering);
            break;
        case 0xFC:
            options->screenShader   = self->sliderPos;
            options->overrideShader = true;

            RSDK.SetVideoSetting(VIDEOSETTING_SHADERID, options->screenShader);
            RSDK.SetVideoSetting(VIDEOSETTING_CHANGED, false);
            break;

        case 0:
            options->volMusic         = self->sliderPos;
            options->overrideMusicVol = true;
            RSDK.SetVideoSetting(VIDEOSETTING_STREAM_VOL, options->volMusic);
            break;

        case 1:
            options->volSfx         = self->sliderPos;
            options->overrideSfxVol = true;
            RSDK.SetVideoSetting(VIDEOSETTING_SFX_VOL, options->volSfx);
            break;

        default: break;
    }

    Options->changed = true;
}

void OptionsMenu_ShowManual(void)
{
    RSDK.PlaySfx(UIWidgets->sfxAccept, false, 0xFF);
    API_LaunchManual();
}

void OptionsMenu_EraseSaveDataCB(bool32 success)
{
    String message;
    EntityUIControl *control = OptionsMenu->dataOptionsControl;

    UIWaitSpinner_FinishWait();
    if (success) {
        ManiaModeMenu_StartReturnToTitle();
    }
    else {
        control->selectionDisabled = false;
        RSDK.InitString(&message, "ERROR ERASING DATA.", 0);
        UIDialog_CreateDialogOk(&message, StateMachine_None, true);
    }
}

void OptionsMenu_AreYouSureDlg_YesCB_EraseSaveGame(void)
{
    // Bug Details(?):
    // sizeof(globals->noSaveSlot) and sizeof(saveData) is 4096 (sizeof(int32) * 0x400)
    // but the memset size is only 1024 (sizeof(uint8) * 0x400)
    // so only about 1/4th of the save slot is cleared, though nothin uses the extra space so it's not a big deal

    // Mania Mode Slots
    for (int32 i = 0; i < 8; ++i) {
        int32 *saveRAM = SaveGame_GetDataPtr(i, false);
        memset(saveRAM, 0, 0x400);
    }

    // Encore Slots
    for (int32 i = 0; i < 3; ++i) {
        int32 *saveRAM = SaveGame_GetDataPtr(i, true);
        memset(saveRAM, 0, 0x400);
    }

    memset(globals->noSaveSlot, 0, 0x400);
    globals->continues = 0;

    SaveGame_SaveFile(OptionsMenu_EraseSaveDataCB);
}

void OptionsMenu_AreYouSureDlg_YesCB_EraseAllData(void)
{
    // Mania Mode Slots
    for (int32 i = 0; i < 8; ++i) {
        int32 *saveRAM = SaveGame_GetDataPtr(i, false);
        memset(saveRAM, 0, 0x400);
    }

    // Encore Slots
    for (int32 i = 0; i < 3; ++i) {
        int32 *saveRAM = SaveGame_GetDataPtr(i, true);
        memset(saveRAM, 0, 0x400);
    }

    memset(globals->noSaveSlot, 0, 0x400);
    globals->continues = 0;

    GameProgress_ClearProgress();

    API.RemoveAllDBRows(globals->taTableID);

    SaveGame_SaveFile(OptionsMenu_EraseSaveDataCB);
}

void OptionsMenu_EraseSaveGameButton_ActionCB(void)
{
    String message;
    INIT_STRING(message);

    Localization_GetString(&message, STR_AREYOUSURESAVE);
    UIDialog_CreateDialogYesNo(&message, OptionsMenu_AreYouSureDlg_YesCB_EraseSaveGame, StateMachine_None, true, true);
}

void OptionsMenu_AreYouSureDlg_YesCB_EraseMedallions(void)
{
    EntityUIControl *control   = OptionsMenu->dataOptionsControl;
    control->selectionDisabled = true;

    GameProgress_ClearBSSSave();
    SaveGame_SaveFile(OptionsMenu_EraseSaveDataCB);
}

void OptionsMenu_EraseMedallionsButton_ActionCB(void)
{
    String message;
    INIT_STRING(message);

    Localization_GetString(&message, STR_AREYOUSURESAVE);
    UIDialog_CreateDialogYesNo(&message, OptionsMenu_AreYouSureDlg_YesCB_EraseMedallions, StateMachine_None, true, true);
}

void OptionsMenu_AreYouSureDlg_YesCB_EraseTimeAttack(void)
{
    EntityUIControl *control   = OptionsMenu->dataOptionsControl;
    control->selectionDisabled = true;

    UIWaitSpinner_StartWait();
    API.RemoveAllDBRows(globals->taTableID);

    TimeAttackData_SaveTimeAttackDB(OptionsMenu_EraseSaveDataCB);
    LogHelpers_Print("TimeAttack table ID = %d, status = %d", globals->taTableID, globals->taTableLoaded);
}

void OptionsMenu_EraseTimeAttackButton_ActionCB(void)
{
    String message;
    INIT_STRING(message);

    Localization_GetString(&message, STR_AREYOUSURESAVE);
    UIDialog_CreateDialogYesNo(&message, OptionsMenu_AreYouSureDlg_YesCB_EraseTimeAttack, StateMachine_None, true, true);
}

void OptionsMenu_AreYouSureDlg_YesCB_EraseReplays(void)
{
    EntityUIControl *control = OptionsMenu->dataOptionsControl;

    if (!control->selectionDisabled) {
        control->selectionDisabled = true;
        UIWaitSpinner_StartWait();
    }

    API.SetupUserDBRowSorting(globals->replayTableID);
    API.SetupUserDBRowSorting(globals->taTableID);

    if (API.GetSortedUserDBRowCount(globals->replayTableID) <= 0) {
        ReplayRecorder_SaveReplayDB(OptionsMenu_EraseReplaysCB);
    }
    else {
        int32 row = API.GetSortedUserDBRowID(globals->replayTableID, 0);
        ReplayRecorder_DeleteReplay(row, OptionsMenu_EraseReplaysCB, true);
    }
}

void OptionsMenu_EraseReplaysCB(bool32 success) { TimeAttackData_SaveTimeAttackDB(OptionsMenu_EraseSaveDataCB); }

void OptionsMenu_EraseReplaysButton_ActionCB(void)
{
    String message;
    INIT_STRING(message);

    if (API.CheckDLC(DLC_PLUS)) {
        Localization_GetString(&message, STR_AREYOUSURESAVE);
        UIDialog_CreateDialogYesNo(&message, OptionsMenu_AreYouSureDlg_YesCB_EraseReplays, StateMachine_None, true, true);
    }
    else {
        Localization_GetString(&message, STR_ENCOREREQUIRED);
        UIDialog_CreateDialogOk(&message, StateMachine_None, true);
    }
}

void OptionsMenu_EraseAllButton_ActionCB(void)
{
    String message;
    INIT_STRING(message);

    Localization_GetString(&message, STR_AREYOUSURESAVE);
    UIDialog_CreateDialogYesNo(&message, OptionsMenu_AreYouSureDlg_YesCB_EraseAllData, StateMachine_None, true, true);
}

#if RETRO_INCLUDE_EDITOR
void OptionsMenu_EditorDraw(void) {}

void OptionsMenu_EditorLoad(void) {}
#endif

void OptionsMenu_Serialize(void) {}
#endif
