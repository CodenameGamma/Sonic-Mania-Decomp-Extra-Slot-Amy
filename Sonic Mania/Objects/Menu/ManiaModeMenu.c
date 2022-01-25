// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: ManiaModeMenu Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

#if RETRO_USE_PLUS
ObjectManiaModeMenu *ManiaModeMenu;

void ManiaModeMenu_Update(void) {}

void ManiaModeMenu_LateUpdate(void) {}

void ManiaModeMenu_StaticUpdate(void) {}

void ManiaModeMenu_Draw(void) {}

void ManiaModeMenu_Create(void *data) {}

void ManiaModeMenu_StageLoad(void) {}

void ManiaModeMenu_Initialize(void)
{
    LogHelpers_Print("ManiaModeMenu_Initialize()");
    MainMenu_Initialize();
    UISubHeading_Initialize();
    TimeAttackMenu_Initialize();
    CompetitionMenu_Initialize();
    OptionsMenu_Initialize();
    ExtrasMenu_Initialize();

    ManiaModeMenu_HandleUnlocks();
    ManiaModeMenu_SetupActions();
}

bool32 ManiaModeMenu_InitAPI(void)
{
    if (!MenuSetup->initializedAPI)
        MenuSetup->fxFade->timer = 512;

    int32 authStatus = API.GetUserAuthStatus();
    if (!authStatus) {
        API.TryAuth();
    }
    else if (authStatus != STATUS_CONTINUE) {
        int32 storageStatus = API.GetStorageStatus();
        if (!storageStatus) {
            API.TryInitStorage();
        }
        else if (storageStatus != STATUS_CONTINUE) {
            int32 saveStatus = API.GetSaveStatus();
            if (!checkNoSave && (authStatus != STATUS_OK || storageStatus != STATUS_OK)) {
                if (saveStatus != STATUS_CONTINUE) {
                    if (saveStatus != STATUS_FORBIDDEN) {
                        DialogRunner_PromptSavePreference(storageStatus);
                    }
                    else {
                        RSDK.SetScene("Presentation", "Title Screen");
                        RSDK.LoadScene();
                    }
                }
                return false;
            }

            if (!MenuSetup->initializedSaves) {
                UIWaitSpinner_StartWait();
                Options_LoadOptionsBin();
                SaveGame_LoadFile();
                ReplayRecorder_LoadReplayDB(NULL);
                MenuSetup->initializedSaves = true;
            }

            if (MenuSetup->initializedAPI)
                return true;

            if (globals->optionsLoaded == STATUS_OK && globals->saveLoaded == STATUS_OK && globals->replayTableLoaded == STATUS_OK
                && globals->taTableLoaded == STATUS_OK) {
                if (!checkNoSave && DialogRunner_NotifyAutosave())
                    return false;
                UIWaitSpinner_FinishWait();
                if (DialogRunner_CheckUnreadNotifs())
                    return false;
                MenuSetup->initializedAPI = true;
                return true;
            }

            if (checkNoSave) {
                UIWaitSpinner_FinishWait();
                return true;
            }
            else {
                if (globals->optionsLoaded == STATUS_ERROR || globals->saveLoaded == STATUS_ERROR || globals->replayTableLoaded == STATUS_ERROR
                    || globals->taTableLoaded == STATUS_ERROR) {
                    int32 status = API.GetSaveStatus();
                    if (status != STATUS_CONTINUE) {
                        if (status == STATUS_FORBIDDEN) {
                            RSDK.SetScene("Presentation", "Title Screen");
                            RSDK.LoadScene();
                        }
                        else {
                            DialogRunner_PromptSavePreference(STATUS_CORRUPT);
                        }
                    }
                }
            }
        }
    }
    return false;
}

void ManiaModeMenu_InitLocalization(bool32 success)
{
    if (success) {
        Localization->loaded = false;
        Localization_LoadStrings();
        UIWidgets_ApplyLanguage();
        UIHeading_LoadSprites();
    }
}

int32 ManiaModeMenu_GetActiveMenu(void)
{
    EntityUIControl *control = UIControl_GetUIControl();
    if (control == MainMenu->menuControlPtr || control == ExtrasMenu->extrasControl || control == OptionsMenu->optionsControl
        || control == OptionsMenu->videoControl || control == OptionsMenu->soundControl || control == OptionsMenu->dataOptionsControl
        || control == OptionsMenu->controlsControl_Windows || control == OptionsMenu->controlsControl_KB
        || control == OptionsMenu->controlsControl_PS4 || control == OptionsMenu->controlsControl_XB1 || control == OptionsMenu->controlsControl_NX
        || control == OptionsMenu->controlsControl_NXGrip || control == OptionsMenu->controlsControl_NXJoycon
        || control == OptionsMenu->controlsControl_NXPro) {
        return 0;
    }
    if (control == TimeAttackMenu->timeAttackControl || control == TimeAttackMenu->timeAttackControl_Legacy
        || control == TimeAttackMenu->taZoneSelControl || control == TimeAttackMenu->taDetailsControl
        || control == TimeAttackMenu->leaderboardsControl || control == TimeAttackMenu->replaysControl
        || control == CompetitionMenu->competitionControl || control == CompetitionMenu->competitionControl_Legacy
        || control == CompetitionMenu->compRulesControl || control == CompetitionMenu->compZoneControl) {
        return 1;
    }
    if (control == CompetitionMenu->compRoundControl || control == CompetitionMenu->compTotalControl)
       return 2;
    if (control == ManiaModeMenu->saveSelectMenu || control == ManiaModeMenu->noSaveMenu || control == ManiaModeMenu->secretsMenu) {
        return 3;
    }
    if (control == ManiaModeMenu->encoreSaveSelect || control == ManiaModeMenu->noSaveMenuEncore)
        return 4;
    return 0;
}

