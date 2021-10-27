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
    ManiaModeMenu_Unknown3();
    MainMenu_Unknown3();
    UISubHeading_Unknown3();
    TimeAttackMenu_Unknown3();
    CompetitionMenu_Unknown2();
    OptionsMenu_Unknown3();
    ExtrasMenu_Unknown3();
}

bool32 ManiaModeMenu_InitUserdata(void)
{
    if (!MenuSetup->dword10)
        MenuSetup->fxFade->timer = 512;
    int32 authStatus = API.GetUserAuthStatus();
    if (!authStatus) {
        API.TryAuth();
    }
    else if (authStatus != STATUS_CONTINUE) {
        int32 storageStatus = API.UserStorageStatusUnknown1();
        if (!storageStatus) {
            API.TryInitStorage();
        }
        else if (storageStatus != STATUS_CONTINUE) {
            int32 statusUnknown2 = API.GetSaveStatus();
            if (!API.GetUserStorageNoSave() && (authStatus != STATUS_OK || storageStatus != STATUS_OK)) {
                if (statusUnknown2 != STATUS_CONTINUE) {
                    if (statusUnknown2 != STATUS_FORBIDDEN) {
                        DialogRunner_PromptSavePreference(storageStatus);
                    }
                    else {
                        RSDK.SetScene("Presentation", "Title Screen");
                        RSDK.LoadScene();
                    }
                }
                return false;
            }

            if (!MenuSetup->gameLoaded) {
                UIWaitSpinner_Wait();
                Options_LoadOptionsBin();
                SaveGame_LoadFile();
                ReplayRecorder_LoadReplayDB(NULL);
                MenuSetup->gameLoaded = true;
            }
            if (MenuSetup->dword10)
                return true;
            if (globals->optionsLoaded == STATUS_OK && globals->saveLoaded == STATUS_OK && globals->replayTableLoaded == STATUS_OK
                && globals->taTableLoaded == STATUS_OK) {
                if (!API.GetUserStorageNoSave() && DialogRunner_NotifyAutosave())
                    return false;
                UIWaitSpinner_Wait2();
                if (DialogRunner_CheckUnreadNotifs())
                    return false;
                MenuSetup->dword10 = 1;
                return true;
            }

            if (API.GetUserStorageNoSave()) {
                UIWaitSpinner_Wait2();
                return true;
            }
            else {
                if (globals->optionsLoaded != STATUS_ERROR && globals->saveLoaded != STATUS_ERROR && globals->replayTableLoaded != STATUS_ERROR
                    && globals->taTableLoaded != STATUS_ERROR) {
                }
                else {
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

void ManiaModeMenu_InitLocalization(int32 a1)
{
    if (a1) {
        Localization->loaded = false;
        Localization_LoadStrings();
        UIWidgets_ApplyLanguage();
        UIHeading_LoadSprites();
    }
}

int32 ManiaModeMenu_GetActiveMenu(void)
{
    Entity *control = (Entity *)UIControl_GetUIControl();
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
    TimeAttackData_ClearOptions();
    RSDK.SetScene("Presentation", "Title Screen");
    RSDK.LoadScene();
}

void ManiaModeMenu_Unknown13(void)
{
    RSDK_THIS(MenuSetup);
    entity->fadeTimer = entity->timer << ((entity->field_68 & 0xFF) - 1);
    if (entity->fadeTimer >= 512)
        entity->fadeTimer = 512;
    else if (entity->fadeTimer < 0)
        entity->fadeTimer = 0;
}

void ManiaModeMenu_Unknown3(void)
{
#if RETRO_USE_PLUS
    MainMenu_Unknown2();
    UISubHeading_Unknown2();
    TimeAttackMenu_Unknown2();
    int32 activeCount     = CompetitionMenu_Unknown4();
    EntityUIControl *compRules = (EntityUIControl *)CompetitionMenu->compRulesControl;
    EntityUIButton *button     = UIButton_GetChoicePtr(compRules->buttons[1], compRules->buttons[1]->selection);
    if (button) {
        button->choiceCount = activeCount;
        if (button->align < activeCount)
            activeCount = button->align;
        button->align = activeCount;
    }
    OptionsMenu_Unknown2();
    ExtrasMenu_Unknown2();
#endif
}

void ManiaModeMenu_Unknown7(void)
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
                UIControl_Unknown6(control);
            }
            else {
                control->storedEntityID = param->selectionID;
                control->dwordC4        = 1;
                UIControl_Unknown4(control);
                control->activeEntityID = param->selectionID;
            }
        }
    }

    UISubHeading_Unknown4(0);
    TimeAttackMenu_Unknown4();
    CompetitionMenu_Unknown3();
    OptionsMenu_Unknown4();
    if (param->selectionFlag == 2) {
        EntityUIControl *extras = (EntityUIControl *)ExtrasMenu->extrasControl;
        UIButton_SetChoiceSelection(extras->buttons[1], 1);
    }

    int32 a, b, c, d;
    if (param->clearFlag) {
        a = param->characterID;
        b = param->zoneID;
        c = param->actID;
        d = param->isEncoreMode;
    }
    else {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
    }
    TimeAttackData_ClearOptions();
    if (param->clearFlag) {
        param->characterID = a;
        param->zoneID      = b;
        param->actID       = c;
        param->isEncoreMode   = d;
    }
}

#if RETRO_INCLUDE_EDITOR
void ManiaModeMenu_EditorDraw(void) {}

void ManiaModeMenu_EditorLoad(void) {}
#endif

void ManiaModeMenu_Serialize(void) {}
#endif
