// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: DASetup Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectDASetup *DASetup;

void DASetup_Update(void) {}

void DASetup_LateUpdate(void) {}

void DASetup_StaticUpdate(void)
{
    if (!DASetup->initialized) {
        DASetup_SetupUI();
        DASetup->initialized = true;
    }

    EntityFXFade *fade = (EntityFXFade *)DASetup->fxFade;
    if (DASetup->flag && fade->timer == 512) {
        Music_FadeOut(0.02);
        RSDK.SetScene("Presentation", "Menu");
        RSDK.LoadScene();
    }

    EntityUIControl *control = (EntityUIControl *)DASetup->control;
    if (!control->childHasFocus)
        control->childHasFocus = true;
}

void DASetup_Draw(void) {}

void DASetup_Create(void *data) {}

void DASetup_StageLoad(void)
{

    foreach_all(UIControl, control)
    {
        DASetup->control          = (Entity *)control;
        control->processButtonInputCB = DASetup_State_ManageControl;
    }

    foreach_all(FXFade, fade) { DASetup->fxFade = (Entity *)fade; }

    foreach_all(UIInfoLabel, label)
    {
        if (DASetup->labelB) {
            if (!DASetup->labelA) {
                DASetup->labelA = (Entity *)label;
                foreach_break;
            }
        }
        else {
            DASetup->labelB = (Entity *)label;
        }
    }

    int32 trackCount = 0;
    foreach_all(Music, track) { DASetup->trackList[trackCount++] = (Entity *)track; }

    DASetup->trackCount    = trackCount;
    DASetup->activeTrack   = -1;
    DASetup->sfxEmerald    = RSDK.GetSfx("Special/Emerald.wav");
    DASetup->sfxMedal      = RSDK.GetSfx("Special/Medal.wav");
    DASetup->sfxSSExit     = RSDK.GetSfx("Special/SSExit.wav");
    DASetup->sfxScoreTotal = RSDK.GetSfx("Global/ScoreTotal.wav");
}

void DASetup_DisplayTrack(int32 trackID)
{
    char buffer[0x10];
    TextInfo text;
    INIT_TEXTINFO(text);

    EntityUIInfoLabel *label     = (EntityUIInfoLabel *)DASetup->labelB;
    EntityMusic *trackCountTrack = (EntityMusic *)DASetup->trackList[trackID];

    memset(buffer, 0, 0x10 * sizeof(char));
    strcpy(&buffer[2], " - ");
    buffer[0] = (trackID / 10) + '0';
    buffer[1] = trackID - 10 * (trackID / 10) + '0';
    RSDK.PrependText(&text, buffer);
    RSDK.AppendString(&text, &trackCountTrack->soundTestTitle);
    UIInfoLabel_SetString(label, &text);
}

bool32 DASetup_HandleMedallionDebug(void)
{
    EntityGameProgress *progress = GameProgress_GetGameProgress();
    if (!globals->medallionDebug || !progress)
        return false;

    switch (DASetup->trackID) {
        case 8:
            if (GameProgress_CheckZoneClear()) {
                RSDK.PlaySfx(DASetup->sfxScoreTotal, false, 255);
                return true;
            }
            break;
        case 44:
            if (progress->silverMedalCount < 32) {
                GameProgress_GiveMedal(globals->blueSpheresID, 1);
                GameProgress_ShuffleBSSID();
                RSDK.PlaySfx(DASetup->sfxMedal, false, 255);
                return true;
            }
            else if (progress->goldMedalCount < 32) {
                GameProgress_GiveMedal(globals->blueSpheresID, 2);
                GameProgress_ShuffleBSSID();
                RSDK.PlaySfx(DASetup->sfxMedal, false, 255);
                return true;
            }
            break;
        case 46:
            if (!progress->allGoldMedals) {
                GameProgress_UnlockAllMedals();
                progress->allSpecialCleared = false;
                RSDK.PlaySfx(DASetup->sfxEmerald, false, 255);
                return true;
            }
            break;
        case 48:
            if (progress->silverMedalCount > 0 || progress->zoneClearFlags[0]) {
                GameProgress_ClearProgress();
                progress->allSpecialCleared = false;
                RSDK.PlaySfx(DASetup->sfxSSExit, false, 255);
                return true;
            }
            break;
        default: break;
    }
    return false;
}

void DASetup_SetupUI(void)
{
    TextInfo buffer;
    INIT_TEXTINFO(buffer);

    TitleBG_SetupFX();
    DASetup_DisplayTrack(0);
    EntityUIInfoLabel *label = (EntityUIInfoLabel *)DASetup->labelA;
    Localization_GetString(&buffer, STR_SELECTATRACK);
#if RETRO_USE_PLUS
    RSDK.PrintText(PRINT_NORMAL, &buffer);
#endif
    UIInfoLabel_SetString(label, &buffer);
}