void ManiaModeMenu_ChangeMenuTrack(void)
{
    int32 trackID = 0;
    switch (ManiaModeMenu_GetActiveMenu()) {
        case 1: trackID = 1; break;
        case 2: trackID = 2; break;
        case 3: trackID = 3; break;
        case 4: trackID = 4; break;
        default: trackID = 0; break;
    }
    if (!RSDK.ChannelActive(Music->channelID))
        Music_PlayTrack(trackID);
    if (Music->activeTrack != trackID)
        Music_TransitionTrack(trackID, 0.12);
}

int32 ManiaModeMenu_StartReturnToTitle(void)
{
    EntityUIControl *control = UIControl_GetUIControl();
    if (control)
        control->state = StateMachine_None;
    Music_FadeOut(0.05);
    MenuSetup_StartTransition(ManiaModeMenu_ReturnToTitle, 32);
    return 1;
}

void ManiaModeMenu_SetBGColours(void)
{
    switch (ManiaModeMenu_GetActiveMenu()) {
        case 0: UIBackground->activeColours = UIBackground->bgColours; break;
        case 1:
        case 2: UIBackground->activeColours = &UIBackground->bgColours[3]; break;
        case 3: UIBackground->activeColours = &UIBackground->bgColours[6]; break;
        case 4: UIBackground->activeColours = &UIBackground->bgColours[15]; break;
        default: break;
    }
}

void ManiaModeMenu_ReturnToTitle(void)
{
    TimeAttackData_Clear();
    RSDK.SetScene("Presentation", "Title Screen");
    RSDK.LoadScene();
}

void ManiaModeMenu_State_HandleTransition(void)
{
    RSDK_THIS(MenuSetup);
    self->fadeTimer = self->timer << ((self->fadeShift & 0xFF) - 1);
    if (self->fadeTimer >= 512)
        self->fadeTimer = 512;
    else if (self->fadeTimer < 0)
        self->fadeTimer = 0;
}

void ManiaModeMenu_HandleUnlocks(void)
{
    MainMenu_HandleUnlocks();
    UISubHeading_HandleUnlocks();
    TimeAttackMenu_HandleUnlocks();
    int32 activeCount     = CompetitionMenu_HandleUnlocks();
    EntityUIControl *compRules = (EntityUIControl *)CompetitionMenu->compRulesControl;
    EntityUIButton *button     = UIButton_GetChoicePtr(compRules->buttons[1], compRules->buttons[1]->selection);
    if (button) {
        button->choiceCount = activeCount;
        if (button->align < activeCount)
            activeCount = button->align;
        button->align = activeCount;
    }
    OptionsMenu_HandleUnlocks();
    ExtrasMenu_HandleUnlocks();
}

void ManiaModeMenu_SetupActions(void)
{
    MainMenu_SetupActions();
    UISubHeading_SetupActions();
    TimeAttackMenu_SetupActions();
    CompetitionMenu_SetupActions();
    OptionsMenu_SetupActions();
    ExtrasMenu_SetupActions();
}

void ManiaModeMenu_HandleMenuReturn(void)
{
    EntityMenuParam *param = (EntityMenuParam *)globals->menuParam;
    char buffer[0x100];
    memset(buffer, 0, 0x100);
    if (strcmp(param->menuTag, "") == 0)
        UIUsernamePopup_ShowPopup();

    foreach_all(UIControl, control)
    {
        if (strcmp(param->menuTag, "") != 0) {
            RSDK.GetCString(buffer, &control->tag);
            if (strcmp((const char *)buffer, param->menuTag) != 0) {
                UIControl_SetInactiveMenu(control);
            }
            else {
                control->storedButtonID  = param->selectionID;
                control->hasStoredButton = true;
                UIControl_SetActiveMenu(control);
                control->buttonID = param->selectionID;
            }
        }
    }

    UISubHeading_HandleMenuReturn(0);
    TimeAttackMenu_HandleMenuReturn();
    CompetitionMenu_HandleMenuReturn();
    OptionsMenu_HandleMenuReturn();
    if (param->selectionFlag == 2) {
        EntityUIControl *extras = (EntityUIControl *)ExtrasMenu->extrasControl;
        UIButton_SetChoiceSelection(extras->buttons[1], 1);
    }

    int32 characterID = 0, zoneID = 0, act = 0, isEncoreMode = false;
    if (param->inTimeAttack) {
        characterID  = param->characterID;
        zoneID       = param->zoneID;
        act          = param->actID;
        isEncoreMode = param->isEncoreMode;
    }

    TimeAttackData_Clear();

    if (param->inTimeAttack) {
        param->characterID  = characterID;
        param->zoneID       = zoneID;
        param->actID        = act;
        param->isEncoreMode = isEncoreMode;
    }
}

#if RETRO_INCLUDE_EDITOR
void ManiaModeMenu_EditorDraw(void) {}

void ManiaModeMenu_EditorLoad(void) {}
#endif

void ManiaModeMenu_Serialize(void) {}
#endif