void DASetup_State_ManageControl(void)
{
#if RETRO_USE_TOUCH_CONTROLS
    bool32 up = false, down = false, left = false, right = false, confirm = false, back = false;
    uint8 dir = -1;

    for (int32 t = 0; t < TouchInfo->count; ++t) {
        int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
        int32 ty = (TouchInfo->y[t] * ScreenInfo->height);

        if (TouchInfo->down[t]) {
            if (tx >= 0 && ty >= 96 && tx <= ScreenInfo->centerX && ty <= ScreenInfo->height) {
                int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
                int32 ty = (TouchInfo->y[t] * ScreenInfo->height);
                tx -= 64;
                ty -= 192;

                switch (((RSDK.ATan2(tx, ty) + 32) & 0xFF) >> 6) {
                    case 0:
                        ControllerInfo->keyRight.down |= true;
                        dir = 0;
                        break;
                    case 1:
                        ControllerInfo->keyDown.down |= true;
                        dir = 1;
                        break;
                    case 2:
                        ControllerInfo->keyLeft.down |= true;
                        dir = 2;
                        break;
                    case 3:
                        ControllerInfo->keyUp.down |= true;
                        dir = 3;
                        break;
                }
                break;
            }
        }
    }

    for (int32 t = 0; t < TouchInfo->count; ++t) {
        int32 tx = (TouchInfo->x[t] * ScreenInfo->width);
        int32 ty = (TouchInfo->y[t] * ScreenInfo->height);

        if (TouchInfo->down[t]) {
            if (tx >= ScreenInfo->centerX && ty >= 96 && tx <= ScreenInfo->width && ty <= ScreenInfo->height) {
                ControllerInfo->keyA.down |= true;
                break;
            }

            if (tx >= ScreenInfo->centerX && ty < 96 && tx <= ScreenInfo->width && ty <= ScreenInfo->height) {
                ControllerInfo->keyB.down |= true;
                break;
            }
        }
    }

    if (dir != DASetup->touchDir && ControllerInfo->keyUp.down)
        ControllerInfo->keyUp.press |= ControllerInfo->keyUp.down;

    if (dir != DASetup->touchDir && ControllerInfo->keyDown.down)
        ControllerInfo->keyDown.press |= ControllerInfo->keyDown.down;

    if (dir != DASetup->touchDir && ControllerInfo->keyLeft.down)
        ControllerInfo->keyLeft.press |= ControllerInfo->keyLeft.down;

    if (dir != DASetup->touchDir && ControllerInfo->keyRight.down)
        ControllerInfo->keyRight.press |= ControllerInfo->keyRight.down;

    if (!DASetup->touchConfirm) 
        ControllerInfo->keyA.press |= ControllerInfo->keyA.down;
    if (!DASetup->touchBack)
        ControllerInfo->keyB.press |= ControllerInfo->keyB.down;

    DASetup->touchConfirm = ControllerInfo->keyA.down;
    DASetup->touchBack    = ControllerInfo->keyB.down;
    DASetup->touchDir     = dir;

    up      = ControllerInfo->keyUp.press;
    down    = ControllerInfo->keyDown.press;
    left    = ControllerInfo->keyLeft.press;
    right   = ControllerInfo->keyRight.press;
    confirm = ControllerInfo->keyA.press;
    back    = ControllerInfo->keyB.press;

    int32 prevTrack = DASetup->trackID;
    if (UIControl->keyRight || right)
        DASetup->trackID++;
    else if (UIControl->keyLeft || left)
        DASetup->trackID--;
    else if (UIControl->keyUp || up)
        DASetup->trackID += 10;
    else if (UIControl->keyDown || down)
        DASetup->trackID -= 10;
#else
    int32 prevTrack = DASetup->trackID;
    if (UIControl->keyRight)
        DASetup->trackID++;
    else if (UIControl->keyLeft)
        DASetup->trackID--;
    else if (UIControl->keyUp)
        DASetup->trackID += 10;
    else if (UIControl->keyDown)
        DASetup->trackID -= 10;
#endif

    if (DASetup->trackID < 0)
        DASetup->trackID += DASetup->trackCount;
    if (DASetup->trackID >= DASetup->trackCount)
        DASetup->trackID -= DASetup->trackCount;

    if (prevTrack != DASetup->trackID) {
        RSDK.PlaySfx(UIWidgets->sfxBleep, false, 255);
        DASetup_DisplayTrack(DASetup->trackID);
    }

#if RETRO_USE_TOUCH_CONTROLS
    if (UIControl->keyConfirm || confirm) {
#else
    if (UIControl->keyConfirm) {
#endif
        if (DASetup->activeTrack == DASetup->trackID) {
            RSDK.StopChannel(Music->channelID);
            DASetup->activeTrack = -1;
        }
        else {
            EntityMusic *track = (EntityMusic *)DASetup->trackList[DASetup->trackID];
            if (!DASetup_HandleMedallionDebug()) {
                if (track->trackFile.length) {
                    DASetup->activeTrack = DASetup->trackID;
                    Music_PlayTrackPtr(track);
                }
                else {
                    DASetup->activeTrack = -1;
                    RSDK.StopChannel(Music->channelID);
                }
            }
        }
    }

    if (!DASetup->flag) {
#if RETRO_USE_TOUCH_CONTROLS
        if (UIControl->keyBack || back) {
#else
        if (UIControl->keyBack) {
#endif
            DASetup->flag      = true;
            EntityFXFade *fade = (EntityFXFade *)DASetup->fxFade;
            fade->state        = FXFade_State_FadeIn;
            fade->timer        = 0;
        }
    }
}

#if RETRO_INCLUDE_EDITOR
void DASetup_EditorDraw(void) {}

void DASetup_EditorLoad(void) {}
#endif

void DASetup_Serialize(void) {}
